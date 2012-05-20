#pragma once

#include <cstdint>
#include <vector>

#include "types/linear_node.hpp"

namespace fr {

struct Mesh;
struct FatRay;

class BVH {
public:
    /// Constructs a BVH for traversing the given mesh.
    explicit BVH(const Mesh* mesh);

    /// Intersects the given ray with the this BVH.
    void Intersect(FatRay* ray, uint64_t me);

private:
    std::vector<LinearNode> _nodes;
};

} // namespace fr
