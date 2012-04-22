#pragma once

#include "types/vertex.hpp"

namespace fr {

struct Triangle {
    explicit Triangle() {
        vertices[0] = Vertex();
        vertices[1] = Vertex();
        vertices[2] = Vertex();
    }

    // Vertices of the triangle.
    Vertex vertices[3];

    MSGPACK_DEFINE(vertices[0], vertices[1], vertices[2]);
};

} // namespace fr
