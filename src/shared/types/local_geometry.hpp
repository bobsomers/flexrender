#pragma once

#include "glm/glm.hpp"

#include "utils/tostring.hpp"

namespace fr {

struct LocalGeometry {
    explicit LocalGeometry(glm::vec3 n);

    explicit LocalGeometry(glm::vec3 n, glm::vec2 t);

    explicit LocalGeometry();

    /// The interpolated surface normal.
    glm::vec3 n;

    /// The interpolated texture coordinates.
    glm::vec2 t;

    TOSTRINGABLE(LocalGeometry);
};

std::string ToString(const LocalGeometry& geom, const std::string& indent = "");

} // namespace fr
