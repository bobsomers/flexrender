#pragma once

#include <cstdint>

#include "utils/tostring.hpp"

namespace fr {

struct WeakHit {
    explicit WeakHit(uint32_t worker);

    explicit WeakHit(uint32_t worker, float t);

    explicit WeakHit();

    /// Resource ID of worker this hit occurred on.
    uint32_t worker;

    /// The t value of the intersection.
    float t;

    TOSTRINGABLE(WeakHit);
};

std::string ToString(const WeakHit& weak, const std::string& indent = "");

} // namespace fr
