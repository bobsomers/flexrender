#pragma once

#include <vector>

#include "ray_forward.hpp"
#include "buffer_op.hpp"

namespace fr {

struct WorkResults {
    explicit WorkResults() :
     forwards(),
     ops() {}

    /// Rays we need to forward.
    std::vector<RayForward> forwards;

    /// Buffer operations we need to do.
    std::vector<BufferOp> ops;
};

} // namespace fr
