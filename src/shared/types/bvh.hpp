#pragma once

#include <cstdint>
#include <vector>

#include "glm/glm.hpp"
#include "msgpack.hpp"

#include "types/linear_node.hpp"

namespace fr {

struct Mesh;
struct SlimRay;
struct LinkedNode;
struct PrimitiveInfo;
struct HitRecord;

/**
 * The construction implementation is based on the one presented in Physically
 * Based Rendering, Section 4.4, pages 208-227, with some modifications to
 * support stackless traversal. The stackless traversal algorithm is as
 * described in Hapala et al [2011].
 */

class BVH {
public:
    typedef bool (*PrimitiveIntersect)(uint32_t index, HitRecord* info);

    /// Constructs a BVH for traversing the given mesh.
    explicit BVH(const Mesh* mesh);

    /// MSGPACK ONLY!
    explicit BVH();

    /**
     * Traverses the BVH by testing the given SlimRay against the bounding
     * volumes. If a leaf node is hit, the passed primitive intersector
     * function will be called.
     */
    void Traverse(const SlimRay& ray, PrimitiveIntersect intersector);

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

    /**
     * Flattens the linked tree structure into a linear structure with offsets
     * for faster/portable traversal at runtime.
     */
    size_t FlattenTree(LinkedNode* current, size_t parent, size_t* offset);

    /**
     * Recursively deletes a LinkedNode tree structure rooted at node.
     */
    void DeleteLinked(LinkedNode* node);

    /// Returns the index of the sibling of the current node.
    inline size_t Sibling(size_t current) {
        size_t parent = _nodes[current].parent;
        size_t right = _nodes[parent].right;
        return (right == current) ? parent + 1 : right;
    }

    /// The near child is defined to be the left-hand child.
    inline size_t NearChild(size_t current) {
        return current + 1;
    }

    /// The far child is defined to be the right-hand child.
    inline size_t FarChild(size_t current) {
        return _nodes[current].right;
    }

    /// Performs a quick bounding box check against the given bounds and ray.
    inline bool BoundingHit(const BoundingBox& bounds, const SlimRay& ray,
     glm::vec3 inv_dir, float max) {
        float t = -1.0f;
        return bounds.Intersect(ray, inv_dir, &t) && t < max;
    }
};

} // namespace fr
