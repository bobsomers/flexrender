#pragma once

#include <vector>

#include "ray_forward.hpp"

namespace fr {

struct WorkResults {
    explicit WorkResults() :
     forwards() {}

    /// Rays we need to forward.
    std::vector<RayForward> forwards;
};

} // namespace fr
