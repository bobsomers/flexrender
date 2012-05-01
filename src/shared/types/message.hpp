#pragma once

#include <cstdint>
#include <cstdio>

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
        SYNC_DONE     = 290,
        SYNC_BUFFERS  = 295,
        RAY           = 300,
        STATS         = 400
    };

    explicit Message(Kind kind) :
     kind(kind),
     size(0),
     body(nullptr) {}

    explicit Message() :
     kind(Kind::NONE),
     size(0),
     body(nullptr) {}

    /// The kind of message this is.
    uint32_t kind;

    /// The size of the body.
    uint32_t size;

    /// Pointer to the data that goes in the body of the message. The amount
    /// of data behind this pointer should be the same as size.
    void* body;

    TOSTRINGABLE(Message);
};

inline std::string ToString(const Message& msg, const std::string& indent = "") {
    std::stringstream stream;
    stream << "Message {" << std::endl;
    switch (msg.kind) {
        case Message::Kind::NONE:
            stream << indent << "| kind = NONE" << std::endl;
            break;

        case Message::Kind::OK:
            stream << indent << "| kind = OK" << std::endl;
            break;

        case Message::Kind::ERROR:
            stream << indent << "| kind = ERROR" << std::endl;
            break;

        case Message::Kind::INIT:
            stream << indent << "| kind = INIT" << std::endl;
            break;

        case Message::Kind::SYNC_CONFIG:
            stream << indent << "| kind = SYNC_CONFIG" << std::endl;
            break;

        case Message::Kind::SYNC_SHADER:
            stream << indent << "| kind = SYNC_SHADER" << std::endl;
            break;

        case Message::Kind::SYNC_TEXTURE:
            stream << indent << "| kind = SYNC_TEXTURE" << std::endl;
            break;

        case Message::Kind::SYNC_MATERIAL:
            stream << indent << "| kind = SYNC_MATERIAL" << std::endl;
            break;

        case Message::Kind::SYNC_MESH:
            stream << indent << "| kind = SYNC_MESH" << std::endl;
            break;

        case Message::Kind::SYNC_DONE:
            stream << indent << "| kind = SYNC_DONE" << std::endl;
            break;

        case Message::Kind::SYNC_BUFFERS:
            stream << indent << "| kind = SYNC_BUFFERS" << std::endl;
            break;

        case Message::Kind::RAY:
            stream << indent << "| kind = RAY" << std::endl;
            break;

        case Message::Kind::STATS:
            stream << indent << "| kind = STATS" << std::endl;
            break;

        default:
            stream << indent << "| kind = ?" << std::endl;
            break;
    }
    stream << indent << "| size = " << msg.size << std::endl <<
     indent << "| body = ";
    for (uint32_t i = 0; i < msg.size; i++) {
        uint8_t byte = *(reinterpret_cast<uint8_t*>(
         reinterpret_cast<uintptr_t>(msg.body) +
         static_cast<uintptr_t>(i)));
        char buf[8]; // I/O streams really suck sometimes...
        snprintf(buf, 8, "0x%x ", byte);
        stream << buf;
    }
    stream << std::endl << indent << "}";
    return stream.str(); 
}

} // namespace fr
