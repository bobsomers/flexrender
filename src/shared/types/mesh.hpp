#pragma once

#include <cstdint>
#include <vector>
#include <limits>

#include "glm/glm.hpp"
#include "msgpack.hpp"

#include "utils/tostring.hpp"
#include "utils/printers.hpp"
#include "types/triangle.hpp"
#include "types/transform.hpp"

namespace fr {

struct Mesh {
    explicit Mesh(uint64_t id, uint64_t material) :
     id(id),
     material(material),
     tris(),
     xforms() {
        centroid.x = std::numeric_limits<float>::quiet_NaN();
        centroid.y = std::numeric_limits<float>::quiet_NaN();
        centroid.z = std::numeric_limits<float>::quiet_NaN();
    }

    // FOR MSGPACK ONLY!
    explicit Mesh() :
     tris(),
     xforms() {
        id = std::numeric_limits<uint64_t>::max();
        material = std::numeric_limits<uint64_t>::max();

        centroid.x = std::numeric_limits<float>::quiet_NaN();
        centroid.y = std::numeric_limits<float>::quiet_NaN();
        centroid.z = std::numeric_limits<float>::quiet_NaN();
    }

    /// Resource ID of the mesh.
    uint64_t id;

    /// Resource ID of the material to use for rendering.
    uint64_t material;

    /// Centroid of the mesh.
    glm::vec3 centroid;

    /// The raw triangle data.
    std::vector<Triangle> tris;

    /// List of transforms (in the order they should be applied).
    std::vector<Transform> xforms;

    MSGPACK_DEFINE(id, material, centroid, tris, xforms);

    TOSTRINGABLE(Mesh);
};

inline std::string ToString(const Mesh& mesh, const std::string& indent = "") {
    std::stringstream stream;
    std::string pad = indent + "| | ";
    stream << "Mesh {" << std::endl <<
     indent << "| id = " << mesh.id << std::endl <<
     indent << "| material = " << mesh.material << std::endl <<
     indent << "| centroid = " << ToString(mesh.centroid) << std::endl <<
     indent << "| tris = {" << std::endl;
    for (const auto& tri : mesh.tris) {
        stream << pad << ToString(tri, pad) << std::endl;
    }
    stream << indent << "| }" << std::endl <<
     indent << "| xforms = {" << std::endl;
    for (const auto& xform : mesh.xforms) {
        stream << pad << ToString(xform, pad) << std::endl;
    }
    stream << indent << "| }" << std::endl <<
     indent << "}" << std::endl;
    return stream.str();
}

} // namespace fr
