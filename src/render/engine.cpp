#include "engine.hpp"

#include <cassert>

#include "uv.h"

#include "scripting.hpp"
#include "types.hpp"
#include "utils.hpp"

/// How long to wait for more data before flushing the send buffer.
#define FR_FLUSH_TIMEOUT_MS 100

using std::string;

namespace fr {

/// The library for the current render.
static Library* lib = nullptr;

/// The number of workers that we've connected to.
static size_t num_workers_connected = 0;

/// The number of workers that are ready to sync.
static size_t num_workers_ready = 0;

/// The timer for ensuring we flush the send buffer.
static uv_timer_t flush_timer;

// Callbacks, handlers, and helpers for client functionality.
namespace client {

void Init(const Config& config);
void DispatchMessage(NetNode* node);

void OnConnect(uv_connect_t* req, int status);
uv_buf_t OnAlloc(uv_handle_t* handle, size_t suggested_size);
void OnRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf);
void OnClose(uv_handle_t* handle);
void OnFlushTimeout(uv_timer_t* timer, int status);

void OnOK(NetNode* node);

}

void EngineInit(const string& config_file, const string& scene_file) {
    lib = new Library;

    // Parse the config file.
    ConfigScript config_script;
    TOUTLN("Loading config from " << config_file << ".");
    if (!config_script.Parse(config_file, lib)) {
        TERRLN("Can't continue with bad config.");
        exit(EXIT_FAILURE);
    }
    TOUTLN("Config loaded.");

    client::Init(*lib->LookupConfig());

    // Parse and distribute the scene.
    // TODO: do this in a a uv_work thread after connected to all workers
    //SceneScript scene_script;
    //TOUTLN("Loading scene from " << scene_file << ".");
    //if (!scene_script.Parse(scene_file, lib)) {
    //    TERRLN("Can't continue with bad scene.");
    //    exit(EXIT_FAILURE);
    //}
    //TOUTLN("Scene loaded.");
}

void EngineRun() {
    uv_run(uv_default_loop());
}

void client::Init(const Config& config) {
    int result = 0;
    struct sockaddr_in addr;

    TOUTLN("Connecting to " << config.workers.size() << " workers...");

    for (size_t i = 0; i < config.workers.size(); i++) {
        const auto& worker = config.workers[i];

        NetNode* node = new NetNode(DispatchMessage, worker);

        // Initialize the TCP socket.
        result = uv_tcp_init(uv_default_loop(), &node->socket);
        CheckUVResult(result, "tcp_init");
        
        // Squirrel away the node in the data baton.
        node->socket.data = node;

        // Connect to the server.
        addr = uv_ip4_addr(node->ip.c_str(), node->port);
        uv_connect_t* req = reinterpret_cast<uv_connect_t*>(malloc(sizeof(uv_connect_t)));
        result = uv_tcp_connect(req, &node->socket, addr, OnConnect);
        CheckUVResult(result, "tcp_connect");

        // Add the node to the library.
        lib->StoreNetNode(i + 1, node); // +1 because 0 is reserved
    }

    // Initialize the flush timeout timer.
    result = uv_timer_init(uv_default_loop(), &flush_timer);
    CheckUVResult(result, "timer_init");
    result = uv_timer_start(&flush_timer, OnFlushTimeout, FR_FLUSH_TIMEOUT_MS,
     FR_FLUSH_TIMEOUT_MS);
    CheckUVResult(result, "timer_start");
}

void client::DispatchMessage(NetNode* node) {
    switch (node->message.kind) {
        case Message::Kind::OK:
            OnOK(node);
            break;

        default:
            TERRLN("Received unexpected message.");
            TERRLN(ToString(node->message));
            break;
    }
}

void client::OnConnect(uv_connect_t* req, int status) {
    assert(req != nullptr);
    assert(req->handle != nullptr);
    assert(req->handle->data != nullptr);

    int result = 0;

    // Pull the net node out of the data baton.
    NetNode* node = reinterpret_cast<NetNode*>(req->handle->data);
    free(req);

    if (status != 0) {
        TERRLN("Failed connecting to " << node->ip << ".");
        exit(EXIT_FAILURE);
    }

    TOUTLN("[" << node->ip << "] Connected on port " << node->port << ".");

    // Start reading replies from the server.
    result = uv_read_start(reinterpret_cast<uv_stream_t*>(&node->socket),
     OnAlloc, OnRead);
    CheckUVResult(result, "read_start");

    // Nothing else to do if we're still waiting for everyone to connect.
    num_workers_connected++;
    if (num_workers_connected < lib->LookupConfig()->workers.size()) {
        return;
    }

    // Send init messages to each server.
    lib->ForEachNetNode([](uint64_t id, NetNode* node) {
        Message request(Message::Kind::INIT);
        request.size = sizeof(uint64_t);
        request.body = &id;
        node->Send(request);
        node->state = NetNode::State::INITIALIZING;
    });
}

uv_buf_t client::OnAlloc(uv_handle_t* handle, size_t suggested_size) {
    assert(handle != nullptr);
    assert(handle->data != nullptr);

    // Just allocate a buffer of the suggested size.
    uv_buf_t buf;
    buf.base = reinterpret_cast<char*>(malloc(suggested_size));
    buf.len = suggested_size;

    return buf;
}

void client::OnRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
    assert(stream != nullptr);
    assert(stream->data != nullptr);

    // Pull the net node out of the data baton.
    NetNode* node = reinterpret_cast<NetNode*>(stream->data);

    if (nread < 0) {
        // No data was read.
        uv_err_t err = uv_last_error(uv_default_loop());
        if (err.code == UV_EOF) {
            uv_close(reinterpret_cast<uv_handle_t*>(&node->socket), OnClose);
        } else {
            TERRLN("read: " << uv_strerror(err));
        }
    } else if (nread > 0) {
        // Data is available, parse any new messages out.
        node->Receive(buf.base, nread);
    }

    if (buf.base) {
        free(buf.base);
    }
}

void client::OnClose(uv_handle_t* handle) {
    assert(handle != nullptr);
    assert(handle->data != nullptr);

    // Pull the net node out of the data baton.
    NetNode* node = reinterpret_cast<NetNode*>(handle->data);

    TOUTLN("[" << node->ip << "] Disconnected.");

    // Net node will be deleted with library.
    // TODO: halt rendering if it wasn't done
}

void client::OnFlushTimeout(uv_timer_t* timer, int status) {
    assert(timer == &flush_timer);
    assert(status == 0);

    lib->ForEachNetNode([](uint64_t id, NetNode* node) {
        if (!node->flushed && node->nwritten > 0) {
            node->Flush();
        }
        node->flushed = false;
    });
}

void client::OnOK(NetNode* node) {
    switch (node->state) {
        case NetNode::State::INITIALIZING:
            node->state = NetNode::State::READY;
            TOUTLN("[" << node->ip << "] Worker is ready.");
            num_workers_ready++;
            if (num_workers_ready == lib->LookupConfig()->workers.size()) {
                TOUTLN("SYNC TIEM!");
                // TODO: begin syncing
            }
            break;

        default:
            TERRLN("Received OK in unexpected state.");
    }
}

} // namespace fr
