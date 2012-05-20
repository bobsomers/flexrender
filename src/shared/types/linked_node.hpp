#pragma once

#include <cstring>

#include "types/bounding_box.hpp"

namespace fr {

struct LinkedNode {
    /// Constructor for leaf nodes.
    explicit LinkedNode(size_t first, size_t second, const BoundingBox& bounds);

    /// Constructor for interior nodes.
    explicit LinkedNode(LinkedNode* left, LinkedNode* right);

    /// The bounding box of this node in world space.
    BoundingBox bounds;

    /// Links to this node's children, if it's an interior node.
    LinkedNode* children[2];

    /// Indexes of the primitives, if it's a leaf node.
    size_t primitives[2];
};

} // namespace fr
