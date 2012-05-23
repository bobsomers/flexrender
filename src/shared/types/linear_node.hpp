#pragma once

#include <cstdint>
#include <cstring>

#include "msgpack.hpp"

#include "types/bounding_box.hpp"
#include "utils/tostring.hpp"

namespace fr {

struct LinearNode {
    explicit LinearNode();

    /// The bounding box of this node in world space.
    BoundingBox bounds;

    /// Boolean flag that indicates if this is a leaf node or interior node.
    /// Yes, it is intentionally this big to pad the entire structure to fit
    /// in a cache line (64 bytes).
    uint64_t leaf;

    /// Index of the primitive, if this is a leaf node.
    size_t index;

    /// Offset of the sibling of this node if this is an interior node.
    ssize_t sibling;

    /// Offset of the parent of this node if this is an interior node.
    ssize_t parent;

    /// Axis that we split on. Yes, it is intentionally this big to pad the
    /// entire structure to fit in a cache line (64 bytes).
    uint64_t axis;

    MSGPACK_DEFINE(bounds, leaf, index, sibling, parent, axis);
};

std::string ToString(const LinearNode& node, const std::string& indent = "");

} // namespace fr
