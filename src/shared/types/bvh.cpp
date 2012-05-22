#include "types/bvh.hpp"

#include <cstring>
#include <algorithm>

#include "types.hpp"

using std::vector;
using std::nth_element;
using std::partition;

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

    // Recursively build the BVH tree.
    size_t total_nodes = 0;
    LinkedNode* root = RecursiveBuild(build_data, 0, build_data.size(),
     &total_nodes);

    // Flatten the tree into a linear representation.
    // TODO
}

void BVH::Intersect(FatRay* ray, uint64_t me) {
    // TODO
}

LinkedNode* BVH::RecursiveBuild(vector<PrimitiveInfo>& build_data, size_t start,
 size_t end, size_t* total_nodes) {
    assert(start != end);
    
    (*total_nodes)++;
    LinkedNode* node = nullptr;

    // Compute the bounds of all primitives in this BVH node.
    BoundingBox bounds;
    for (size_t i = start; i < end; i++) {
        bounds = bounds.Union(build_data[i].bounds);
    }

    // How many primitives are we partitioning?
    size_t num_primitives = end - start;
    if (num_primitives == 1) {
        // Only one primitive left, create a leaf node.
        node = new LinkedNode(build_data[start].index, 0, bounds);
    } else {
        // Find the bounds of the centroids.
        BoundingBox centroid_bounds;
        for (size_t i = start; i < end; i++) {
            centroid_bounds.Absorb(build_data[i].centroid);
        }

        // Split along the longest axis.
        BoundingBox::Axis split_axis = centroid_bounds.LongestAxis();
        float split_min = (split_axis == BoundingBox::Axis::X) ? centroid_bounds.min.x :
                          (split_axis == BoundingBox::Axis::Y) ? centroid_bounds.min.y :
                                                                 centroid_bounds.min.z;
        float split_max = (split_axis == BoundingBox::Axis::X) ? centroid_bounds.max.x :
                          (split_axis == BoundingBox::Axis::Y) ? centroid_bounds.max.y :
                                                                 centroid_bounds.max.z;

        // Degenerate case where we have multiple primitives stacked on top
        // of each other with the same centroid.
        if (split_min == split_max) {
            // TODO
        }

        size_t mid = (start + end) / 2;
        if (num_primitives <= 4) {
            // Partition using equal size subsets, since SAH has diminishing
            // returns at this point.
            nth_element(&build_data[start], &build_data[mid], &build_data[end - 1] + 1,
             [split_axis](const PrimitiveInfo& a, const PrimitiveInfo& b) {
                 float a_val = (split_axis == BoundingBox::Axis::X) ? a.centroid.x :
                               (split_axis == BoundingBox::Axis::Y) ? a.centroid.y :
                                                                      a.centroid.z;
                 float b_val = (split_axis == BoundingBox::Axis::X) ? b.centroid.x :
                               (split_axis == BoundingBox::Axis::Y) ? b.centroid.y :
                                                                      b.centroid.z;
                 return a_val < b_val;
             });
        } else {
            // Partition using the surface area heuristic (SAH).
            // TODO
        }

        // Recursively build the subtrees for both partitions.
        node = new LinkedNode(RecursiveBuild(build_data, start, mid, total_nodes),
                              RecursiveBuild(build_data, mid, end, total_nodes),
                              split_axis);
    }

    return node;
}

} // namespace fr
