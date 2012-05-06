#pragma once

#include <cstdint>

#include "msgpack.hpp"

/// Interval between sending render stats back to the renderer.
#define FR_STATS_TIMEOUT_MS 250

namespace fr {

struct RenderStats {
    explicit RenderStats() {
        Reset();
    }

    uint64_t rays_processed;
    uint64_t rays_rx;
    uint64_t rays_tx;
    uint64_t bytes_rx;

    // Resets all the stats counters.
    inline void Reset() {
        rays_processed = 0;
        rays_rx = 0;
        rays_tx = 0;
        bytes_rx = 0;
    }

    MSGPACK_DEFINE(rays_processed, rays_rx, rays_tx, bytes_rx);
};

} // namespace fr
