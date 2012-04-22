#pragma once

#include <cstdint>
#include <cstring>
#include <limits>
#include <vector>

#include "msgpack.hpp"

#include "utils/tostring.hpp"

namespace fr {

struct Texture {
    enum Kind {
        NONE       = 0,
        PROCEDURAL = 1,
        IMAGE      = 2
    };

    explicit Texture(uint64_t id) :
     id(id),
     kind(Kind::NONE),
     width(0),
     height(0),
     code(""),
     image() {}

    explicit Texture(uint64_t id, const std::string& code) :
     id(id),
     kind(Kind::PROCEDURAL),
     width(0),
     height(0),
     code(code),
     image() {}

    explicit Texture(uint64_t id, int16_t width, int16_t height, const float* data) :
     id(id),
     kind(Kind::IMAGE),
     width(width),
     height(height),
     code("") {
        image.resize(sizeof(float) * width * height);
        memcpy(&image[0], data, sizeof(float) * width * height);
    }

    // FOR MSGPACK ONLY!
    explicit Texture() :
     kind(Kind::NONE),
     width(0),
     height(0),
     code(""),
     image() {
        std::numeric_limits<uint64_t>::max();
    }

    /// Resource ID of the texture.
    uint64_t id;

    /// The kind of texture (from the above possible).
    uint32_t kind;

    /// If the texture is an image, the width (in pixels).
    int16_t width;

    /// If the texture is an image, the height (in pixels).
    int16_t height;

    /// If this texture is procedural, the code we should run.
    std::string code;

    /// If the texture is an image, store flat array of pixel values.
    std::vector<float> image;

    MSGPACK_DEFINE(id, kind, width, height, code, image);

    TOSTRINGABLE(Texture);
};

inline std::string ToString(const Texture& tex, const std::string& indent = "") {
    std::stringstream stream;
    std::string pad = indent + "  ";
    stream << "Texture {" << std::endl <<
     indent << "  id = " << tex.id << std::endl;
    switch (tex.kind) {
        case Texture::Kind::NONE:
            stream << indent << "  kind = NONE" << std::endl;
            break;

        case Texture::Kind::PROCEDURAL:
            stream << indent << "  kind = PROCEDURAL" << std::endl <<
             indent << "  code = ..." << std::endl <<
"======================================================================" << std::endl <<
tex.code << std::endl <<
"======================================================================" << std::endl;
            break;

        case Texture::Kind::IMAGE:
            stream << indent << "  kind = IMAGE" << std::endl <<
             indent << "  width = " << tex.width << std::endl <<
             indent << "  height = " << tex.height << std::endl <<
             indent << "  image = ..." << std::endl <<
"======================================================================" << std::endl;
            for (int16_t y = 0; y < tex.height; y++) {
                if (y > 0) {
                    stream << std::endl;
                }
                for (int16_t x = 0; x < tex.width; x++) {
                    if (x > 0) {
                        stream << "\t";
                    }
                    int32_t index = y * tex.width + x;
                    stream << tex.image[index];
                }
            }
            stream << std::endl <<
"======================================================================" << std::endl;
            break;

        default:
            stream << indent << "  kind = ?" << std::endl;
            break;
    }
    stream << indent << "}";
    return stream.str();
}

} // namespace fr
