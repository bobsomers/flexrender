#pragma once

#include <cstdint>

#include "msgpack.hpp"

#include "utils/tostring.hpp"

namespace fr {

struct Shader {
    explicit Shader(uint64_t id);

    explicit Shader(uint64_t id, const std::string& code);

    // FOR MSGPACK ONLY!
    explicit Shader();

    /// Resource ID of the shader.
    uint64_t id;

    /// The code we run for the shader.
    std::string code;

    MSGPACK_DEFINE(id, code);

    TOSTRINGABLE(Shader);
};

std::string ToString(const Shader& shader, const std::string& indent = "");

} // namespace fr
