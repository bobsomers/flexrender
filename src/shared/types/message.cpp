#include "types/message.hpp"

#include <iostream>
#include <sstream>
#include <cstdio>

using std::string;
using std::stringstream;
using std::endl;

namespace fr {

Message::Message(Kind kind) :
 kind(kind),
 size(0),
 body(nullptr) {}

Message::Message() :
 kind(Kind::NONE),
 size(0),
 body(nullptr) {}

string ToString(const Message& msg, const string& indent) {
    stringstream stream;
    stream << "Message {" << endl;
    switch (msg.kind) {
        case Message::Kind::NONE:
            stream << indent << "| kind = NONE" << endl;
            break;

        case Message::Kind::OK:
            stream << indent << "| kind = OK" << endl;
            break;

        case Message::Kind::ERROR:
            stream << indent << "| kind = ERROR" << endl;
            break;

        case Message::Kind::INIT:
            stream << indent << "| kind = INIT" << endl;
            break;

        case Message::Kind::SYNC_CONFIG:
            stream << indent << "| kind = SYNC_CONFIG" << endl;
            break;

        case Message::Kind::SYNC_SHADER:
            stream << indent << "| kind = SYNC_SHADER" << endl;
            break;

        case Message::Kind::SYNC_TEXTURE:
            stream << indent << "| kind = SYNC_TEXTURE" << endl;
            break;

        case Message::Kind::SYNC_MATERIAL:
            stream << indent << "| kind = SYNC_MATERIAL" << endl;
            break;

        case Message::Kind::SYNC_MESH:
            stream << indent << "| kind = SYNC_MESH" << endl;
            break;

        case Message::Kind::SYNC_CAMERA:
            stream << indent << "| kind = SYNC_CAMERA" << endl;
            break;

        case Message::Kind::SYNC_IMAGE:
            stream << indent << "| kind = SYNC_IMAGE" << endl;
            break;

        case Message::Kind::RENDER_START:
            stream << indent << "| kind = RENDER_START" << endl;
            break;

        case Message::Kind::RENDER_STOP:
            stream << indent << "| kind = RENDER_STOP" << endl;
            break;

        case Message::Kind::RENDER_STATS:
            stream << indent << "| kind = RENDER_STATS" << endl;
            break;

        case Message::Kind::RAY:
            stream << indent << "| kind = RAY" << endl;
            break;

        default:
            stream << indent << "| kind = ?" << endl;
            break;
    }
    stream << indent << "| size = " << msg.size << endl <<
     indent << "| body = ";
    for (uint32_t i = 0; i < msg.size; i++) {
        uint8_t byte = *(reinterpret_cast<uint8_t*>(
         reinterpret_cast<uintptr_t>(msg.body) +
         static_cast<uintptr_t>(i)));
        char buf[8]; // I/O streams really suck sometimes...
        snprintf(buf, 8, "0x%x ", byte);
        stream << buf;
    }
    stream << endl << indent << "}";
    return stream.str(); 
}

} // namespace fr
