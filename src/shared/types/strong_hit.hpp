#pragma once

#include <cstdint>

#include "types/local_geometry.hpp"
#include "utils/tostring.hpp"

namespace fr {

struct StrongHit {
    explicit StrongHit(uint64_t worker, uint64_t mesh);

    explicit StrongHit(uint64_t worker, uint64_t mesh, float t);

    explicit StrongHit();

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

std::string ToString(const StrongHit& strong, const std::string& indent = "");

} // namespace fr
