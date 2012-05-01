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

static Library* lib = nullptr;
static size_t num_workers_connected = 0;
static uv_timer_t flush_timer;

// Callbacks, handlers, and helpers for client functionality.
namespace client {

void Init(const Config& config);
void Send(Server* server, const Message& message);
void Flush(Server* server);

void OnConnect(uv_connect_t* req, int status);
uv_buf_t OnAlloc(uv_handle_t* handle, size_t suggested_size);
void OnRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf);
void OnClose(uv_handle_t* handle);
void AfterFlush(uv_write_t* req, int status);
void OnFlushTimeout(uv_timer_t* timer, int status);

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
    struct sockaddr_in server_addr;

    TOUTLN("Connecting to " << config.workers.size() << " workers...");

    for (size_t i = 0; i < config.workers.size(); i++) {
        const auto& worker = config.workers[i];

        Server* server = new Server(worker);

        // Initialize the TCP socket.
        result = uv_tcp_init(uv_default_loop(), &server->socket);
        CheckUVResult(result, "tcp_init");
        
        // Squirrel away the server in the data baton.
        server->socket.data = server;

        // Connect to the server.
        server_addr = uv_ip4_addr(server->ip.c_str(), server->port);
        uv_connect_t* req = reinterpret_cast<uv_connect_t*>(malloc(sizeof(uv_connect_t)));
        result = uv_tcp_connect(req, &server->socket, server_addr, OnConnect);
        CheckUVResult(result, "tcp_connect");

        // Add the server to the library.
        lib->StoreServer(i + 1, server); // +1 because 0 is reserved
    }

    // Initialize the flush timeout timer.
    result = uv_timer_init(uv_default_loop(), &flush_timer);
    CheckUVResult(result, "timer_init");
    result = uv_timer_start(&flush_timer, OnFlushTimeout, FR_FLUSH_TIMEOUT_MS,
     FR_FLUSH_TIMEOUT_MS);
    CheckUVResult(result, "timer_start");
}

void client::Send(Server* server, const Message& message) {
    ssize_t bytes_sent = 0;
    ssize_t space_left = 0;

    ssize_t bytes_remaining = sizeof(message.kind) + sizeof(message.size);
    if (server->nwritten + bytes_remaining > FR_WRITE_BUFFER_SIZE) {
        Flush(server);
    }

    char* to = reinterpret_cast<char*>(
     reinterpret_cast<uintptr_t>(&server->buffer) +
     static_cast<uintptr_t>(server->nwritten));

    const char* from = reinterpret_cast<const char*>(&message);

    memcpy(to, from, sizeof(message.kind) + sizeof(message.size));
    server->nwritten += sizeof(message.kind) + sizeof(message.size);

    while (true) {
        bytes_remaining = message.size - bytes_sent;
        space_left = FR_WRITE_BUFFER_SIZE - server->nwritten;

        to = reinterpret_cast<char*>(
         reinterpret_cast<uintptr_t>(&server->buffer) +
         static_cast<uintptr_t>(server->nwritten));

        from = reinterpret_cast<const char*>(
         reinterpret_cast<uintptr_t>(message.body) +
         static_cast<uintptr_t>(bytes_sent));

        if (bytes_remaining <= space_left) {
            memcpy(to, from, bytes_remaining);
            server->nwritten += bytes_remaining;
            bytes_sent += bytes_remaining;
            break;
        }

        if (space_left > 0) {
            memcpy(to, from, space_left);
            server->nwritten += space_left;
            bytes_sent += space_left;
        }

        Flush(server);
    }
}

void client::Flush(Server* server) {
    int result = 0;

    if (server->nwritten <= 0) return;

    uv_write_t* req = reinterpret_cast<uv_write_t*>(malloc(sizeof(uv_write_t)));
    req->data = malloc(FR_WRITE_BUFFER_SIZE);
    memcpy(req->data, &server->buffer, server->nwritten);

    uv_buf_t buf;
    buf.base = reinterpret_cast<char*>(req->data);
    buf.len = server->nwritten;

    result = uv_write(req, reinterpret_cast<uv_stream_t*>(&server->socket),
     &buf, 1, AfterFlush);
    CheckUVResult(result, "write");

    server->flushed = true;
    server->nwritten = 0;
}

void client::OnConnect(uv_connect_t* req, int status) {
    assert(req != nullptr);
    assert(req->handle != nullptr);
    assert(req->handle->data != nullptr);

    int result = 0;

    // Pull the server out of the data baton.
    Server* server = reinterpret_cast<Server*>(req->handle->data);
    free(req);

    if (status != 0) {
        TERRLN("Failed connecting to " << server->ip << ".");
        exit(EXIT_FAILURE);
    }

    TOUTLN("[" << server->ip << "] Connected on port " << server->port << ".");

    // Nothing else to do if we're still waiting for everyone to connect.
    num_workers_connected++;
    if (num_workers_connected < lib->LookupConfig()->workers.size()) {
        return;
    }

    // Start reading replies from the servers.
    result = uv_read_start(reinterpret_cast<uv_stream_t*>(&server->socket),
     OnAlloc, OnRead);
    CheckUVResult(result, "read_start");

    // Send init messages to each server.
    lib->ForEachServer([](uint64_t id, Server* server) {
        Message msg(Message::Kind::INIT);
        msg.size = sizeof(uint64_t);
        msg.body = &id;
        Send(server, msg);
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

    // Pull the server out of the data baton.
    Server* server = reinterpret_cast<Server*>(stream->data);

    if (nread < 0) {
        // No data was read.
        uv_err_t err = uv_last_error(uv_default_loop());
        if (err.code == UV_EOF) {
            uv_close(reinterpret_cast<uv_handle_t*>(&server->socket), OnClose);
        } else {
            TERRLN("read: " << uv_strerror(err));
        }
    } else if (nread > 0) {
        // Data is available, parse any new messages out.
        //ParseMessages(client, buf.base, nread);
        // TODO
    }

    if (buf.base) {
        free(buf.base);
    }
}

void client::OnClose(uv_handle_t* handle) {
    assert(handle != nullptr);
    assert(handle->data != nullptr);

    // Pull the server out of the data baton.
    Server* server = reinterpret_cast<Server*>(handle->data);

    TOUTLN("[" << server->ip << "] Disconnected.");

    // Server will be deleted with library.
    // TODO: halt rendering if it wasn't done
}

void client::AfterFlush(uv_write_t* req, int status) {
    assert(req != nullptr);
    assert(req->data != nullptr);
    assert(status == 0);

    free(req->data);
    free(req);
}

void client::OnFlushTimeout(uv_timer_t* timer, int status) {
    assert(timer == &flush_timer);
    assert(status == 0);

    lib->ForEachServer([](uint64_t id, Server* server) {
        if (!server->flushed && server->nwritten > 0) {
            Flush(server);
        }
        server->flushed = false;
    });
}

} // namespace fr
