#include "types/slim_ray.hpp"

#include <limits>
#include <iostream>
#include <sstream>

#include "utils/printers.hpp"

using std::numeric_limits;
using std::string;
using std::stringstream;
using std::endl;
using glm::vec3;

namespace fr {

const float SELF_INTERSECT_EPSILON = 0.0001f;

const float TARGET_INTERSECT_EPSILON = 0.0001f;

SlimRay::SlimRay(vec3 origin, vec3 direction) :
 origin(origin),
 direction(direction) {}

SlimRay::SlimRay() {
    origin.x = numeric_limits<float>::quiet_NaN();
    origin.y = numeric_limits<float>::quiet_NaN();
    origin.z = numeric_limits<float>::quiet_NaN();

    direction.x = numeric_limits<float>::quiet_NaN();
    direction.y = numeric_limits<float>::quiet_NaN();
    direction.z = numeric_limits<float>::quiet_NaN();
}

string ToString(const SlimRay& ray, const string& indent) {
    stringstream stream;
    string pad = indent + "| ";
    stream << "SlimRay {" << endl <<
     indent << "| origin = " << ToString(ray.origin) << endl <<
     indent << "| direction = " << ToString(ray.direction) << endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
