#pragma once

#include <cstdint>

#include "glm/glm.hpp"

#include "types/skinny_ray.hpp"
#include "types/weak_hit.hpp"
#include "types/strong_hit.hpp"
#include "utils/tostring.hpp"

namespace fr {

struct Mesh;

struct FatRay {
    enum Kind {
        NONE      = 0,
        INTERSECT = 1,
        LIGHT     = 2
    };

    explicit FatRay(Kind kind, int16_t x, int16_t y);

    explicit FatRay(Kind kind);

    explicit FatRay();

    /// The kind of ray (from the above possible).
    uint16_t kind;

    /// The x component of the source pixel.
    int16_t x;

    /// The y component of the source pixel.
    int16_t y;

    /// The number of times this ray has bounced (zero is primary).
    int16_t bounces;

    /// The embedded skinny ray (origin and direction).
    SkinnyRay skinny;

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
    FatRay* next;

    /// Returns a skinny ray that represents this fat ray transformed into
    /// object space of the given mesh.
    SkinnyRay TransformTo(const Mesh* mesh);

    /// Evaluate a point along the ray at a specific t value.
    inline glm::vec3 operator()(float t) const { return skinny(t); }

    TOSTRINGABLE(FatRay);
};

std::string ToString(const FatRay& ray, const std::string& indent = "");

} // namespace fr
