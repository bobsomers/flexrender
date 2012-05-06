#include "types/strong_hit.hpp"

#include <limits>
#include <iostream>
#include <sstream>

using std::numeric_limits;
using std::string;
using std::stringstream;
using std::endl;

namespace fr {

StrongHit::StrongHit(uint64_t worker, uint64_t mesh) :
 worker(worker),
 mesh(mesh),
 geom(geom) {
    t = numeric_limits<float>::infinity();
}

StrongHit::StrongHit(uint64_t worker, uint64_t mesh, float t) :
 worker(worker),
 mesh(mesh),
 t(t),
 geom(geom) {}

StrongHit::StrongHit() :
 geom() {
    worker = numeric_limits<uint64_t>::max();
    mesh = numeric_limits<uint64_t>::max();
    t = numeric_limits<float>::infinity();
}

string ToString(const StrongHit& strong, const string& indent) {
    stringstream stream;
    string pad = indent + "| ";
    stream << "StrongHit {" << endl <<
     indent << "| worker = " << strong.worker << endl <<
     indent << "| mesh = " << strong.mesh << endl <<
     indent << "| t = " << strong.t << endl <<
     indent << "| geom = " << ToString(strong.geom, pad) <<
     indent << "}";
    return stream.str();
}

} // namespace fr
