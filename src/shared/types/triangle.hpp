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

    /// Generates a sample point on the triangle in object space. If normal or
    /// texcoord is non-null, they will be filled in with the interpolated
    /// normal and texture coordinates, respectively.
    glm::vec3 Sample(glm::vec3* normal = nullptr, glm::vec2* texcoord = nullptr) const;

    MSGPACK_DEFINE(verts[0], verts[1], verts[2]);

    TOSTRINGABLE(Triangle);
};

std::string ToString(const Triangle& tri, const std::string& indent = "");

} // namespace fr
