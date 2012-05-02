#pragma once

#include <string>

#include "glm/glm.hpp"

namespace fr {

std::string ToString(glm::vec2 vec, const std::string& indent = "");

std::string ToString(glm::vec3 vec, const std::string& indent = "");

std::string ToString(glm::vec4 vec, const std::string& indent = "");

} // namespace fr
