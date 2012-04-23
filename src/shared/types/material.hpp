#pragma once

#include <cstdint>
#include <map>
#include <limits>

#include "msgpack.hpp"

#include "utils/tostring.hpp"

namespace fr {

struct Material {
    explicit Material(uint64_t id) :
     id(id),
     textures(),
     emissive(false) {
        shader = std::numeric_limits<uint64_t>::max();
    }

    explicit Material(uint64_t id, uint64_t shader) :
     id(id),
     shader(shader),
     textures(),
     emissive(false) {}

    explicit Material(uint64_t id, uint64_t shader, bool emissive) :
     id(id),
     shader(shader),
     textures(),
     emissive(emissive) {}

    // FOR MSGPACK ONLY!
    explicit Material() :
     textures(),
     emissive(false) {
        id = std::numeric_limits<uint64_t>::max();
        shader = std::numeric_limits<uint64_t>::max();
    }

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

inline std::string ToString(const Material& mat, const std::string& indent = "") {
    std::stringstream stream;
    stream << "Material {" << std::endl <<
     indent << "| id = " << mat.id << std::endl <<
     indent << "| shader = " << mat.shader << std::endl <<
     indent << "| textures = {" << std::endl;
    for (const auto& binding : mat.textures) {
        stream << indent << "| | " << binding.first << " -> " << binding.second << std::endl;
    }
    stream << indent << "| }" << std::endl <<
     indent << "| emissive = " << (mat.emissive ? "true" : "false") << std::endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
