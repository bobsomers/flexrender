#pragma once

#include <cstdint>
#include <vector>

#include "glm/glm.hpp"
#include "msgpack.hpp"

#include "types/triangle.hpp"
#include "types/transform.hpp"
#include "utils/tostring.hpp"

namespace fr {

struct Mesh {
    explicit Mesh(uint64_t id);

    explicit Mesh(uint64_t id, uint64_t material);

    // FOR MSGPACK ONLY!
    explicit Mesh();

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

std::string ToString(const Mesh& mesh, const std::string& indent = "");

} // namespace fr
