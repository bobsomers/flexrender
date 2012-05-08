#include "intersection.hpp"

#include "glm/glm.hpp"

#include "types.hpp"

using glm::vec3;
using glm::vec2;
using glm::cross;
using glm::dot;
using glm::normalize;

namespace fr {

/// Avoid self-intersection by only recognizing intersections that occur
/// at this minimum t-value along the ray.
const float INTERSECT_EPSILON = 0.0001f;

bool IntersectRayTri(const SkinnyRay& ray, const Triangle& tri, float* t,
 LocalGeometry* local) {
    // From Physically Based Rendering, page 141.
    
    // First compute s1, edge vectors, and denominator.
    vec3 e1 = tri.verts[1].v - tri.verts[0].v;
    vec3 e2 = tri.verts[2].v - tri.verts[0].v;
    vec3 s1 = cross(ray.direction, e2);
    float divisor = dot(s1, e1);
    if (divisor == 0.0f) {
        return false;
    }
    float inv_divisor = 1.0f / divisor;

    // Next, compute the first barycentric coordinate, b1.
    vec3 d = ray.origin - tri.verts[0].v;
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
    if (*t < INTERSECT_EPSILON) {
        return false;
    }

    // Compute the interpolated normal from the barycentric coordinates.
    local->n = vec3((b0 * tri.verts[0].n.x) + // first coordinate (x)
                    (b1 * tri.verts[1].n.x) +
                    (b2 * tri.verts[2].n.x),
                    (b0 * tri.verts[0].n.y) + // second coordinate (y)
                    (b1 * tri.verts[1].n.y) +
                    (b2 * tri.verts[2].n.y),
                    (b0 * tri.verts[0].n.z) + // third coordinate (z)
                    (b1 * tri.verts[1].n.z) +
                    (b2 * tri.verts[2].n.z));
    local->n = normalize(local->n);

    // Compute the interpolated texture coordinate from the barycentric coords.
    local->t = vec2((b0 * tri.verts[0].t.x) + // first coordinate (u)
                    (b1 * tri.verts[1].t.x) +
                    (b2 * tri.verts[2].t.x),
                    (b0 * tri.verts[0].t.y) + // second coordinate (v)
                    (b1 * tri.verts[1].t.y) +
                    (b2 * tri.verts[2].t.y));

    // Intersection succeeded.
    return true;
}

} // flexrender
