#pragma once

#include <cstdint>
#include <vector>

#include "msgpack.hpp"

#include "types/linear_node.hpp"

namespace fr {

struct Mesh;
struct FatRay;
struct LinkedNode;
struct PrimitiveInfo;

/**
 * The construction implementation is based on the one presented in Physically
 * Based Rendering, Section 4.4, pages 208-227, with some modifications to
 * support stackless traversal. The stackless traversal algorithm is as
 * described in Hapala et al [2011].
 */

class BVH {
public:
    /// Constructs a BVH for traversing the given mesh.
    explicit BVH(const Mesh* mesh);

    /// MSGPACK ONLY!
    explicit BVH();

    /// Intersects the given ray with the this BVH.
    void Intersect(FatRay* ray, uint64_t me);

    MSGPACK_DEFINE(_nodes);

private:
    struct BucketInfo {
        uint32_t count;
        BoundingBox bounds; 
    };

    static const uint32_t NUM_BUCKETS;

    std::vector<LinearNode> _nodes;

    /**
     * Constructs the BVH from the given initialized build data.
     */
    void Build(std::vector<PrimitiveInfo>& build_data);

    /**
     * Recursively partitions and builds the BVH for the given build data
     * between the start and end indexes. Returns the total nodes created
     * through the passed pointer.
     */
    LinkedNode* RecursiveBuild(std::vector<PrimitiveInfo>& build_data,
     size_t start, size_t end, size_t* total_nodes);

    /**
     * Computes the minimum cost split for the build_data given num_buckets
     * possible candidate splits and the centroid bounding box.
     */
    uint32_t ComputeSAH(std::vector<PrimitiveInfo>& build_data, size_t start,
     size_t end, float min, float max, float surface_area, BoundingBox::Axis axis);
};

} // namespace fr
