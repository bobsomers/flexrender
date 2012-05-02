#include "types/local_geometry.hpp"

#include <limits>
#include <iostream>
#include <sstream>

#include "utils/printers.hpp"

using std::numeric_limits;
using std::string;
using std::stringstream;
using std::endl;

namespace fr {

LocalGeometry::LocalGeometry(glm::vec3 n) :
 n(n) {
    t.x = numeric_limits<float>::quiet_NaN();
    t.y = numeric_limits<float>::quiet_NaN();
}

LocalGeometry::LocalGeometry(glm::vec3 n, glm::vec2 t) :
 n(n),
 t(t) {}

LocalGeometry::LocalGeometry() {
    n.x = numeric_limits<float>::quiet_NaN();
    n.y = numeric_limits<float>::quiet_NaN();
    n.z = numeric_limits<float>::quiet_NaN();

    t.x = numeric_limits<float>::quiet_NaN();
    t.y = numeric_limits<float>::quiet_NaN();
}

string ToString(const LocalGeometry& geom, const string& indent) {
    stringstream stream;
    stream << "LocalGeometry {" << endl <<
     indent << "| n = " << ToString(geom.n) << endl <<
     indent << "| t = " << ToString(geom.t) << endl <<
     indent << "}" << endl;
    return stream.str();
}

} // namespace fr
