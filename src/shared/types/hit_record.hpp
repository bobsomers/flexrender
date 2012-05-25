#pragma once

#include <cstdint>

#include "types/local_geometry.hpp"
#include "utils/tostring.hpp"

namespace fr {

struct HitRecord {
    explicit HitRecord(uint32_t worker, uint32_t mesh);

    explicit HitRecord(uint32_t worker, uint32_t mesh, float t);

    explicit HitRecord();

    /// Resource ID of worker this hit occurred on.
    uint32_t worker;

    /// Resource ID of the mesh this hit occurred on.
    uint32_t mesh;

    /// The t value of the intersection.
    float t;

    /// The local geometry at the point of intersection.
    LocalGeometry geom;

    TOSTRINGABLE(HitRecord);
};

std::string ToString(const HitRecord& strong, const std::string& indent = "");

} // namespace fr
