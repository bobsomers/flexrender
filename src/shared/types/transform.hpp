#pragma once

#include <cstdint>
#include <limits>

#include "glm/glm.hpp"
#include "msgpack.hpp"

#include "utils/tostring.hpp"
#include "utils/printers.hpp"

namespace fr {

struct Transform {
    enum Kind {
        NONE      = 0,
        ROTATE    = 1,
        SCALE     = 2,
        TRANSLATE = 3
    };

    explicit Transform(uint32_t kind, float scalar, glm::vec3 vec) :
     kind(kind),
     scalar(scalar),
     vec(vec) {}

    explicit Transform(uint32_t kind, float scalar) :
     kind(kind),
     scalar(scalar) {
        vec.x = std::numeric_limits<float>::quiet_NaN();
        vec.y = std::numeric_limits<float>::quiet_NaN();
        vec.z = std::numeric_limits<float>::quiet_NaN();
    }

    explicit Transform(uint32_t kind, glm::vec3 vec) :
     kind(kind),
     vec(vec) {
        scalar = std::numeric_limits<float>::quiet_NaN();
    }

    // FOR MSGPACK ONLY!
    explicit Transform() :
     kind(Kind::NONE) {
        scalar = std::numeric_limits<float>::quiet_NaN();
        vec.x = std::numeric_limits<float>::quiet_NaN();
        vec.y = std::numeric_limits<float>::quiet_NaN();
        vec.z = std::numeric_limits<float>::quiet_NaN();
    }

    /// The kind of transform (from the above possible).
    uint32_t kind;

    /// Scalar value (interpretation depends on kind).
    float scalar;

    /// Vector value (interpretation depends on kind).
    glm::vec3 vec;

    MSGPACK_DEFINE(kind, scalar, vec);

    TOSTRINGABLE(Transform);
};

inline std::string ToString(const Transform& xform, const std::string& indent = "") {
    std::stringstream stream;
    stream << "Transform {" << std::endl;
    switch (xform.kind) {
        case Transform::Kind::NONE:
            stream << indent << "  kind = NONE" << std::endl;
            break;
        
        case Transform::Kind::ROTATE:
            stream << indent << "  kind = ROTATE" << std::endl <<
             indent << "  scalar = " << xform.scalar << std::endl <<
             indent << "  vec = " << ToString(xform.vec) << std::endl;
            break;

        case Transform::Kind::SCALE:
            stream << indent << "  kind = SCALE" << std::endl <<
             indent << "  vec = " << ToString(xform.vec) << std::endl;
            break;

        case Transform::Kind::TRANSLATE:
            stream << indent << "  kind = TRANSLATE" << std::endl <<
             indent << "  vec = " << ToString(xform.vec) << std::endl;
            break;

        default:
            stream << indent << "  kind = ?" << std::endl;
            break;
    }
    stream << indent << "}";
    return stream.str();
}

} // namespace fr
