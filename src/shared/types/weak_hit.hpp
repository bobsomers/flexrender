#pragma once

#include <cstdint>
#include <limits>

#include "utils/tostring.hpp"

namespace fr {

struct WeakHit {
    explicit WeakHit(uint64_t worker) :
     worker(worker) {
        t = std::numeric_limits<float>::infinity();
    }

    explicit WeakHit(uint64_t worker, float t) :
     worker(worker),
     t(t) {}

    explicit WeakHit() {
        worker = std::numeric_limits<uint64_t>::max();
        t = std::numeric_limits<float>::infinity();
    }

    /// Resource ID of worker this hit occurred on.
    uint64_t worker;

    /// The t value of the intersection.
    float t;

    TOSTRINGABLE(WeakHit);
};

inline std::string ToString(const WeakHit& weak, const std::string& indent = "") {
    std::stringstream stream;
    stream << "WeakHit {" << std::endl <<
     indent << "| worker = " << weak.worker << std::endl <<
     indent << "| t = " << weak.t << std::endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
