#pragma once

#include <cstdint>
#include <string>

#include "uv.h"

#include "types.hpp"

namespace fr {

struct Client {
    enum class ReadMode {
        HEADER,
        BODY
    };

    explicit Client() :
     message(),
     mode(ReadMode::HEADER),
     nread(0) {}

    /// The TCP socket this client is connected on.
    uv_tcp_t socket;

    /// The message we're in the middle of reading.
    Message message;

    /// The current read mode of the client.
    ReadMode mode;

    /// The amount of data this client has read.
    ssize_t nread;

    /// A string representation of the client's IP address.
    std::string ip;

    /// The port the client connected from.
    uint16_t port;
};

} // namespace fr
