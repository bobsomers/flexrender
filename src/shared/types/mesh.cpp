#include "types/mesh.hpp"

#include <limits>
#include <iostream>
#include <sstream>

#include "types/bvh.hpp"
#include "utils/printers.hpp"

using std::numeric_limits;
using std::string;
using std::stringstream;
using std::endl;
using glm::vec4;
using glm::mat4;
using glm::inverse;
using glm::transpose;

namespace fr {

Mesh::Mesh(uint32_t id) :
 id(id),
 tris(),
 bvh(nullptr) {
    material = numeric_limits<uint32_t>::max();

    centroid.x = numeric_limits<float>::quiet_NaN();
    centroid.y = numeric_limits<float>::quiet_NaN();
    centroid.z = numeric_limits<float>::quiet_NaN();

    xform_cols[0] = vec4(1.0f, 0.0f, 0.0f, 0.0f);
    xform_cols[1] = vec4(0.0f, 1.0f, 0.0f, 0.0f);
    xform_cols[2] = vec4(0.0f, 0.0f, 1.0f, 0.0f);
    xform_cols[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    xform = mat4(xform_cols[0], xform_cols[1], xform_cols[2], xform_cols[3]);
    xform_inv = mat4(xform_cols[0], xform_cols[1], xform_cols[2], xform_cols[3]);
    xform_inv_tr = mat4(xform_cols[0], xform_cols[1], xform_cols[2], xform_cols[3]);
}

Mesh::Mesh(uint32_t id, uint32_t material) :
 id(id),
 material(material),
 tris(),
 bvh(nullptr) {
    centroid.x = numeric_limits<float>::quiet_NaN();
    centroid.y = numeric_limits<float>::quiet_NaN();
    centroid.z = numeric_limits<float>::quiet_NaN();

    xform_cols[0] = vec4(1.0f, 0.0f, 0.0f, 0.0f);
    xform_cols[1] = vec4(0.0f, 1.0f, 0.0f, 0.0f);
    xform_cols[2] = vec4(0.0f, 0.0f, 1.0f, 0.0f);
    xform_cols[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    xform = mat4(xform_cols[0], xform_cols[1], xform_cols[2], xform_cols[3]);
    xform_inv = mat4(xform_cols[0], xform_cols[1], xform_cols[2], xform_cols[3]);
    xform_inv_tr = mat4(xform_cols[0], xform_cols[1], xform_cols[2], xform_cols[3]);
}

Mesh::Mesh() :
 tris(),
 bvh(nullptr) {
    id = numeric_limits<uint32_t>::max();
    material = numeric_limits<uint32_t>::max();

    centroid.x = numeric_limits<float>::quiet_NaN();
    centroid.y = numeric_limits<float>::quiet_NaN();
    centroid.z = numeric_limits<float>::quiet_NaN();

    xform_cols[0] = vec4(1.0f, 0.0f, 0.0f, 0.0f);
    xform_cols[1] = vec4(0.0f, 1.0f, 0.0f, 0.0f);
    xform_cols[2] = vec4(0.0f, 0.0f, 1.0f, 0.0f);
    xform_cols[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    xform = mat4(xform_cols[0], xform_cols[1], xform_cols[2], xform_cols[3]);
    xform_inv = mat4(xform_cols[0], xform_cols[1], xform_cols[2], xform_cols[3]);
    xform_inv_tr = mat4(xform_cols[0], xform_cols[1], xform_cols[2], xform_cols[3]);
}

void Mesh::ComputeMatrices() {
    xform = mat4(xform_cols[0], xform_cols[1], xform_cols[2], xform_cols[3]);
    xform_inv = inverse(xform);
    xform_inv_tr = transpose(xform_inv);
}

string ToString(const Mesh& mesh, const string& indent) {
    stringstream stream;
    string pad = indent + "| ";
    string pad2 = indent + "| | ";
    stream << "Mesh {" << endl <<
     indent << "| id = " << mesh.id << endl <<
     indent << "| material = " << mesh.material << endl <<
     indent << "| xform_cols[0] = " << ToString(mesh.xform_cols[0]) << endl <<
     indent << "| xform_cols[1] = " << ToString(mesh.xform_cols[1]) << endl <<
     indent << "| xform_cols[2] = " << ToString(mesh.xform_cols[2]) << endl <<
     indent << "| xform_cols[3] = " << ToString(mesh.xform_cols[3]) << endl <<
     indent << "| tris = {" << endl;
    for (const auto& tri : mesh.tris) {
        stream << pad << ToString(tri, pad2) << endl;
    }
    stream << indent << "| }" << endl <<
     indent << "| centroid = " << ToString(mesh.centroid) << endl <<
     indent << "| xform = " << ToString(mesh.xform, pad) << endl <<
     indent << "| xform_inv = " << ToString(mesh.xform_inv, pad) << endl <<
     indent << "| xform_inv_tr = " << ToString(mesh.xform_inv_tr, pad) << endl <<
     indent << "}" << endl;
    return stream.str();
}

} // namespace fr
