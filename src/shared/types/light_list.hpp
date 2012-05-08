#pragma once

#include <cstdint>
#include <vector>
#include <functional>

#include "msgpack.hpp"

namespace fr {

class LightList {
public:
    explicit LightList();

    void AddEmissiveWorker(uint64_t id);

    void ForEachEmissiveWorker(std::function<void (uint64_t)> func);

    MSGPACK_DEFINE(_workers);

private:
    std::vector<uint64_t> _workers;
};

} // namespace fr
