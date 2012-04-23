#pragma once

#include <cstdint>
#include <limits>

#include "glm/glm.hpp"

#include "types/weak_hit.hpp"
#include "types/strong_hit.hpp"
#include "utils/tostring.hpp"
#include "utils/printers.hpp"

namespace fr {

struct Ray {
    enum Kind {
        NONE      = 0,
        INTERSECT = 1,
        LIGHT     = 2
    };

    explicit Ray(Kind kind, int16_t x, int16_t y) :
     kind(kind),
     x(x),
     y(y),
     weak(),
     strong(),
     next(nullptr) {
        bounces = std::numeric_limits<int16_t>::min();

        origin.x = std::numeric_limits<float>::quiet_NaN();
        origin.y = std::numeric_limits<float>::quiet_NaN();
        origin.z = std::numeric_limits<float>::quiet_NaN();

        direction.x = std::numeric_limits<float>::quiet_NaN();
        direction.y = std::numeric_limits<float>::quiet_NaN();
        direction.z = std::numeric_limits<float>::quiet_NaN();

        transmittance = std::numeric_limits<float>::quiet_NaN();

        emission.x = std::numeric_limits<float>::quiet_NaN();
        emission.y = std::numeric_limits<float>::quiet_NaN();
        emission.z = std::numeric_limits<float>::quiet_NaN();
    }

    explicit Ray(Kind kind) :
     kind(kind),
     weak(),
     strong(),
     next(nullptr) {
        x = std::numeric_limits<int16_t>::min();
        y = std::numeric_limits<int16_t>::min();

        bounces = std::numeric_limits<int16_t>::min();

        origin.x = std::numeric_limits<float>::quiet_NaN();
        origin.y = std::numeric_limits<float>::quiet_NaN();
        origin.z = std::numeric_limits<float>::quiet_NaN();

        direction.x = std::numeric_limits<float>::quiet_NaN();
        direction.y = std::numeric_limits<float>::quiet_NaN();
        direction.z = std::numeric_limits<float>::quiet_NaN();

        transmittance = std::numeric_limits<float>::quiet_NaN();

        emission.x = std::numeric_limits<float>::quiet_NaN();
        emission.y = std::numeric_limits<float>::quiet_NaN();
        emission.z = std::numeric_limits<float>::quiet_NaN();
    }

    explicit Ray() :
     kind(Kind::NONE),
     weak(),
     strong(),
     next(nullptr) {
        x = std::numeric_limits<int16_t>::min();
        y = std::numeric_limits<int16_t>::min();

        bounces = std::numeric_limits<int16_t>::min();

        origin.x = std::numeric_limits<float>::quiet_NaN();
        origin.y = std::numeric_limits<float>::quiet_NaN();
        origin.z = std::numeric_limits<float>::quiet_NaN();

        direction.x = std::numeric_limits<float>::quiet_NaN();
        direction.y = std::numeric_limits<float>::quiet_NaN();
        direction.z = std::numeric_limits<float>::quiet_NaN();

        transmittance = std::numeric_limits<float>::quiet_NaN();

        emission.x = std::numeric_limits<float>::quiet_NaN();
        emission.y = std::numeric_limits<float>::quiet_NaN();
        emission.z = std::numeric_limits<float>::quiet_NaN();
    }

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

inline std::string ToString(const Ray& ray, const std::string& indent = "") {
    std::stringstream stream;
    std::string pad = indent + "| ";
    stream << "Ray {" << std::endl;
    switch (ray.kind) {
        case Ray::Kind::NONE:
            stream << indent << "| kind = NONE" << std::endl;
            break;

        case Ray::Kind::INTERSECT:
            stream << indent << "| kind = INTERSECT" << std::endl <<
             indent << "| x = " << ray.x << std::endl <<
             indent << "| y = " << ray.y << std::endl <<
             indent << "| bounces = " << ray.bounces << std::endl <<
             indent << "| origin = " << ToString(ray.origin) << std::endl <<
             indent << "| direction = " << ToString(ray.direction) << std::endl <<
             indent << "| transmittance = " << ray.transmittance << std::endl <<
             indent << "| weak = " << ToString(ray.weak, pad) << std::endl <<
             indent << "| strong = " << ToString(ray.strong, pad) << std::endl <<
             indent << "| next = " << std::hex << std::showbase << ray.next << std::endl;
            break;

        case Ray::Kind::LIGHT:
            stream << indent << "| kind = LIGHT" << std::endl <<
             indent << "| x = " << ray.x << std::endl <<
             indent << "| y = " << ray.y << std::endl <<
             indent << "| origin = " << ToString(ray.origin) << std::endl <<
             indent << "| direction = " << ToString(ray.direction) << std::endl <<
             indent << "| transmittance = " << ray.transmittance << std::endl <<
             indent << "| emission = " << ToString(ray.emission) << std::endl <<
             indent << "| weak = " << ToString(ray.weak, pad) << std::endl <<
             indent << "| strong = " << ToString(ray.strong, pad) << std::endl <<
             indent << "| next = " << std::hex << std::showbase << ray.next << std::endl;
            break;
    }
    stream << indent << "}";
    return stream.str();
}

} // namespace fr
