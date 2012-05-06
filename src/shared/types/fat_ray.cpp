#include "types/fat_ray.hpp"

#include <limits>
#include <iostream>
#include <sstream>

#include "types/mesh.hpp"
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
 skinny(),
 weak(),
 strong(),
 next(nullptr) {
    bounces = numeric_limits<int16_t>::min();

    transmittance = numeric_limits<float>::quiet_NaN();

    emission.x = numeric_limits<float>::quiet_NaN();
    emission.y = numeric_limits<float>::quiet_NaN();
    emission.z = numeric_limits<float>::quiet_NaN();
}

FatRay::FatRay(Kind kind) :
 kind(kind),
 skinny(),
 weak(),
 strong(),
 next(nullptr) {
    x = numeric_limits<int16_t>::min();
    y = numeric_limits<int16_t>::min();

    bounces = numeric_limits<int16_t>::min();

    transmittance = numeric_limits<float>::quiet_NaN();

    emission.x = numeric_limits<float>::quiet_NaN();
    emission.y = numeric_limits<float>::quiet_NaN();
    emission.z = numeric_limits<float>::quiet_NaN();
}

FatRay::FatRay() :
 kind(Kind::NONE),
 skinny(),
 weak(),
 strong(),
 next(nullptr) {
    x = numeric_limits<int16_t>::min();
    y = numeric_limits<int16_t>::min();

    bounces = numeric_limits<int16_t>::min();

    transmittance = numeric_limits<float>::quiet_NaN();

    emission.x = numeric_limits<float>::quiet_NaN();
    emission.y = numeric_limits<float>::quiet_NaN();
    emission.z = numeric_limits<float>::quiet_NaN();
}

SkinnyRay FatRay::TransformTo(const Mesh* mesh) {
    vec4 o(skinny.origin, 1.0f);
    vec4 d(skinny.direction, 0.0f);
    return SkinnyRay(vec3(mesh->xform_inv * o),
                     vec3(mesh->xform_inv * d));
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
             indent << "| skinny = " << ToString(ray.skinny, pad) << endl <<
             indent << "| transmittance = " << ray.transmittance << endl <<
             indent << "| weak = " << ToString(ray.weak, pad) << endl <<
             indent << "| strong = " << ToString(ray.strong, pad) << endl <<
             indent << "| next = " << hex << showbase << ray.next << endl;
            break;

        case FatRay::Kind::LIGHT:
            stream << indent << "| kind = LIGHT" << endl <<
             indent << "| x = " << ray.x << endl <<
             indent << "| y = " << ray.y << endl <<
             indent << "| skinny = " << ToString(ray.skinny) << endl <<
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
