#pragma once

#include <cstdint>
#include <map>

#include "msgpack.hpp"

#include "utils/tostring.hpp"

namespace fr {

struct Material {
    explicit Material(uint64_t id);

    explicit Material(uint64_t id, uint64_t shader);

    explicit Material(uint64_t id, uint64_t shader, bool emissive);

    // FOR MSGPACK ONLY!
    explicit Material();

    /// Resource ID of the material.
    uint64_t id;

    /// The resource ID of the shader to use for this material.
    uint64_t shader;

    /// Mapping of texture "sampler" names (available to the shader) to resouce
    /// IDs.
    std::map<std::string, uint64_t> textures;

    /// If the shader adds light to the scene, the material is emissive.
    bool emissive;

    MSGPACK_DEFINE(id, shader, textures, emissive);

    TOSTRINGABLE(Material);
};

std::string ToString(const Material& mat, const std::string& indent = "");

} // namespace fr
