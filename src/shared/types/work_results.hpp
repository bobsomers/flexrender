#pragma once

#include <cstdint>
#include <vector>

#include "ray_forward.hpp"
#include "buffer_op.hpp"

namespace fr {

struct WorkResults {
    explicit WorkResults() :
     forwards(),
     ops(),
     intersects_produced(0),
     illuminates_produced(0),
     lights_produced(0),
     intersects_killed(0),
     illuminates_killed(0),
     lights_killed(0) {}

    /// Rays we need to forward.
    std::vector<RayForward> forwards;

    /// Buffer operations we need to do.
    std::vector<BufferOp> ops;

    /// Workers touched.
    std::map<uint32_t, uint64_t> workers_touched;

    /// Intersect rays produced.
    uint64_t intersects_produced;

    /// Illuminate rays produced.
    uint64_t illuminates_produced;

    /// Light rays produced.
    uint64_t lights_produced;

    /// Intersect rays killed.
    uint64_t intersects_killed;

    /// Illuminate rays killed.
    uint64_t illuminates_killed;

    /// Light rays killed.
    uint64_t lights_killed;
};

} // namespace fr
