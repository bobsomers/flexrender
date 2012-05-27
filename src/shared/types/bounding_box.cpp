#include "types/bounding_box.hpp"

#include <limits>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <utility>

#include "types/slim_ray.hpp"
#include "utils/printers.hpp"

using std::numeric_limits;
using std::string;
using std::stringstream;
using std::endl;
using std::swap;
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

BoundingBox::Axis BoundingBox::LongestAxis() const {
    vec3 d = max - min;
    if (d.x > d.y && d.x > d.z) {
        return Axis::X;
    } else if (d.y > d.z) {
        return Axis::Y;
    }
    return Axis::Z;
}

bool BoundingBox::Intersect(const SlimRay& ray, vec3 inv_dir, float* t) const {
    // Check the X slab.
    float x_near = (min.x - ray.origin.x) * inv_dir.x;
    float x_far = (max.x - ray.origin.x) * inv_dir.x;
    if (x_near > x_far) swap(x_near, x_far);

    float t_min = x_near;
    float t_max = x_far;
    if (t_min > t_max) return false;

    // Check the Y slab.
    float y_near = (min.y - ray.origin.y) * inv_dir.y;
    float y_far = (max.y - ray.origin.y) * inv_dir.y;
    if (y_near > y_far) swap(y_near, y_far);

    t_min = y_near > t_min ? y_near : t_min;
    t_max = y_far < t_max ? y_far : t_max;
    if (t_min > t_max) return false;

    // Check the Z slab.
    float z_near = (min.z - ray.origin.z) * inv_dir.z;
    float z_far = (max.z - ray.origin.z) * inv_dir.z;
    if (z_near > z_far) swap(z_near, z_far);

    t_min = z_near > t_min ? z_near : t_min;
    t_max = z_far < t_max ? z_far : t_max;
    if (t_min > t_max) return false;

    if (t_max < 0.0f) return false; // Box behind the ray.

    *t = t_min;
    return true;
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
