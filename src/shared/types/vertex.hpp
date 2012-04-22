#pragma once

#include <limits>

#include "glm/glm.hpp"
#include "msgpack.hpp"

#include "utils/tostring.hpp"
#include "utils/printers.hpp"

namespace fr {

struct Vertex {
    explicit Vertex(glm::vec3 v, glm::vec3 n, glm::vec2 t) :
     v(v),
     n(n),
     t(t) {}

    explicit Vertex(glm::vec3 v, glm::vec3 n) :
     v(v),
     n(n) {
        t.x = std::numeric_limits<float>::quiet_NaN();
        t.y = std::numeric_limits<float>::quiet_NaN();
    }

    explicit Vertex(glm::vec3 v) :
     v(v) {
        n.x = std::numeric_limits<float>::quiet_NaN();
        n.y = std::numeric_limits<float>::quiet_NaN();
        n.z = std::numeric_limits<float>::quiet_NaN();

        t.x = std::numeric_limits<float>::quiet_NaN();
        t.y = std::numeric_limits<float>::quiet_NaN();
    }

    explicit Vertex(float x, float y, float z) :
     v(glm::vec3(x, y, z)) {
        n.x = std::numeric_limits<float>::quiet_NaN();
        n.y = std::numeric_limits<float>::quiet_NaN();
        n.z = std::numeric_limits<float>::quiet_NaN();

        t.x = std::numeric_limits<float>::quiet_NaN();
        t.y = std::numeric_limits<float>::quiet_NaN();
    }

    // FOR MSGPACK ONLY!
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

    TOSTRINGABLE(Vertex);
};

inline std::string ToString(const Vertex& vert, const std::string& indent = "") {
    std::stringstream stream;
    std::string pad = indent + "  ";
    stream << "Vertex {" << std::endl <<
     indent << "  v = " << ToString(vert.v) << std::endl <<
     indent << "  n = " << ToString(vert.n) << std::endl <<
     indent << "  t = " << ToString(vert.t) << std::endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
