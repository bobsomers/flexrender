#pragma once

namespace fr {

struct SkinnyRay;
struct Triangle;
struct LocalGeometry;

/**
 * Intersects the given ray with the given triangle and returns true if they
 * indeed intersect. If they do intersect, the passed t value and local
 * geometry structure are filled in with information about the point of
 * intersection.
 */
bool IntersectRayTri(const SkinnyRay& ray, const Triangle& tri, float* t,
 LocalGeometry* local);

} // namespace fr
