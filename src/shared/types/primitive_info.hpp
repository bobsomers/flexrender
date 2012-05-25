#pragma once

#include <cstring>

#include "glm/glm.hpp"

#include "types/bounding_box.hpp"
#include "utils/tostring.hpp"

namespace fr {

struct PrimitiveInfo {
    explicit PrimitiveInfo(size_t index, const BoundingBox& bounds);

    /// Index of the primitive we're talking about in the list of primitives.
    uint32_t index;

    /// Centroid of the primitive in question.
    glm::vec3 centroid;

    /// Bounding box that encloses the primitive (in world space).
    BoundingBox bounds;

    TOSTRINGABLE(PrimitiveInfo);
};

std::string ToString(const PrimitiveInfo& info, const std::string& indent = "");

} // namespace fr
