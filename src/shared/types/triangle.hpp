#pragma once

#include "glm/glm.hpp"
#include "msgpack.hpp"

#include "types/vertex.hpp"
#include "utils/tostring.hpp"

namespace fr {

struct Triangle {
    explicit Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3);

    // FOR MSGPACK ONLY!
    explicit Triangle();

    /// Vertices of the triangle.
    Vertex verts[3];

    /// Generates a sample point on the triangle in object space. If texcoords
    /// is non-null, it will be filled in with the calculated interpoated
    /// texture coordinates.
    glm::vec3 Sample(glm::vec2* texcoords = nullptr) const;

    MSGPACK_DEFINE(verts[0], verts[1], verts[2]);

    TOSTRINGABLE(Triangle);
};

std::string ToString(const Triangle& tri, const std::string& indent = "");

} // namespace fr
