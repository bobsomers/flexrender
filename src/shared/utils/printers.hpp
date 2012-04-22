#pragma once

#include "glm/glm.hpp"

#include "utils/tostring.hpp"

namespace fr {

inline std::string ToString(glm::vec2 vec, const std::string& indent = "") {
    std::stringstream stream;
    stream << indent << "<" << vec.x << ", " << vec.y << ">";
    return stream.str();
}

inline std::string ToString(glm::vec3 vec, const std::string& indent = "") {
    std::stringstream stream;
    stream << indent << "<" << vec.x << ", " << vec.y << ", " << vec.z << ">";
    return stream.str();
}

inline std::string ToString(glm::vec4 vec, const std::string& indent = "") {
    std::stringstream stream;
    stream << indent <<
     "<" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ">";
    return stream.str();
}

} // namespace fr
