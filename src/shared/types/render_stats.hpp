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

    uint64_t intersects_produced;
    uint64_t illuminates_produced;
    uint64_t lights_produced;
    uint64_t intersects_killed;
    uint64_t illuminates_killed;
    uint64_t lights_killed;
    uint64_t rays_rx;
    uint64_t rays_tx;
    uint64_t bytes_rx;
    uint64_t intersect_queue;
    uint64_t illuminate_queue;
    uint64_t light_queue;

    // Resets all the stats counters.
    inline void Reset() {
        intersects_produced = 0;
        illuminates_produced = 0;
        lights_produced = 0;
        intersects_killed = 0;
        illuminates_killed = 0;
        lights_killed = 0;
        rays_rx = 0;
        rays_tx = 0;
        bytes_rx = 0;
        intersect_queue = 0;
        illuminate_queue = 0;
        light_queue = 0;
    }

    MSGPACK_DEFINE(intersects_produced, illuminates_produced, lights_produced,
     intersects_killed, illuminates_killed, lights_killed, rays_rx, rays_tx,
     bytes_rx, intersect_queue, illuminate_queue, light_queue);
};

} // namespace fr
