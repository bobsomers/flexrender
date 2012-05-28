#include "types/fat_ray.hpp"

#include <limits>
#include <iostream>
#include <sstream>

#include "utils/printers.hpp"

using std::numeric_limits;
using std::string;
using std::stringstream;
using std::endl;
using std::hex;
using std::showbase;
using glm::vec3;
using glm::vec4;

namespace fr {

FatRay::FatRay(Kind kind, int16_t x, int16_t y) :
 kind(kind),
 x(x),
 y(y),
 slim(),
 traversal(),
 hit(),
 current_worker(0),
 workers_touched(0),
 next(nullptr) {
    bounces = numeric_limits<int16_t>::min();

    transmittance = numeric_limits<float>::quiet_NaN();

    emission.x = numeric_limits<float>::quiet_NaN();
    emission.y = numeric_limits<float>::quiet_NaN();
    emission.z = numeric_limits<float>::quiet_NaN();

    target.x = numeric_limits<float>::quiet_NaN();
    target.y = numeric_limits<float>::quiet_NaN();
    target.z = numeric_limits<float>::quiet_NaN();
}

FatRay::FatRay(Kind kind) :
 kind(kind),
 slim(),
 traversal(),
 hit(),
 current_worker(0),
 workers_touched(0),
 next(nullptr) {
    x = numeric_limits<int16_t>::min();
    y = numeric_limits<int16_t>::min();

    bounces = numeric_limits<int16_t>::min();

    transmittance = numeric_limits<float>::quiet_NaN();

    emission.x = numeric_limits<float>::quiet_NaN();
    emission.y = numeric_limits<float>::quiet_NaN();
    emission.z = numeric_limits<float>::quiet_NaN();

    target.x = numeric_limits<float>::quiet_NaN();
    target.y = numeric_limits<float>::quiet_NaN();
    target.z = numeric_limits<float>::quiet_NaN();
}

FatRay::FatRay() :
 kind(Kind::NONE),
 slim(),
 traversal(),
 hit(),
 current_worker(0),
 workers_touched(0),
 next(nullptr) {
    x = numeric_limits<int16_t>::min();
    y = numeric_limits<int16_t>::min();

    bounces = numeric_limits<int16_t>::min();

    transmittance = numeric_limits<float>::quiet_NaN();

    emission.x = numeric_limits<float>::quiet_NaN();
    emission.y = numeric_limits<float>::quiet_NaN();
    emission.z = numeric_limits<float>::quiet_NaN();

    target.x = numeric_limits<float>::quiet_NaN();
    target.y = numeric_limits<float>::quiet_NaN();
    target.z = numeric_limits<float>::quiet_NaN();
}

string ToString(const FatRay& ray, const string& indent) {
    stringstream stream;
    string pad = indent + "| ";
    stream << "FatRay {" << endl;
    switch (ray.kind) {
        case FatRay::Kind::NONE:
            stream << indent << "| kind = NONE" << endl;
            break;

        case FatRay::Kind::INTERSECT:
            stream << indent << "| kind = INTERSECT" << endl <<
             indent << "| x = " << ray.x << endl <<
             indent << "| y = " << ray.y << endl <<
             indent << "| bounces = " << ray.bounces << endl <<
             indent << "| slim = " << ToString(ray.slim, pad) << endl <<
             indent << "| transmittance = " << ray.transmittance << endl <<
             indent << "| traversal = " << ToString(ray.traversal, pad) << endl <<
             indent << "| hit = " << ToString(ray.hit, pad) << endl <<
             indent << "| current_worker = " << ray.current_worker << endl <<
             indent << "| workers_touched = " << ray.workers_touched << endl <<
             indent << "| next = " << hex << showbase << ray.next << endl;
            break;

        case FatRay::Kind::ILLUMINATE:
            stream << indent << "| kind = ILLUMINATE" << endl <<
             indent << "| x = " << ray.x << endl <<
             indent << "| y = " << ray.y << endl <<
             indent << "| bounces = " << ray.bounces << endl <<
             indent << "| slim = " << ToString(ray.slim, pad) << endl <<
             indent << "| transmittance = " << ray.transmittance << endl <<
             indent << "| traversal = " << ToString(ray.traversal, pad) << endl <<
             indent << "| hit = " << ToString(ray.hit, pad) << endl <<
             indent << "| current_worker = " << ray.current_worker << endl <<
             indent << "| workers_touched = " << ray.workers_touched << endl <<
             indent << "| next = " << hex << showbase << ray.next << endl;
            break;

        case FatRay::Kind::LIGHT:
            stream << indent << "| kind = LIGHT" << endl <<
             indent << "| x = " << ray.x << endl <<
             indent << "| y = " << ray.y << endl <<
             indent << "| slim = " << ToString(ray.slim, pad) << endl <<
             indent << "| transmittance = " << ray.transmittance << endl <<
             indent << "| emission = " << ToString(ray.emission) << endl <<
             indent << "| target = " << ToString(ray.target) << endl <<
             indent << "| traversal = " << ToString(ray.traversal, pad) << endl <<
             indent << "| hit = " << ToString(ray.hit, pad) << endl <<
             indent << "| current_worker = " << ray.current_worker << endl <<
             indent << "| workers_touched = " << ray.workers_touched << endl <<
             indent << "| next = " << hex << showbase << ray.next << endl;
            break;
    }
    stream << indent << "}";
    return stream.str();
}

} // namespace fr
