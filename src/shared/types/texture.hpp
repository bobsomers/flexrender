#pragma once

#include <cstdint>
#include <vector>

#include "glm/glm.hpp"
#include "msgpack.hpp"

#include "utils/tostring.hpp"

namespace fr {

class TextureScript;

struct Texture {
    enum Kind {
        NONE       = 0,
        PROCEDURAL = 1,
        IMAGE      = 2
    };

    explicit Texture(uint32_t id);

    explicit Texture(uint32_t id, const std::string& code);

    explicit Texture(uint32_t id, int16_t width, int16_t height, const float* data);

    // FOR MSGPACK ONLY!
    explicit Texture();

    /// Resource ID of the texture.
    uint32_t id;

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

    /// Samples the texture at the given texture coordinate.
    float Sample(glm::vec2 texcoord);

    MSGPACK_DEFINE(id, kind, width, height, code, image);

    TOSTRINGABLE(Texture);

    /// The texture script we actually execute (if procedural).
    TextureScript* script;

private:
    /// Samples the image data at texture coordinates <u, v>
    float Image(float u, float v);
};

std::string ToString(const Texture& tex, const std::string& indent = "");

} // namespace fr
