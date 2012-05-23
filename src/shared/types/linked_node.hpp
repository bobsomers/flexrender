#pragma once

#include <cstring>

#include "types/bounding_box.hpp"
#include "utils/tostring.hpp"

namespace fr {

struct LinkedNode {
    /// Constructor for leaf nodes.
    explicit LinkedNode(size_t index, const BoundingBox& bounds);

    /// Constructor for interior nodes.
    explicit LinkedNode(LinkedNode* left, LinkedNode* right,
     BoundingBox::Axis split);

    /// The bounding box of this node in world space.
    BoundingBox bounds;

    /// Links to this node's children, if it's an interior node.
    LinkedNode* children[2];

    /// Index of the primitive, if it's a leaf node.
    size_t index;

    /// The axis the node was split on.
    BoundingBox::Axis split;
};

std::string ToString(const LinkedNode& node, const std::string& indent = "");

} // namespace fr
