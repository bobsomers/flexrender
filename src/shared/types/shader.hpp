#pragma once

#include <cstdint>
#include <string>
#include <limits>

#include "msgpack.hpp"

#include "utils/tostring.hpp"

namespace fr {

struct Shader {
    explicit Shader(uint64_t id) :
     id(id),
     code("") {}

    explicit Shader(uint64_t id, const std::string& code) :
     id(id),
     code(code) {}

    // FOR MSGPACK ONLY!
    explicit Shader() :
     code("") {
        id = std::numeric_limits<uint64_t>::max(); 
    }

    /// Resource ID of the shader.
    uint64_t id;

    /// The code we run for the shader.
    std::string code;

    MSGPACK_DEFINE(id, code);

    TOSTRINGABLE(Shader);
};

inline std::string ToString(const Shader& shader, const std::string& indent = "") {
    std::stringstream stream;
    stream << "Shader {" << std::endl <<
     indent << "  id = " << shader.id << std::endl <<
     indent << "  code = ..." << std::endl <<
"======================================================================" << std::endl <<
shader.code << std::endl <<
"======================================================================" << std::endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
