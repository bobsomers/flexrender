#include "types/bounding_box.hpp"

#include <limits>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "utils/printers.hpp"

using std::numeric_limits;
using std::string;
using std::stringstream;
using std::endl;
using glm::vec3;

namespace fr {

BoundingBox::BoundingBox(vec3 min, vec3 max) :
 min(min),
 max(max) {}

BoundingBox::BoundingBox() {
    min.x = numeric_limits<float>::infinity();
    min.y = numeric_limits<float>::infinity();
    min.z = numeric_limits<float>::infinity();

    max.x = -numeric_limits<float>::infinity();
    max.y = -numeric_limits<float>::infinity();
    max.z = -numeric_limits<float>::infinity();
}

void BoundingBox::Absorb(vec3 point) {
    min.x = std::min(min.x, point.x);
    min.y = std::min(min.y, point.y);
    min.z = std::min(min.z, point.z);

    max.x = std::max(max.x, point.x);
    max.y = std::max(max.y, point.y);
    max.z = std::max(max.z, point.z);
}

BoundingBox BoundingBox::Union(const BoundingBox& other) const {
    BoundingBox result;

    result.min.x = std::min(min.x, other.min.x);
    result.min.y = std::min(min.y, other.min.y);
    result.min.z = std::min(min.z, other.min.z);

    result.max.x = std::max(max.x, other.max.x);
    result.max.y = std::max(max.y, other.max.y);
    result.max.z = std::max(max.z, other.max.z);

    return result;
}

float BoundingBox::SurfaceArea() const {
    vec3 d = max - min;
    return 2.0f * (d.x * d.y + d.x * d.z + d.y * d.z);
}

string ToString(const BoundingBox& box, const string& indent) {
    stringstream stream;
    stream << "BoundingBox {" << endl <<
     indent << "| min = " << ToString(box.min) << endl <<
     indent << "| max = " << ToString(box.max) << endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
