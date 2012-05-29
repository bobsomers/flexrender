#pragma once

#include <cstdint>
#include <limits>

#include "glm/glm.hpp"
#include "msgpack.hpp"

#include "utils/tostring.hpp"

namespace fr {

struct SlimRay;

struct BoundingBox {
    enum Axis {
        NONE = 0,
        X = 1,
        Y = 2,
        Z = 3
    };

    explicit BoundingBox(glm::vec3 min, glm::vec3 max);

    // FOR MSGPACK ONLY!
    explicit BoundingBox();

    /// The minimum coordinate of the bounding box.
    glm::vec3 min;

    /// The maximum coordinate of the bounding box.
    glm::vec3 max;

    /// Expands the bounding box to include the given point within its extents.
    void Absorb(glm::vec3 point);

    /// Returns a new bounding box that encloses the space within this bounding
    /// box and the passed bounding box.
    BoundingBox Union(const BoundingBox& other) const;

    /// Computes the surface area of this bounding box.
    float SurfaceArea() const;

    /// Returns the longest axis of the bounding box.
    Axis LongestAxis() const;

    /// Returns true of the bounding box extents are valid.
    bool inline IsValid() const {
        return min.x < max.x && min.y < max.y && min.z < max.z;
    }

    /// Performs an intersection check to see if the slim ray intersects
    /// the bounding box.
    bool Intersect(const SlimRay& ray, glm::vec3 inv_dir, float* t) const;

    MSGPACK_DEFINE(min, max);

    TOSTRINGABLE(BoundingBox);
};

inline float AxisComponent(glm::vec3 vec, BoundingBox::Axis axis) {
    float value = std::numeric_limits<float>::quiet_NaN();

    switch (axis) {
        case BoundingBox::Axis::X:
            value = vec.x;
            break;

        case BoundingBox::Axis::Y:
            value = vec.y;
            break;

        case BoundingBox::Axis::Z:
            value = vec.z;
            break;

        default:
            break;
    }

    return value;
}

inline float AxisComponent(glm::vec3 vec, uint64_t axis) {
    return AxisComponent(vec, static_cast<BoundingBox::Axis>(axis));
}

std::string ToString(const BoundingBox& box, const std::string& indent = "");

} // namespace fr
