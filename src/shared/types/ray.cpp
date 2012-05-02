#include "types/ray.hpp"

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

namespace fr {

Ray::Ray(Kind kind, int16_t x, int16_t y) :
 kind(kind),
 x(x),
 y(y),
 weak(),
 strong(),
 next(nullptr) {
    bounces = numeric_limits<int16_t>::min();

    origin.x = numeric_limits<float>::quiet_NaN();
    origin.y = numeric_limits<float>::quiet_NaN();
    origin.z = numeric_limits<float>::quiet_NaN();

    direction.x = numeric_limits<float>::quiet_NaN();
    direction.y = numeric_limits<float>::quiet_NaN();
    direction.z = numeric_limits<float>::quiet_NaN();

    transmittance = numeric_limits<float>::quiet_NaN();

    emission.x = numeric_limits<float>::quiet_NaN();
    emission.y = numeric_limits<float>::quiet_NaN();
    emission.z = numeric_limits<float>::quiet_NaN();
}

Ray::Ray(Kind kind) :
 kind(kind),
 weak(),
 strong(),
 next(nullptr) {
    x = numeric_limits<int16_t>::min();
    y = numeric_limits<int16_t>::min();

    bounces = numeric_limits<int16_t>::min();

    origin.x = numeric_limits<float>::quiet_NaN();
    origin.y = numeric_limits<float>::quiet_NaN();
    origin.z = numeric_limits<float>::quiet_NaN();

    direction.x = numeric_limits<float>::quiet_NaN();
    direction.y = numeric_limits<float>::quiet_NaN();
    direction.z = numeric_limits<float>::quiet_NaN();

    transmittance = numeric_limits<float>::quiet_NaN();

    emission.x = numeric_limits<float>::quiet_NaN();
    emission.y = numeric_limits<float>::quiet_NaN();
    emission.z = numeric_limits<float>::quiet_NaN();
}

Ray::Ray() :
 kind(Kind::NONE),
 weak(),
 strong(),
 next(nullptr) {
    x = numeric_limits<int16_t>::min();
    y = numeric_limits<int16_t>::min();

    bounces = numeric_limits<int16_t>::min();

    origin.x = numeric_limits<float>::quiet_NaN();
    origin.y = numeric_limits<float>::quiet_NaN();
    origin.z = numeric_limits<float>::quiet_NaN();

    direction.x = numeric_limits<float>::quiet_NaN();
    direction.y = numeric_limits<float>::quiet_NaN();
    direction.z = numeric_limits<float>::quiet_NaN();

    transmittance = numeric_limits<float>::quiet_NaN();

    emission.x = numeric_limits<float>::quiet_NaN();
    emission.y = numeric_limits<float>::quiet_NaN();
    emission.z = numeric_limits<float>::quiet_NaN();
}

string ToString(const Ray& ray, const string& indent) {
    stringstream stream;
    string pad = indent + "| ";
    stream << "Ray {" << endl;
    switch (ray.kind) {
        case Ray::Kind::NONE:
            stream << indent << "| kind = NONE" << endl;
            break;

        case Ray::Kind::INTERSECT:
            stream << indent << "| kind = INTERSECT" << endl <<
             indent << "| x = " << ray.x << endl <<
             indent << "| y = " << ray.y << endl <<
             indent << "| bounces = " << ray.bounces << endl <<
             indent << "| origin = " << ToString(ray.origin) << endl <<
             indent << "| direction = " << ToString(ray.direction) << endl <<
             indent << "| transmittance = " << ray.transmittance << endl <<
             indent << "| weak = " << ToString(ray.weak, pad) << endl <<
             indent << "| strong = " << ToString(ray.strong, pad) << endl <<
             indent << "| next = " << hex << showbase << ray.next << endl;
            break;

        case Ray::Kind::LIGHT:
            stream << indent << "| kind = LIGHT" << endl <<
             indent << "| x = " << ray.x << endl <<
             indent << "| y = " << ray.y << endl <<
             indent << "| origin = " << ToString(ray.origin) << endl <<
             indent << "| direction = " << ToString(ray.direction) << endl <<
             indent << "| transmittance = " << ray.transmittance << endl <<
             indent << "| emission = " << ToString(ray.emission) << endl <<
             indent << "| weak = " << ToString(ray.weak, pad) << endl <<
             indent << "| strong = " << ToString(ray.strong, pad) << endl <<
             indent << "| next = " << hex << showbase << ray.next << endl;
            break;
    }
    stream << indent << "}";
    return stream.str();
}

} // namespace fr
