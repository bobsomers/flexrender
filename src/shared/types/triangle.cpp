#include "types/triangle.hpp"

#include <iostream>
#include <sstream>
#include <cassert>
#include <cstdlib>

#include "types/slim_ray.hpp"
#include "types/local_geometry.hpp"

using std::string;
using std::stringstream;
using std::endl;
using glm::vec2;
using glm::vec3;
using glm::cross;
using glm::dot;
using glm::normalize;

namespace fr {

Triangle::Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
    verts[0] = v1;
    verts[1] = v2;
    verts[2] = v3;
}

Triangle::Triangle() {
    verts[0] = Vertex();
    verts[1] = Vertex();
    verts[2] = Vertex();
}

void Triangle::Sample(vec3* position, vec3* normal, vec2* texcoord) const {
    assert(position != nullptr);

    // Credit: Physically Based Rendering, page 671.
    float r1 = rand() / static_cast<float>(RAND_MAX);
    float r2 = rand() / static_cast<float>(RAND_MAX);

    float sqr1 = sqrtf(r1);
    float u = 1.0f - sqr1;
    float v = r2 * sqr1;

    *position = InterpolatePosition(u, v);

    if (normal != nullptr) {
        *normal = InterpolateNormal(u, v);
    }

    if (texcoord != nullptr) {
        *texcoord = InterpolateTexCoord(u, v);
    }
}

bool Triangle::Intersect(const SlimRay& ray, float* t, LocalGeometry* local) const {
    // Credit: Physically Based Rendering, page 141, with modifications.

    // First compute s1, edge vectors, and denominator.
    vec3 e1 = verts[1].v - verts[0].v;
    vec3 e2 = verts[2].v - verts[0].v;
    vec3 s1 = cross(ray.direction, e2);
    float divisor = dot(s1, e1);
    if (divisor == 0.0f) {
        return false;
    }
    float inv_divisor = 1.0f / divisor;

    // Next, compute the first barycentric coordinate, b1.
    vec3 d = ray.origin - verts[0].v;
    float b1 = dot(d, s1) * inv_divisor;
    if (b1 < 0.0f || b1 > 1.0f) {
        return false;
    }

    // Next, compute the second barycentric coordinate, b2.
    vec3 s2 = cross(d, e1);
    float b2 = dot(ray.direction, s2) * inv_divisor;
    if (b2 < 0.0f || b1 + b2 > 1.0f) {
        return false;
    }

    // Compute the intersection parameter, t.
    *t = dot(e2, s2) * inv_divisor;

    // Bail if the intersection doesn't meet our epsilon requirements.
    if (*t < SELF_INTERSECT_EPSILON) {
        return false;
    }

    // Compute the interpolated normal from the barycentric coordinates.
    local->n = InterpolateNormal(b1, b2);

    // Check the interpolated normal against the ray normal to cull back-facing
    // intersections.
    if (dot(local->n, ray.direction) > 0.0f) {
        return false;
    }

    // Compute the interpolated texture coordinate from the barycentric coords.
    local->t = InterpolateTexCoord(b1, b2);

    // Intersection succeeded.
    return true;
}

vec3 Triangle::InterpolatePosition(float u, float v) const {
    float w = 1.0f - u - v;

    return vec3(
        (w * verts[0].v.x) + // first coordinate (x)
        (u * verts[1].v.x) +
        (v * verts[2].v.x),
        (w * verts[0].v.y) + // second coordinate (y)
        (u * verts[1].v.y) +
        (v * verts[2].v.y),
        (w * verts[0].v.z) + // third coordinate (z)
        (u * verts[1].v.z) +
        (v * verts[2].v.z)
    );
}

vec3 Triangle::InterpolateNormal(float u, float v) const {
    float w = 1.0f - u - v;

    return normalize(vec3(
        (w * verts[0].n.x) + // first coordinate (x)
        (u * verts[1].n.x) +
        (v * verts[2].n.x),
        (w * verts[0].n.y) + // second coordinate (y)
        (u * verts[1].n.y) +
        (v * verts[2].n.y),
        (w * verts[0].n.z) + // third coordinate (z)
        (u * verts[1].n.z) +
        (v * verts[2].n.z)
    ));
}

vec2 Triangle::InterpolateTexCoord(float u, float v) const {
    float w = 1.0f - u - v;

    return vec2(
        (w * verts[0].t.x) + // first coordinate (u)
        (u * verts[1].t.x) +
        (v * verts[2].t.x),
        (w * verts[0].t.y) + // second coordinate (v)
        (u * verts[1].t.y) +
        (v * verts[2].t.y)
    );
}

string ToString(const Triangle& tri, const string& indent) {
    stringstream stream;
    string pad = indent + "| ";
    stream << "Triangle {" << endl <<
     indent << "| v1 = " << ToString(tri.verts[0], pad) << endl << 
     indent << "| v2 = " << ToString(tri.verts[1], pad) << endl <<
     indent << "| v3 = " << ToString(tri.verts[2], pad) << endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
