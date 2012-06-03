#pragma once

#include <cstdint>
#include <vector>

#include "glm/glm.hpp"
#include "msgpack.hpp"

#include "types/bounding_box.hpp"
#include "utils/tostring.hpp"

namespace fr {

struct SlimRay;
struct LocalGeometry;
struct Vertex;

struct Triangle {
    explicit Triangle(const uint32_t v1, const uint32_t v2, const uint32_t v3);

    // FOR MSGPACK ONLY!
    explicit Triangle();

    /// Vertices of the triangle.
    uint32_t verts[3];

    /**
     * Generates a sample point on the triangle in object space. If normal or
     * texcoord is non-null, they will be filled in with the interpolated
     * normal and texture coordinates, respectively.
     */
    void Sample(const std::vector<Vertex>& vertices, glm::vec3* position,
     glm::vec3* normal = nullptr, glm::vec2* texcoord = nullptr) const;

    /**
     * Returns the world space bounding box of the triangle given the object
     * to world transformation matrix.
     */
    BoundingBox WorldBounds(const std::vector<Vertex>& vertices,
     const glm::mat4& xform) const;

    /**
     * Intersects the given ray with this triangle and returns true if they
     * indeed intersect. If they do intersect, the passed t value and local
     * geometry structure are filled in with information about the point of
     * intersection.
     */
    bool Intersect(const std::vector<Vertex>& vertices,
     const SlimRay& ray, float* t, LocalGeometry* local) const;

    MSGPACK_DEFINE(verts[0], verts[1], verts[2]);

    TOSTRINGABLE(Triangle);

private:
    /// Computes the interpolated position in object space at the barycentric
    /// coordinates defined by <u, v, 1 - u - v>.
    glm::vec3 InterpolatePosition(const std::vector<Vertex>& vertices, float u,
     float v) const;

    /// Computes the interpolated surface normal in object space at the
    /// barycentric coordinates defined by <u, v, 1 - u - v>.
    glm::vec3 InterpolateNormal(const std::vector<Vertex>& vertices, float u,
     float v) const;

    /// Computes the interpolated texture coordinates in object space at the
    /// barycentric coordinates defined by <u, v, 1 - u - v>.
    glm::vec2 InterpolateTexCoord(const std::vector<Vertex>& vertices, float u,
     float v) const;
};

std::string ToString(const Triangle& tri, const std::string& indent = "");

} // namespace fr
