#pragma once

#include "msgpack.hpp"

#include "types/vertex.hpp"
#include "utils/tostring.hpp"

namespace fr {

struct Triangle {
    explicit Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
        verts[0] = v1;
        verts[1] = v2;
        verts[2] = v3;
    }

    // FOR MSGPACK ONLY!
    explicit Triangle() {
        verts[0] = Vertex();
        verts[1] = Vertex();
        verts[2] = Vertex();
    }

    // Vertices of the triangle.
    Vertex verts[3];

    MSGPACK_DEFINE(verts[0], verts[1], verts[2]);

    TOSTRINGABLE(Triangle);
};

inline std::string ToString(const Triangle& tri, const std::string& indent = "") {
    std::stringstream stream;
    std::string pad = indent + "| ";
    stream << "Triangle {" << std::endl <<
     indent << "| v1 = " << ToString(tri.verts[0], pad) << std::endl << 
     indent << "| v2 = " << ToString(tri.verts[1], pad) << std::endl <<
     indent << "| v3 = " << ToString(tri.verts[2], pad) << std::endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
