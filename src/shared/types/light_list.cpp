#include "types/light_list.hpp"

#include <algorithm>

using std::function;
using std::find;

namespace fr {

LightList::LightList() :
 _workers() {}

void LightList::AddEmissiveWorker(uint32_t id) {
    auto existing = find(_workers.begin(), _workers.end(), id);
    if (existing == _workers.end()) {
        _workers.push_back(id);
    }
}

void LightList::ForEachEmissiveWorker(function<void (uint32_t)> func) {
    for (auto id : _workers) {
        func(id);
    }
}

} // namespace fr
