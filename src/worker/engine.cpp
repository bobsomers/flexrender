#include "engine.hpp"

#include <cassert>
#include <cstdlib>

#include <iostream>

#include "uv.h"

#include "client.hpp"
#include "utils.hpp"

using std::string;

using std::cout;
using std::endl;

namespace fr {

static Library* lib = nullptr;
static uv_tcp_t host;

// Callbacks, handlers, and helpers for server functionality.
namespace server {

void Init(const string& ip, uint16_t port);
void ParseMessages(Client* client, const char* buf, ssize_t len);
void DispatchMessage(const Message& message);

void OnConnection(uv_stream_t* stream, int status);
uv_buf_t OnAlloc(uv_handle_t* handle, size_t suggested_size);
void OnRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf);
//void OnWork(uv_work_t* req);
//void OnStatsTimeout(uv_timer_t* timer, int status);
void OnClose(uv_handle_t* handle);

//void AfterWork(uv_work_t* req);

} // namespace server

// Callbacks, handlers, and helpers for client functionality.
namespace client {

//void Init(const Config& config);
//void Send(Server* server, const Message& message);
//void Flush(Server* server);

//void OnConnect(uv_connect_t* req, int status);
//uv_buf_t OnAlloc(uv_handle_t* handle, size_t suggested_size);
//void OnRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf);
//void OnFlushTimeout(uv_timer_t* timer, int status);
//void OnClose(uv_handle_t* handle);

//void AfterFlush(uv_write_t* req, int status);

} // namespace client

void EngineInit(const string& ip, uint16_t port) {
    server::Init(ip, port);
}

void EngineRun() {
    uv_run(uv_default_loop());
}

void server::Init(const string& ip, uint16_t port) {
    int result = 0;
    struct sockaddr_in host_addr;

    // Initialize the listening TCP socket.
    result = uv_tcp_init(uv_default_loop(), &host);
    CheckUVResult(result, "tcp_init");

    // Attempt to bind to the desired port.
    host_addr = uv_ip4_addr(ip.c_str(), port);
    result = uv_tcp_bind(&host, host_addr);
    CheckUVResult(result, "bind");

    // Attempt to listen on the bound port.
    result = uv_listen(reinterpret_cast<uv_stream_t*>(&host), 128, OnConnection);
    CheckUVResult(result, "listen");
}

void server::OnConnection(uv_stream_t* stream, int status) {
    assert(reinterpret_cast<uv_tcp_t*>(stream) == &host);
    assert(status == 0);

    int result = 0;

    // Create a client to track the machine that just connected.
    Client* client = new Client;
    result = uv_tcp_init(uv_default_loop(), &client->socket);
    CheckUVResult(result, "tcp_init");

    // Squirrel away the client in the data baton.
    client->socket.data = client;

    // Accept the connection.
    result = uv_accept(reinterpret_cast<uv_stream_t*>(&host),
     reinterpret_cast<uv_stream_t*>(&client->socket));
    CheckUVResult(result, "accept");

    // Ensure the client doesn't die due to inactivity on the socket.
    result = uv_tcp_keepalive(&client->socket, 1, 60);
    CheckUVResult(result, "tcp_keepalive");

    // Who connected?
    struct sockaddr_in addr;
    int addr_len = sizeof(addr);
    result = uv_tcp_getpeername(&client->socket,
     reinterpret_cast<struct sockaddr*>(&addr), &addr_len);
    CheckUVResult(result, "getpeername");

    char ip[32];
    result = uv_ip4_name(&addr, ip, sizeof(ip));
    CheckUVResult(result, "ip4_name");
    client->ip = string(ip);
    client->port = ntohs(addr.sin_port);

    TOUTLN("[" << client->ip << "] Connected on port " << client->port << ".");

    // Start reading from the client.
    result = uv_read_start(reinterpret_cast<uv_stream_t*>(&client->socket),
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

    // Pull the client out of the data baton.
    Client* client = reinterpret_cast<Client*>(stream->data);

    if (nread < 0) {
        // No data was read.
        uv_err_t err = uv_last_error(uv_default_loop());
        if (err.code == UV_EOF) {
            uv_close(reinterpret_cast<uv_handle_t*>(&client->socket), OnClose);
        } else {
            TERRLN("read: " << uv_strerror(err));
        }
    } else if (nread > 0) {
        // Data is available, parse any new messages out.
        ParseMessages(client, buf.base, nread);
    }

    if (buf.base) {
        free(buf.base);
    }
}

void server::OnClose(uv_handle_t* handle) {
    assert(handle != nullptr);
    assert(handle->data != nullptr);

    // Pull the client out of the data baton.
    Client* client = reinterpret_cast<Client*>(handle->data);

    TOUTLN("[" << client->ip << "] Disconnected.");

    delete client;
}

void server::ParseMessages(Client* client, const char* buf, ssize_t len) {
    assert(client != nullptr);

    if (buf == nullptr || len <= 0) {
        return;
    }

    const char* from = buf;
    ssize_t remaining = len;

    while (true) {
        if (client->mode == Client::ReadMode::HEADER) {
            char* to = reinterpret_cast<char*>(
             reinterpret_cast<uintptr_t>(&client->message) +
             static_cast<uintptr_t>(client->nread));

            ssize_t bytes_to_go = (sizeof(client->message.kind) +
             sizeof(client->message.size)) - client->nread;

            if (bytes_to_go <= remaining) {
                memcpy(to, from, bytes_to_go);

                remaining -= bytes_to_go;

                from = reinterpret_cast<const char*>(
                 reinterpret_cast<uintptr_t>(from) +
                 static_cast<uintptr_t>(bytes_to_go));

                client->nread = 0;
                if (client->message.size > 0) {
                    client->message.body = malloc(client->message.size);
                } else {
                    client->message.body = nullptr;
                    client->message.size = 0;
                }

                client->mode = Client::ReadMode::BODY;
                continue;
            } else {
                memcpy(to, from, remaining);
                client->nread += remaining;
                break;
            }
        } else {
            char* to = reinterpret_cast<char*>(
             reinterpret_cast<uintptr_t>(client->message.body) +
             static_cast<uintptr_t>(client->nread));

            ssize_t bytes_to_go = client->message.size - client->nread;

            if (bytes_to_go <= remaining) {
                memcpy(to, from, bytes_to_go);

                remaining -= bytes_to_go;

                from = reinterpret_cast<const char*>(
                 reinterpret_cast<uintptr_t>(from) +
                 static_cast<uintptr_t>(bytes_to_go));

                client->nread = 0;
                DispatchMessage(client->message);

                client->mode = Client::ReadMode::HEADER;
                continue;
            } else {
                memcpy(to, from, remaining);
                client->nread += remaining;
                break;
            }
        }
    }
}

void server::DispatchMessage(const Message& message) {
    TOUTLN(ToString(message));
    // TODO: actually implement this
}

} // namespace fr
