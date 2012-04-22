#pragma once

#include <limits>

#include "glm/glm.hpp"

#include "utils/tostring.hpp"
#include "utils/printers.hpp"

namespace fr {

struct LocalGeometry {
    explicit LocalGeometry(glm::vec3 n) :
     n(n) {
        t.x = std::numeric_limits<float>::quiet_NaN();
        t.y = std::numeric_limits<float>::quiet_NaN();
    }

    explicit LocalGeometry(glm::vec3 n, glm::vec2 t) :
     n(n),
     t(t) {}

    explicit LocalGeometry() {
        n.x = std::numeric_limits<float>::quiet_NaN();
        n.y = std::numeric_limits<float>::quiet_NaN();
        n.z = std::numeric_limits<float>::quiet_NaN();

        t.x = std::numeric_limits<float>::quiet_NaN();
        t.y = std::numeric_limits<float>::quiet_NaN();
    }

    /// The interpolated surface normal.
    glm::vec3 n;

    /// The interpolated texture coordinates.
    glm::vec2 t;

    TOSTRINGABLE(LocalGeometry);
};

inline std::string ToString(const LocalGeometry& geom, const std::string& indent = "") {
    std::stringstream stream;
    stream << "LocalGeometry {" << std::endl <<
     indent << "| n = " << ToString(geom.n) << std::endl <<
     indent << "| t = " << ToString(geom.t) << std::endl <<
     indent << "}" << std::endl;
    return stream.str();
}

} // namespace fr
