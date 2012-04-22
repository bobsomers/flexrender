#pragma once

#include <cstdint>
#include <limits>

#include "types/local_geometry.hpp"
#include "utils/tostring.hpp"

namespace fr {

struct StrongHit {
    explicit StrongHit(uint64_t worker, uint64_t mesh) :
     worker(worker),
     mesh(mesh),
     geom(geom) {
        t = std::numeric_limits<float>::infinity();
    }

    explicit StrongHit(uint64_t worker, uint64_t mesh, float t) :
     worker(worker),
     mesh(mesh),
     t(t),
     geom(geom) {}

    explicit StrongHit() :
     geom() {
        worker = std::numeric_limits<uint64_t>::max();
        mesh = std::numeric_limits<uint64_t>::max();
        t = std::numeric_limits<float>::infinity();
    }

    /// Resource ID of worker this hit occurred on.
    uint64_t worker;

    /// Resource ID of the mesh this hit occurred on.
    uint64_t mesh;

    /// The t value of the intersection.
    float t;

    /// The local geometry at the point of intersection.
    LocalGeometry geom;

    TOSTRINGABLE(StrongHit);
};

inline std::string ToString(const StrongHit& strong, const std::string& indent = "") {
    std::stringstream stream;
    std::string pad = indent + "| ";
    stream << "StrongHit {" << std::endl <<
     indent << "| worker = " << strong.worker << std::endl <<
     indent << "| mesh = " << strong.mesh << std::endl <<
     indent << "| t = " << strong.t << std::endl <<
     indent << "| geom = " << ToString(strong.geom, pad) <<
     indent << "}" << std::endl;
    return stream.str();
}

} // namespace fr
