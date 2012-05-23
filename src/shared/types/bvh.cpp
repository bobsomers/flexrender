#include "types/bvh.hpp"

#include <cstring>
#include <algorithm>

#include "types.hpp"
#include "utils.hpp"

using std::vector;
using std::nth_element;
using std::partition;

namespace fr {

const uint32_t BVH::NUM_BUCKETS = 12;

BVH::BVH(const Mesh* mesh) :
 _nodes() {
    // Initialize build data from mesh triangles.
    vector<PrimitiveInfo> build_data;
    build_data.reserve(mesh->tris.size());
    for (size_t i = 0; i < mesh->tris.size(); i++) {
        BoundingBox bounds = mesh->tris[i].WorldBounds(mesh->xform);
        build_data.emplace_back(i, bounds);
    }

    // Actually build the tree.
    Build(build_data);
}

void BVH::Intersect(FatRay* ray, uint64_t me) {
    // TODO
}

void BVH::Build(vector<PrimitiveInfo>& build_data) {
    // Recursively build the BVH tree.
    size_t total_nodes = 0;
    LinkedNode* root = RecursiveBuild(build_data, 0, build_data.size(), &total_nodes);

    TOUTLN(ToString(root)); // TODO: remove

    // Flatten the tree into a linear representation.
    _nodes.reserve(total_nodes);
    for (size_t i = 0; i < total_nodes; i++) {
        _nodes.emplace_back();
    }
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
        node = new LinkedNode(build_data[start].index, bounds);
    } else {
        // Find the bounds of the centroids.
        BoundingBox centroid_bounds;
        for (size_t i = start; i < end; i++) {
            centroid_bounds.Absorb(build_data[i].centroid);
        }

        // Split along the longest axis.
        BoundingBox::Axis split_axis = centroid_bounds.LongestAxis();
        float split_min = AxisComponent(centroid_bounds.min, split_axis);
        float split_max = AxisComponent(centroid_bounds.max, split_axis);

        size_t mid = (start + end) / 2;
        if (num_primitives <= 4 || split_min == split_max) {
            // Partition using equal size subsets, since SAH has diminishing
            // returns at this point. Also handles a degenerate case where we
            // have multiple primitives stacked on top  of each other with the
            // same centroid.
            nth_element(&build_data[start], &build_data[mid], &build_data[end - 1] + 1,
             [split_axis](const PrimitiveInfo& a, const PrimitiveInfo& b) {
                 return AxisComponent(a.centroid, split_axis) < AxisComponent(b.centroid, split_axis);
             });
        } else {
            // Partition using the surface area heuristic (SAH).
            uint32_t min_cost_split = ComputeSAH(build_data, start, end,
             split_min, split_max, bounds.SurfaceArea(), split_axis);

            PrimitiveInfo* pmid = partition(&build_data[start],
             &build_data[end - 1] + 1, [min_cost_split, split_min, split_max, split_axis](const PrimitiveInfo& p) {
                 uint32_t bucket = NUM_BUCKETS *
                  ((AxisComponent(p.centroid, split_axis) - split_min) /
                  (split_max - split_min));

                 if (bucket == NUM_BUCKETS) bucket = NUM_BUCKETS - 1;
                 assert(bucket >= 0 && bucket < NUM_BUCKETS);

                 return bucket <= min_cost_split;
             });

            mid = pmid - &build_data[0];

        }

        // Recursively build the subtrees for both partitions.
        node = new LinkedNode(RecursiveBuild(build_data, start, mid, total_nodes),
                              RecursiveBuild(build_data, mid, end, total_nodes),
                              split_axis);
    }

    return node;
}

uint32_t BVH::ComputeSAH(vector<PrimitiveInfo>& build_data, size_t start,
 size_t end, float min, float max, float surface_area, BoundingBox::Axis axis) {
    // Initialize each bucket for potential split candidates.
    BucketInfo buckets[NUM_BUCKETS];
    for (size_t i = start; i < end; i++) {
        uint32_t bucket = NUM_BUCKETS *
         ((AxisComponent(build_data[i].centroid, axis) - min) / (max - min));

        if (bucket == NUM_BUCKETS) bucket = NUM_BUCKETS - 1;
        assert(bucket >= 0 && bucket < NUM_BUCKETS);

        buckets[bucket].count++;
        buckets[bucket].bounds.Union(build_data[i].bounds);
    }

    // Compute the cost for splitting after each bucket.
    float cost[NUM_BUCKETS - 1];
    for (uint32_t i = 0; i < NUM_BUCKETS - 1; i++) {
        BoundingBox left_bounds, right_bounds;
        size_t left_count = 0;
        size_t right_count = 0;

        for (uint32_t j = 0; j <= i; j++) {
            left_bounds.Union(buckets[j].bounds);
            left_count += buckets[j].count;
        }

        for (uint32_t j = i + i; j < NUM_BUCKETS; j++) {
            right_bounds.Union(buckets[j].bounds);
            right_count += buckets[j].count;
        }

        cost[i] = 0.125f +
         (left_count * left_bounds.SurfaceArea() + right_count * right_bounds.SurfaceArea()) /
         surface_area;
    }
    
    // Find which bucket minimizes the cost.
    float min_cost = cost[0];
    uint32_t min_cost_split = 0;
    for (uint32_t i = 1; i < NUM_BUCKETS - 1; i++) {
        if (cost[i] < min_cost) {
            min_cost = cost[i];
            min_cost_split = i;
        }
    }

    return min_cost_split;
}

} // namespace fr
