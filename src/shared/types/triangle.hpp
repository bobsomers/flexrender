#pragma once

#include "msgpack.hpp"

#include "types/vertex.hpp"
#include "utils/tostring.hpp"

namespace fr {

struct Triangle {
    explicit Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3);

    // FOR MSGPACK ONLY!
    explicit Triangle();

    // Vertices of the triangle.
    Vertex verts[3];

    MSGPACK_DEFINE(verts[0], verts[1], verts[2]);

    TOSTRINGABLE(Triangle);
};

std::string ToString(const Triangle& tri, const std::string& indent = "");

} // namespace fr
