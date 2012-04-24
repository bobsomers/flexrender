#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <string>
#include <iostream>
#include <sstream>

#include "uv.h"

#include "scripting.hpp"
#include "types.hpp"
#include "utils.hpp"
#include "client.hpp"

using std::string;
using std::stringstream;
using std::cerr;
using std::endl;

using namespace fr;

/// The worker's resource library.
static Library *lib = nullptr;

/// The host's (worker's server) TCP socket.
static uv_tcp_t host;

/// Callbacks and handlers for libuv.
static void Init(const string& ip, uint64_t port);
static void OnConnection(uv_stream_t* stream, int status);
static uv_buf_t OnAlloc(uv_handle_t* handle, size_t suggested_size);
static void OnRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf);
static void OnClose(uv_handle_t* handle);

int main(int argc, char *argv[]) {
    // Grab relevant command line arguments.
    uint16_t port = 19400;
    {
        string port_str = FlagValue(argc, argv, "-p", "--port");
        if (port_str != "") {
            stringstream stream(port_str);
            stream >> port;
        }
    }

    TOUTLN("FlexWorker starting.");

    Init("0.0.0.0", port);
    TOUTLN("Listening on port " << port << ".");

    uv_run(uv_default_loop());

    TOUTLN("FlexWorker done.");
    return EXIT_SUCCESS;
}

void Init(const string& ip, uint64_t port) {
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

void OnConnection(uv_stream_t* stream, int status) {
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

    // Add the client to the library.
    // TODO

    // Start reading from the client.
    result = uv_read_start(reinterpret_cast<uv_stream_t*>(&client->socket),
     OnAlloc, OnRead);
    CheckUVResult(result, "read_start");
}

uv_buf_t OnAlloc(uv_handle_t* handle, size_t suggested_size) {
    assert(handle != nullptr);
    assert(handle->data != nullptr);

    // Just allocate a buffer of the suggested size.
    uv_buf_t buf;
    buf.base = reinterpret_cast<char*>(malloc(suggested_size));
    buf.len = suggested_size;

    return buf;
}

void OnRead(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
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
        // TODO
    }

    if (buf.base) {
        free(buf.base);
    }
}

void OnClose(uv_handle_t* handle) {
    assert(handle != nullptr);
    assert(handle->data != nullptr);

    // Pull the client out of the data baton.
    Client* client = reinterpret_cast<Client*>(handle->data);

    // Remove the client from the library.
    // TODO

    TOUTLN("[" << client->ip << "] Disconnected.");

    delete client;
}
