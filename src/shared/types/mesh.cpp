#include "types/mesh.hpp"

#include <limits>
#include <iostream>
#include <sstream>

#include "utils/printers.hpp"

using std::numeric_limits;
using std::string;
using std::stringstream;
using std::endl;

namespace fr {

Mesh::Mesh(uint64_t id) :
 id(id),
 tris(),
 xforms() {
    material = numeric_limits<uint64_t>::max();

    centroid.x = numeric_limits<float>::quiet_NaN();
    centroid.y = numeric_limits<float>::quiet_NaN();
    centroid.z = numeric_limits<float>::quiet_NaN();
}

Mesh::Mesh(uint64_t id, uint64_t material) :
 id(id),
 material(material),
 tris(),
 xforms() {
    centroid.x = numeric_limits<float>::quiet_NaN();
    centroid.y = numeric_limits<float>::quiet_NaN();
    centroid.z = numeric_limits<float>::quiet_NaN();
}

Mesh::Mesh() :
 tris(),
 xforms() {
    id = numeric_limits<uint64_t>::max();
    material = numeric_limits<uint64_t>::max();

    centroid.x = numeric_limits<float>::quiet_NaN();
    centroid.y = numeric_limits<float>::quiet_NaN();
    centroid.z = numeric_limits<float>::quiet_NaN();
}

string ToString(const Mesh& mesh, const string& indent) {
    stringstream stream;
    string pad = indent + "| | ";
    stream << "Mesh {" << endl <<
     indent << "| id = " << mesh.id << endl <<
     indent << "| material = " << mesh.material << endl <<
     indent << "| centroid = " << ToString(mesh.centroid) << endl <<
     indent << "| tris = {" << endl;
    for (const auto& tri : mesh.tris) {
        stream << pad << ToString(tri, pad) << endl;
    }
    stream << indent << "| }" << endl <<
     indent << "| xforms = {" << endl;
    for (const auto& xform : mesh.xforms) {
        stream << pad << ToString(xform, pad) << endl;
    }
    stream << indent << "| }" << endl <<
     indent << "}" << endl;
    return stream.str();
}

} // namespace fr
