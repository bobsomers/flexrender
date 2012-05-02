#include "types/weak_hit.hpp"

#include <limits>
#include <iostream>
#include <sstream>

using std::numeric_limits;
using std::string;
using std::stringstream;
using std::endl;

namespace fr {

WeakHit::WeakHit(uint64_t worker) :
 worker(worker) {
    t = numeric_limits<float>::quiet_NaN();
}

WeakHit::WeakHit(uint64_t worker, float t) :
 worker(worker),
 t(t) {}

WeakHit::WeakHit() {
    worker = numeric_limits<uint64_t>::max();
    t = numeric_limits<float>::quiet_NaN();
}

string ToString(const WeakHit& weak, const string& indent) {
    stringstream stream;
    stream << "WeakHit {" << endl <<
     indent << "| worker = " << weak.worker << endl <<
     indent << "| t = " << weak.t << endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
