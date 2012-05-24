#pragma once

#include <cstdint>
#include <vector>

#include "glm/glm.hpp"
#include "msgpack.hpp"

#include "types/triangle.hpp"
#include "utils/tostring.hpp"

namespace fr {

class BVH;

struct Mesh {
    explicit Mesh(uint32_t id);

    explicit Mesh(uint32_t id, uint32_t material);

    // FOR MSGPACK ONLY!
    explicit Mesh();

    /// Resource ID of the mesh.
    uint32_t id;

    /// Resource ID of the material to use for rendering.
    uint32_t material;

    /// Columns of the 4x4 transform matrix. Only used for syncing.
    glm::vec4 xform_cols[4];

    /// The raw triangle data.
    std::vector<Triangle> tris;

    /// Centroid of the mesh in WORLD space. Only used for scene distribution.
    /// Not synced.
    glm::vec3 centroid;

    /// The world space transformation matrix. Not synced, but recomputed on
    /// worker.
    glm::mat4 xform;

    /// The inverse of the transformation matrix. Not synced, but recomputed on
    /// worker.
    glm::mat4 xform_inv;

    /// The inverse transpose of the transformation matrix. Not synced, but
    /// recomputed on worker.
    glm::mat4 xform_inv_tr;

    /// The BVH for traversing this mesh efficiently.
    BVH* bvh;

    /// Uses the data in xform_cols to build the transformation matrix and
    /// compute the inverse and inverse transpose.
    void ComputeMatrices();

    MSGPACK_DEFINE(id, material, xform_cols[0], xform_cols[1], xform_cols[2],
     xform_cols[3], tris);

    TOSTRINGABLE(Mesh);
};

std::string ToString(const Mesh& mesh, const std::string& indent = "");

} // namespace fr
