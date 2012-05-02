#include "types/texture.hpp"

#include <limits>
#include <cstring>
#include <iostream>
#include <sstream>

using std::numeric_limits;
using std::string;
using std::stringstream;
using std::endl;

namespace fr {

Texture::Texture(uint64_t id) :
 id(id),
 kind(Kind::NONE),
 width(0),
 height(0),
 code(""),
 image() {}

Texture::Texture(uint64_t id, const string& code) :
 id(id),
 kind(Kind::PROCEDURAL),
 width(0),
 height(0),
 code(code),
 image() {}

Texture::Texture(uint64_t id, int16_t width, int16_t height, const float* data) :
 id(id),
 kind(Kind::IMAGE),
 width(width),
 height(height),
 code("") {
    image.resize(sizeof(float) * width * height);
    memcpy(&image[0], data, sizeof(float) * width * height);
}

Texture::Texture() :
 kind(Kind::NONE),
 width(0),
 height(0),
 code(""),
 image() {
    id = numeric_limits<uint64_t>::max();
}

string ToString(const Texture& tex, const string& indent) {
    stringstream stream;
    stream << "Texture {" << endl <<
     indent << "| id = " << tex.id << endl;
    switch (tex.kind) {
        case Texture::Kind::NONE:
            stream << indent << "| kind = NONE" << endl;
            break;

        case Texture::Kind::PROCEDURAL:
            stream << indent << "| kind = PROCEDURAL" << endl <<
             indent << "| code = ..." << endl <<
"======================================================================" << endl <<
tex.code << endl <<
"======================================================================" << endl;
            break;

        case Texture::Kind::IMAGE:
            stream << indent << "| kind = IMAGE" << endl <<
             indent << "| width = " << tex.width << endl <<
             indent << "| height = " << tex.height << endl <<
             indent << "| image = ..." << endl <<
"======================================================================" << endl;
            for (int16_t y = 0; y < tex.height; y++) {
                if (y > 0) {
                    stream << endl;
                }
                for (int16_t x = 0; x < tex.width; x++) {
                    if (x > 0) {
                        stream << "\t";
                    }
                    int32_t index = y * tex.width + x;
                    stream << tex.image[index];
                }
            }
            stream << endl <<
"======================================================================" << endl;
            break;

        default:
            stream << indent << "| kind = ?" << endl;
            break;
    }
    stream << indent << "}";
    return stream.str();
}

} // namespace fr
