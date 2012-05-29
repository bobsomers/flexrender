#pragma once

#include <cstdint>
#include <map>

#include "msgpack.hpp"

namespace fr {

struct TraversalStats {
    explicit TraversalStats() {
        Reset();
    }

    std::map<uint32_t, uint64_t> workers_touched;

    // Resets all the stats counters.
    inline void Reset() {
    }

    MSGPACK_DEFINE(workers_touched);
};

} // namespace fr

