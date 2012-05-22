#include "types/bvh.hpp"

#include <cstring>

#include "types.hpp"

using std::vector;

namespace fr {

BVH::BVH(const Mesh* mesh) :
 _nodes() {
    // Initialize construction data from mesh triangles.
    vector<PrimitiveInfo> build_data;
    build_data.reserve(mesh->tris.size());
    for (size_t i = 0; i < mesh->tris.size(); i++) {
        BoundingBox bounds = mesh->tris[i].WorldBounds(mesh->xform);
        build_data.emplace_back(i, bounds);
    }
}

void BVH::Intersect(FatRay* ray, uint64_t me) {
    // TODO
}

} // namespace fr
