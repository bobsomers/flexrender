#include "engine.hpp"

#include <cassert>
#include <cstdlib>

#include "uv.h"

#include "types.hpp"
#include "utils.hpp"

/// How long to wait for more data before flushing the send buffer.
#define FR_FLUSH_TIMEOUT_MS 100

using std::string;

namespace fr {

/// The library for the current render.
static Library* lib = nullptr;

/// The libuv socket for the server we're running.
static uv_tcp_t host;

/// The server ID of us from everyone else's perspective.
static uint64_t me = 0;

/// The connected renderer who's running the show.
static NetNode* renderer = nullptr;

/// The timer for ensuring we flush the send buffer.
static uv_timer_t flush_timer;

/// The number of other workers we're connected to.
static uint64_t num_workers_connected = 0;

// Callbacks, handlers, and helpers for server functionality.
namespace server {

void Init(const string& ip, uint16_t port);
void DispatchMessage(NetNode* node);

void OnConnection(uv_stream_t* stream, int status);
uv_buf_t OnAlloc(uv_handle_t* handle, size_t suggested_size);
void OnRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf);
//void OnWork(uv_work_t* req);
//void OnStatsTimeout(uv_timer_t* timer, int status);
void OnClose(uv_handle_t* handle);
//void AfterWork(uv_work_t* req);

void OnInit(NetNode* node);
void OnSyncConfig(NetNode* node);
void OnSyncMesh(NetNode* node);
void OnSyncMaterial(NetNode* node);
void OnSyncTexture(NetNode* node);
void OnSyncShader(NetNode* node);
void OnSyncCamera(NetNode* node);
void OnRenderStart(NetNode* node);

} // namespace server

// Callbacks, handlers, and helpers for client functionality.
namespace client {

void Init();
void DispatchMessage(NetNode* node);

void OnConnect(uv_connect_t* req, int status);
uv_buf_t OnAlloc(uv_handle_t* handle, size_t suggested_size);
void OnRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf);
void OnClose(uv_handle_t* handle);

} // namespace client

void OnFlushTimeout(uv_timer_t* timer, int status);

void EngineInit(const string& ip, uint16_t port) {
    int result = 0;

    server::Init(ip, port);

    // Initialize the flush timeout timer.
    result = uv_timer_init(uv_default_loop(), &flush_timer);
    CheckUVResult(result, "timer_init");
    result = uv_timer_start(&flush_timer, OnFlushTimeout, FR_FLUSH_TIMEOUT_MS,
     FR_FLUSH_TIMEOUT_MS);
    CheckUVResult(result, "timer_start");
}

void EngineRun() {
    uv_run(uv_default_loop());
}

void server::Init(const string& ip, uint16_t port) {
    int result = 0;
    struct sockaddr_in addr;

    // Initialize the listening TCP socket.
    result = uv_tcp_init(uv_default_loop(), &host);
    CheckUVResult(result, "tcp_init");

    // Attempt to bind to the desired port.
    addr = uv_ip4_addr(ip.c_str(), port);
    result = uv_tcp_bind(&host, addr);
    CheckUVResult(result, "bind");

    // Attempt to listen on the bound port.
    result = uv_listen(reinterpret_cast<uv_stream_t*>(&host), 128, OnConnection);
    CheckUVResult(result, "listen");
}

void server::OnConnection(uv_stream_t* stream, int status) {
    assert(reinterpret_cast<uv_tcp_t*>(stream) == &host);
    assert(status == 0);

    int result = 0;

    // Create a net node to track the machine that just connected.
    NetNode* node = new NetNode(DispatchMessage);
    result = uv_tcp_init(uv_default_loop(), &node->socket);
    CheckUVResult(result, "tcp_init");

    // Squirrel away the net node in the data baton.
    node->socket.data = node;

    // Accept the connection.
    result = uv_accept(reinterpret_cast<uv_stream_t*>(&host),
     reinterpret_cast<uv_stream_t*>(&node->socket));
    CheckUVResult(result, "accept");

    // Ensure the client doesn't die due to inactivity on the socket.
    result = uv_tcp_keepalive(&node->socket, 1, 60);
    CheckUVResult(result, "tcp_keepalive");

    // Who connected?
    struct sockaddr_in addr;
    int addr_len = sizeof(addr);
    result = uv_tcp_getpeername(&node->socket,
     reinterpret_cast<struct sockaddr*>(&addr), &addr_len);
    CheckUVResult(result, "getpeername");

    char ip[32];
    result = uv_ip4_name(&addr, ip, sizeof(ip));
    CheckUVResult(result, "ip4_name");
    node->ip = string(ip);
    node->port = ntohs(addr.sin_port);

    TOUTLN("[" << node->ip << "] Connected on port " << node->port << ".");

    // Start reading from the client.
    result = uv_read_start(reinterpret_cast<uv_stream_t*>(&node->socket),
     OnAlloc, OnRead);
    CheckUVResult(result, "read_start");
}

uv_buf_t server::OnAlloc(uv_handle_t* handle, size_t suggested_size) {
    assert(handle != nullptr);
    assert(handle->data != nullptr);

    // Just allocate a buffer of the suggested size.
    uv_buf_t buf;
    buf.base = reinterpret_cast<char*>(malloc(suggested_size));
    buf.len = suggested_size;

    return buf;
}

void server::OnRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
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

void server::OnClose(uv_handle_t* handle) {
    assert(handle != nullptr);
    assert(handle->data != nullptr);

    // Pull the net node out of the data baton.
    NetNode* node = reinterpret_cast<NetNode*>(handle->data);

    TOUTLN("[" << node->ip << "] Disconnected.");

    // Net nodes in the library will be deleted when the library is deleted.
    if (node == renderer) {
        delete node;
        renderer = nullptr;
    }
}

void server::DispatchMessage(NetNode* node) {
    switch (node->message.kind) {
        case Message::Kind::INIT:
            OnInit(node);
            break;

        case Message::Kind::SYNC_CONFIG:
            OnSyncConfig(node);
            break;

        case Message::Kind::SYNC_MESH:
            OnSyncMesh(node);
            break;

        case Message::Kind::SYNC_MATERIAL:
            OnSyncMaterial(node);
            break;

        case Message::Kind::SYNC_TEXTURE:
            OnSyncTexture(node);
            break;

        case Message::Kind::SYNC_SHADER:
            OnSyncShader(node);
            break;

        case Message::Kind::SYNC_CAMERA:
            OnSyncCamera(node);
            break;

        case Message::Kind::RENDER_START:
            OnRenderStart(node);
            break;

        default:
            TERRLN("Received unexpected message.");
            TERRLN(ToString(node->message));
            break;
    }
}

void server::OnInit(NetNode* node) {
    assert(node->message.size == sizeof(uint64_t));

    // Who am I?
    me = *(reinterpret_cast<uint64_t*>(node->message.body));
    TOUTLN("[" << node->ip << "] Joining the render as worker " << me << ".");

    // Create a fresh library.
    if (lib != nullptr) delete lib;
    lib = new Library;
    
    // Hang on to the renderer.
    renderer = node;

    // No workers connected yet.
    num_workers_connected = 0;

    // Reply with OK.
    Message reply(Message::Kind::OK);
    node->Send(reply);
}

void server::OnSyncConfig(NetNode* node) {
    assert(node != nullptr);
    assert(lib != nullptr);

    // Unpack the config.
    node->ReceiveConfig(lib);

    TOUTLN("[" << node->ip << "] Received configuration.");

    Config* config = lib->LookupConfig();
    assert(config != nullptr);

    // Create the image with all the requested buffers.
    Image* image = new Image(config->width, config->height);
    for (const auto& buffer : config->buffers) {
        image->AddBuffer(buffer);
    }
    lib->StoreImage(image);

    // Connect to all the other workers.
    client::Init();
}

void server::OnSyncMesh(NetNode* node) {
    assert(node != nullptr);
    assert(lib != nullptr);

    // Unpack the mesh.
    uint64_t id = node->ReceiveMesh(lib);

    // Reply with OK.
    Message reply(Message::Kind::OK);
    node->Send(reply);

    TOUTLN("[" << node->ip << "] Received mesh " << id << ".");
}

void server::OnSyncMaterial(NetNode* node) {
    assert(node != nullptr);
    assert(lib != nullptr);

    // Unpack the material.
    uint64_t id = node->ReceiveMaterial(lib);

    TOUTLN("[" << node->ip << "] Received material " << id << ".");
}

void server::OnSyncTexture(NetNode* node) {
    assert(node != nullptr);
    assert(lib != nullptr);

    // Unpack the texture.
    uint64_t id = node->ReceiveTexture(lib);

    TOUTLN("[" << node->ip << "] Received texture " << id << ".");
}

void server::OnSyncShader(NetNode* node) {
    assert(node != nullptr);
    assert(lib != nullptr);

    // Unpack the shader.
    uint64_t id = node->ReceiveShader(lib);

    TOUTLN("[" << node->ip << "] Received shader " << id << ".");
}

void server::OnSyncCamera(NetNode* node) {
    assert(node != nullptr);
    assert(lib != nullptr);

    // Unpack the camera.
    node->ReceiveCamera(lib);

    // Reply with OK.
    Message reply(Message::Kind::OK);
    node->Send(reply);

    TOUTLN("[" << node->ip << "] Received camera.");
}

void server::OnRenderStart(NetNode* node) {
    assert(node != nullptr);
    assert(lib != nullptr);
    assert(node->message.size == sizeof(uint32_t));

    // Unpack the offset and chunk size.
    uint32_t payload = *(reinterpret_cast<uint32_t*>(node->message.body));
    int16_t offset = (payload & 0xffff0000) >> 16;
    uint16_t chunk_size = payload & 0xffff;

    // TODO: do something with them
    TOUTLN("offset = " << offset);
    TOUTLN("chunk_size = " << chunk_size);
}

void OnFlushTimeout(uv_timer_t* timer, int status) {
    assert(timer == &flush_timer);
    assert(status == 0);

    // Flush the server's connection to the renderer.
    if (renderer != nullptr) {
        if (!renderer->flushed && renderer->nwritten > 0) {
            renderer->Flush();
        }
        renderer->flushed = false;
    }

    // Flush all the client connections.
    if (lib != nullptr) {
        lib->ForEachNetNode([](uint64_t id, NetNode* node) {
            if (!node->flushed && node->nwritten > 0) {
                node->Flush();
            }
            node->flushed = false;
        });
    }

    // Note: This does NOT flush the server's connection to any of its
    // clients, except for the renderer. This is because when we want to write
    // back to one of the server's clients, we write back through our client
    // to that server. If you actually want to write from the server, you
    // need to flush that manually.
}

void client::Init() {
    int result = 0;
    struct sockaddr_in addr;

    Config* config = lib->LookupConfig();

    TOUTLN("Connecting to " << (config->workers.size() - 1) << " other workers...");

    for (size_t i = 0; i < config->workers.size(); i++) {
        // Don't connect to ourselves.
        if (i == me - 1) continue;

        const auto& worker = config->workers[i];

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
}

void client::DispatchMessage(NetNode* node) {

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
    if (num_workers_connected < lib->LookupConfig()->workers.size() - 1) {
        return;
    }

    // Reply with OK to the renderer.
    Message reply(Message::Kind::OK);
    assert(renderer != nullptr);
    renderer->Send(reply);
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
}

} // namespace fr
