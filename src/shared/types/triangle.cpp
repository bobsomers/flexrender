#include "types/triangle.hpp"

#include <iostream>
#include <sstream>

#include "types/skinny_ray.hpp"
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

vec3 Triangle::Sample(vec3* normal, vec2* texcoord) const {
    // TODO: implement actual sampling

    if (normal != nullptr) {
        *normal = verts[0].n;
    }

    if (texcoord != nullptr) {
        *texcoord = verts[0].t;
    }

    return verts[0].v;
}

bool Triangle::Intersect(const SkinnyRay& ray, float* t, LocalGeometry* local) const {
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

    // Compute the intersection parameter, t, and the final barycentric
    // coordinate, b0.
    *t = dot(e2, s2) * inv_divisor;
    float b0 = 1.0f - b1 - b2;

    // Bail if the intersection doesn't meet our epsilon requirements.
    if (*t < SELF_INTERSECT_EPSILON) {
        return false;
    }

    // Compute the interpolated normal from the barycentric coordinates.
    local->n = vec3((b0 * verts[0].n.x) + // first coordinate (x)
                    (b1 * verts[1].n.x) +
                    (b2 * verts[2].n.x),
                    (b0 * verts[0].n.y) + // second coordinate (y)
                    (b1 * verts[1].n.y) +
                    (b2 * verts[2].n.y),
                    (b0 * verts[0].n.z) + // third coordinate (z)
                    (b1 * verts[1].n.z) +
                    (b2 * verts[2].n.z));
    local->n = normalize(local->n);

    // Check the interpolated normal against the ray normal to cull back-facing
    // intersections.
    if (dot(local->n, ray.direction) > 0.0f) {
        return false;
    }

    // Compute the interpolated texture coordinate from the barycentric coords.
    local->t = vec2((b0 * verts[0].t.x) + // first coordinate (u)
                    (b1 * verts[1].t.x) +
                    (b2 * verts[2].t.x),
                    (b0 * verts[0].t.y) + // second coordinate (v)
                    (b1 * verts[1].t.y) +
                    (b2 * verts[2].t.y));

    // Intersection succeeded.
    return true;
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
