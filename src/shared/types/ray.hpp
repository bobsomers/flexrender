#pragma once

#include <cstdint>

#include "glm/glm.hpp"

#include "types/weak_hit.hpp"
#include "types/strong_hit.hpp"
#include "utils/tostring.hpp"

namespace fr {

struct Ray {
    enum Kind {
        NONE      = 0,
        INTERSECT = 1,
        LIGHT     = 2
    };

    explicit Ray(Kind kind, int16_t x, int16_t y);

    explicit Ray(Kind kind);

    explicit Ray();

    /// The kind of ray (from the above possible).
    int16_t kind; 

    /// The x component of the source pixel.
    int16_t x;

    /// The y component of the source pixel.
    int16_t y;

    /// The number of times this ray has bounced (zero is primary).
    int16_t bounces;

    /// The origin position of the ray.
    glm::vec3 origin;

    /// The normalized direction of the ray. Unit length is not enforced.
    glm::vec3 direction;

    /// The transmittance of light along this ray. Range is not enforced.
    float transmittance;

    /// The light emission along this ray, not affected by transmittance.
    glm::vec3 emission;

    /// The next weak hit to check during scene traversal.
    WeakHit weak;

    /// The terminating strong hit, if one has been found.
    StrongHit strong;

    /// Next pointer for chaining rays together. Obviously not valid once the
    /// ray has been sent over the network.
    Ray* next;

    /// Evaluate a point along the ray at a specific t value.
    inline glm::vec3 operator()(float t) const { return direction * t + origin; }

    TOSTRINGABLE(Ray);
};

std::string ToString(const Ray& ray, const std::string& indent = "");

} // namespace fr
