#include "types/vertex.hpp"

#include <limits>
#include <iostream>
#include <sstream>

#include "utils/printers.hpp"

using std::numeric_limits;
using std::string;
using std::stringstream;
using std::endl;
using glm::vec2;
using glm::vec3;

namespace fr {

Vertex::Vertex(vec3 v, vec3 n, vec2 t) :
 v(v),
 n(n),
 t(t) {}

Vertex::Vertex(vec3 v, vec3 n) :
 v(v),
 n(n) {
    t.x = numeric_limits<float>::quiet_NaN();
    t.y = numeric_limits<float>::quiet_NaN();
}

Vertex::Vertex(vec3 v) :
 v(v) {
    n.x = numeric_limits<float>::quiet_NaN();
    n.y = numeric_limits<float>::quiet_NaN();
    n.z = numeric_limits<float>::quiet_NaN();

    t.x = numeric_limits<float>::quiet_NaN();
    t.y = numeric_limits<float>::quiet_NaN();
}

Vertex::Vertex(float x, float y, float z) :
 v(vec3(x, y, z)) {
    n.x = numeric_limits<float>::quiet_NaN();
    n.y = numeric_limits<float>::quiet_NaN();
    n.z = numeric_limits<float>::quiet_NaN();

    t.x = numeric_limits<float>::quiet_NaN();
    t.y = numeric_limits<float>::quiet_NaN();
}

Vertex::Vertex() {
    v.x = numeric_limits<float>::quiet_NaN();
    v.y = numeric_limits<float>::quiet_NaN();
    v.z = numeric_limits<float>::quiet_NaN();

    n.x = numeric_limits<float>::quiet_NaN();
    n.y = numeric_limits<float>::quiet_NaN();
    n.z = numeric_limits<float>::quiet_NaN();

    t.x = numeric_limits<float>::quiet_NaN();
    t.y = numeric_limits<float>::quiet_NaN();
}

string ToString(const Vertex& vert, const string& indent) {
    stringstream stream;
    stream << "Vertex {" << endl <<
     indent << "| v = " << ToString(vert.v) << endl <<
     indent << "| n = " << ToString(vert.n) << endl <<
     indent << "| t = " << ToString(vert.t) << endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
