#pragma once

#include <cstdint>

#include "utils/tostring.hpp"

namespace fr {

struct Message {
    enum Kind {
        NONE          = 0,
        OK            = 1,
        ERROR         = 2,
        INIT          = 100,
        SYNC_CONFIG   = 200,
        SYNC_SHADER   = 201,
        SYNC_TEXTURE  = 202,
        SYNC_MATERIAL = 203,
        SYNC_MESH     = 204,
        SYNC_CAMERA   = 205,
        SYNC_BUFFER   = 290,
        RENDER_START  = 300,
        RENDER_STOP   = 301,
        RAY           = 400,
        STATS         = 500
    };

    explicit Message(Kind kind);

    explicit Message();

    /// The kind of message this is.
    uint32_t kind;

    /// The size of the body.
    uint32_t size;

    /// Pointer to the data that goes in the body of the message. The amount
    /// of data behind this pointer should be the same as size.
    void* body;

    TOSTRINGABLE(Message);
};

std::string ToString(const Message& msg, const std::string& indent = "");

} // namespace fr
