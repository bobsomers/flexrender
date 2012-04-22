#pragma once

#include <limits>

#include "glm/glm.hpp"
#include "msgpack.hpp"

namespace fr {

struct Vertex {
    explicit Vertex() {
        v.x = std::numeric_limits<float>::quiet_NaN();
        v.y = std::numeric_limits<float>::quiet_NaN();
        v.z = std::numeric_limits<float>::quiet_NaN();

        n.x = std::numeric_limits<float>::quiet_NaN();
        n.y = std::numeric_limits<float>::quiet_NaN();
        n.z = std::numeric_limits<float>::quiet_NaN();

        t.x = std::numeric_limits<float>::quiet_NaN();
        t.y = std::numeric_limits<float>::quiet_NaN();
    }

    /// Position of the vertex.
    glm::vec3 v;

    /// Normal of the vertex. Unit length is not enforced.
    glm::vec3 n;

    /// Texture coordinate of the vertex. Range is not enforced.
    glm::vec2 t;

    MSGPACK_DEFINE(v, n, t);
};

} // namespace fr
