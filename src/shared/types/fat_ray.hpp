#pragma once

#include <cstdint>

#include "glm/glm.hpp"

#include "types/hit_record.hpp"
#include "types/mesh.hpp"
#include "types/slim_ray.hpp"
#include "types/traversal_state.hpp"
#include "utils/tostring.hpp"

namespace fr {

struct FatRay {
    enum Kind {
        NONE       = 0,
        INTERSECT  = 1,
        ILLUMINATE = 2,
        LIGHT      = 3
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

    /// The embedded slim ray (origin and direction).
    SlimRay slim;

    /// The transmittance of light along this ray. Range is not enforced.
    float transmittance;

    /// The light emission along this ray, not affected by transmittance.
    glm::vec3 emission;

    /// The target intersection point in world space.
    glm::vec3 target;

    /// State of the acceleration structure traversal.
    TraversalState traversal;

    /// The hit record of the closest intersection so far.
    HitRecord hit;

    /// Next pointer for chaining rays together. Obviously not valid once the
    /// ray has been sent over the network.
    FatRay* next;

    /// Returns a skinny ray that represents this fat ray transformed into
    /// object space of the given mesh.
    inline SlimRay TransformTo(const Mesh* mesh) const {
        return slim.TransformTo(mesh->xform_inv);
    }

    /// Evaluate a point along the ray at a specific t value.
    inline glm::vec3 EvaluateAt(float t) const {
        return slim.EvaluateAt(t);
    }

    TOSTRINGABLE(FatRay);
};

std::string ToString(const FatRay& ray, const std::string& indent = "");

} // namespace fr
