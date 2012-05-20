#pragma once

#include <cstring>

#include "types/bounding_box.hpp"

namespace fr {

struct LinearNode {
    explicit LinearNode(const BoundingBox& bounds, size_t primitive = 0);

    /// The bounding box of this node in world space.
    BoundingBox bounds;

    /// Left-hand primitive index if this is a leaf node, 0 if this is an
    /// interior node.
    size_t primitive;

    /// Right-hand primitive index if this is a leaf node, right-hand subtree
    /// index if this is an interior node.
    size_t offset;
};

} // namespace fr
