#pragma once

#include <cstdint>
#include <vector>

#include "types/linear_node.hpp"

namespace fr {

struct Mesh;
struct FatRay;
struct LinkedNode;
struct PrimitiveInfo;

/**
 * This implementation is heavily based on the one presented in Physically
 * Based Rendering, Section 4.4, pages 208-227, with some modifications.
 */

class BVH {
public:
    /// Constructs a BVH for traversing the given mesh.
    explicit BVH(const Mesh* mesh);

    /// Intersects the given ray with the this BVH.
    void Intersect(FatRay* ray, uint64_t me);

private:
    std::vector<LinearNode> _nodes;

    /**
     * Recursively partitions and builds the BVH for the given build data
     * between the start and end indexes. Returns the total nodes created
     * through the passed pointer.
     */
    LinkedNode* RecursiveBuild(std::vector<PrimitiveInfo>& build_data,
     size_t start, size_t end, size_t* total_nodes);
};

} // namespace fr
