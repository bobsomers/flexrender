#pragma once

#include <cstdint>
#include <string>
#include <sstream>

#include "uv.h"

#include "types.hpp"

/// The size of the static write buffer.
#define FR_WRITE_BUFFER_SIZE 65536

namespace fr {

struct Server {
    enum class State {
        INIT_SENT,
        READY_TO_SYNC,
        SYNC_SENT,
        SYNC_LIMBO
    };

    explicit Server() :
     nwritten(0),
     flushed(false),
     ip(""),
     port(0) {}
    
    explicit Server(const std::string& ip_port) :
     nwritten(0),
     flushed(false) {
        size_t pos = ip_port.find(':');
        if (pos == std::string::npos) {
            ip = ip_port;
            port = 19400;
        } else {
            ip = ip_port.substr(0, pos);
            std::stringstream stream(ip_port.substr(pos + 1));
            stream >> port;
        }
    }

    /// The TCP socket this server is connected on.
    uv_tcp_t socket;

    /// The state this server is currently in (one of the above).
    State state;

    /// The amount of data we've written to the write buffer.
    ssize_t nwritten;

    /// Has the write buffer been flushed recently?
    bool flushed;

    /// A string representation of the server's IP address.
    std::string ip;

    /// The port the server is running on.
    uint16_t port;

    /// The static write buffer for sending data to this server.
    char buffer[FR_WRITE_BUFFER_SIZE];
};

} // namespace fr
