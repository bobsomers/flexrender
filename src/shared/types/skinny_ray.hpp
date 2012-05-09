#pragma once

#include "glm/glm.hpp"

#include "utils/tostring.hpp"

namespace fr {

/// Avoid self-intersection by only recognizing intersections that occur
/// at this minimum t-value along the ray.
extern const float SELF_INTERSECT_EPSILON;

/// A light ray must hit within this distance of its target to say it has hit
/// the target.
extern const float TARGET_INTERSECT_EPSILON;

struct SkinnyRay {
    explicit SkinnyRay(glm::vec3 origin, glm::vec3 direction);

    explicit SkinnyRay();

    /// The origin position of the ray.
    glm::vec3 origin;

    /// The normalized direction of the ray. Unit length is not enforced.
    glm::vec3 direction;

    /// Evaluate a point along the ray at a specific t value.
    inline glm::vec3 EvaluateAt(float t) const { return direction * t + origin; }

    TOSTRINGABLE(SkinnyRay);
};

std::string ToString(const SkinnyRay& ray, const std::string& indent = "");

} // namespace fr
