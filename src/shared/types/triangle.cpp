#include "types/triangle.hpp"

#include <iostream>
#include <sstream>

using std::string;
using std::stringstream;
using std::endl;
using glm::vec2;
using glm::vec3;

namespace fr {

Triangle::Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
    verts[0] = v1;
    verts[1] = v2;
    verts[2] = v3;
}

Triangle::Triangle() {
    verts[0] = Vertex();
    verts[1] = Vertex();
    verts[2] = Vertex();
}

vec3 Triangle::Sample(vec3* normal, vec2* texcoord) const {
    // TODO: implement actual sampling

    if (normal != nullptr) {
        *normal = verts[0].n;
    }

    if (texcoord != nullptr) {
        *texcoord = verts[0].t;
    }

    return verts[0].v;
}

string ToString(const Triangle& tri, const string& indent) {
    stringstream stream;
    string pad = indent + "| ";
    stream << "Triangle {" << endl <<
     indent << "| v1 = " << ToString(tri.verts[0], pad) << endl << 
     indent << "| v2 = " << ToString(tri.verts[1], pad) << endl <<
     indent << "| v3 = " << ToString(tri.verts[2], pad) << endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
