#include "types/texture.hpp"

#include <limits>
#include <cstring>
#include <iostream>
#include <sstream>

#include "scripting/texture_script.hpp"

using std::numeric_limits;
using std::string;
using std::stringstream;
using std::endl;
using glm::vec2;

namespace fr {

Texture::Texture(uint32_t id) :
 id(id),
 kind(Kind::NONE),
 width(0),
 height(0),
 code(""),
 image(),
 script(nullptr) {}

Texture::Texture(uint32_t id, const string& code) :
 id(id),
 kind(Kind::PROCEDURAL),
 width(0),
 height(0),
 code(code),
 image(),
 script(nullptr) {}

Texture::Texture(uint32_t id, int16_t width, int16_t height, const float* data) :
 id(id),
 kind(Kind::IMAGE),
 width(width),
 height(height),
 code(""),
 script(nullptr) {
    image.resize(sizeof(float) * width * height);
    memcpy(&image[0], data, sizeof(float) * width * height);
}

Texture::Texture() :
 kind(Kind::NONE),
 width(0),
 height(0),
 code(""),
 image(),
 script(nullptr) {
    id = numeric_limits<uint32_t>::max();
}

float Texture::Sample(vec2 texcoord) {
    float value = 0.0f;

    switch (kind) {
        case Kind::PROCEDURAL:
            value = script->Evaluate(texcoord);
            break;

        case Kind::IMAGE:
            value = Image(texcoord.x, texcoord.y);
            break;

        default:
            TERRLN("Attempt to sample unknown texture kind!");
            exit(EXIT_FAILURE);
            break;
    }

    return value;
}

float Texture::Image(float u, float v) {
    // This could be a LOT better.
    int16_t x = static_cast<int16_t>(u * (width - 1));
    int16_t y = static_cast<int16_t>(v * (height - 1));
    uint32_t index = y * width + x;
    return image[index];
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
