#include "types/triangle.hpp"

#include <iostream>
#include <sstream>
#include <cassert>
#include <cstdlib>

#include "types/slim_ray.hpp"
#include "types/local_geometry.hpp"
#include "types/vertex.hpp"

using std::string;
using std::stringstream;
using std::endl;
using std::vector;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::cross;
using glm::dot;
using glm::normalize;

namespace fr {

Triangle::Triangle(const uint32_t v1, const uint32_t v2, const uint32_t v3) {
    verts[0] = v1;
    verts[1] = v2;
    verts[2] = v3;
}

Triangle::Triangle() {
    verts[0] = 0;
    verts[1] = 0;
    verts[2] = 0;
}

void Triangle::Sample(const vector<Vertex>& vertices, vec3* position,
 vec3* normal, vec2* texcoord) const {
    assert(position != nullptr);

    // Credit: Physically Based Rendering, page 671.
    float r1 = rand() / static_cast<float>(RAND_MAX);
    float r2 = rand() / static_cast<float>(RAND_MAX);

    float sqr1 = sqrtf(r1);
    float u = 1.0f - sqr1;
    float v = r2 * sqr1;

    *position = InterpolatePosition(vertices, u, v);

    if (normal != nullptr) {
        *normal = InterpolateNormal(vertices, u, v);
    }

    if (texcoord != nullptr) {
        *texcoord = InterpolateTexCoord(vertices, u, v);
    }
}

BoundingBox Triangle::WorldBounds(const vector<Vertex>& vertices,
 const mat4& xform) const {
    BoundingBox bounds;
    bounds.Absorb(vec3(xform * vec4(vertices[verts[0]].v, 1.0f)));
    bounds.Absorb(vec3(xform * vec4(vertices[verts[1]].v, 1.0f)));
    bounds.Absorb(vec3(xform * vec4(vertices[verts[2]].v, 1.0f)));
    return bounds;
}

bool Triangle::Intersect(const vector<Vertex>& vertices, const SlimRay& ray,
 float* t, LocalGeometry* local) const {
    // Credit: Physically Based Rendering, page 141, with modifications.
    
    vec3 v1 = vertices[verts[0]].v;
    vec3 v2 = vertices[verts[1]].v;
    vec3 v3 = vertices[verts[2]].v;

    // First compute s1, edge vectors, and denominator.
    vec3 e1 = v2 - v1;
    vec3 e2 = v3 - v1;
    vec3 s1 = cross(ray.direction, e2);
    float divisor = dot(s1, e1);
    if (divisor == 0.0f) {
        return false;
    }
    float inv_divisor = 1.0f / divisor;

    // Next, compute the first barycentric coordinate, b1.
    vec3 d = ray.origin - v1;
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
    local->n = InterpolateNormal(vertices, b1, b2);

    // Check the interpolated normal against the ray normal to cull back-facing
    // intersections.
    if (dot(local->n, ray.direction) > 0.0f) {
        return false;
    }

    // Compute the interpolated texture coordinate from the barycentric coords.
    local->t = InterpolateTexCoord(vertices, b1, b2);

    // Intersection succeeded.
    return true;
}

vec3 Triangle::InterpolatePosition(const vector<Vertex>& vertices, float u,
 float v) const {
    vec3 v1 = vertices[verts[0]].v;
    vec3 v2 = vertices[verts[1]].v;
    vec3 v3 = vertices[verts[2]].v;

    float w = 1.0f - u - v;

    return vec3(
        (w * v1.x) + // first coordinate (x)
        (u * v2.x) +
        (v * v3.x),
        (w * v1.y) + // second coordinate (y)
        (u * v2.y) +
        (v * v3.y),
        (w * v1.z) + // third coordinate (z)
        (u * v2.z) +
        (v * v3.z)
    );
}

vec3 Triangle::InterpolateNormal(const vector<Vertex>& vertices, float u,
 float v) const {
    vec3 n1 = vertices[verts[0]].n;
    vec3 n2 = vertices[verts[1]].n;
    vec3 n3 = vertices[verts[2]].n;

    float w = 1.0f - u - v;

    return normalize(vec3(
        (w * n1.x) + // first coordinate (x)
        (u * n2.x) +
        (v * n3.x),
        (w * n1.y) + // second coordinate (y)
        (u * n2.y) +
        (v * n3.y),
        (w * n1.z) + // third coordinate (z)
        (u * n2.z) +
        (v * n3.z)
    ));
}

vec2 Triangle::InterpolateTexCoord(const vector<Vertex>& vertices, float u,
 float v) const {
    vec2 t1 = vertices[verts[0]].t;
    vec2 t2 = vertices[verts[1]].t;
    vec2 t3 = vertices[verts[2]].t;

    float w = 1.0f - u - v;

    return vec2(
        (w * t1.x) + // first coordinate (u)
        (u * t2.x) +
        (v * t3.x),
        (w * t1.y) + // second coordinate (v)
        (u * t2.y) +
        (v * t3.y)
    );
}

string ToString(const Triangle& tri, const string& indent) {
    stringstream stream;
    string pad = indent + "| ";
    stream << "Triangle {" << endl <<
     indent << "| v1 = " << tri.verts[0] << endl <<
     indent << "| v2 = " << tri.verts[1] << endl <<
     indent << "| v3 = " << tri.verts[2] << endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
