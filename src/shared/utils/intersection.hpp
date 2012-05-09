#pragma once

namespace fr {

struct SkinnyRay;
struct Triangle;
struct LocalGeometry;

/// Avoid self-intersection by only recognizing intersections that occur
/// at this minimum t-value along the ray.
extern const float SELF_INTERSECT_EPSILON;

/// A light ray must hit within this distance of its target to say it has hit
/// the target.
extern const float TARGET_INTERSECT_EPSILON;

/**
 * Intersects the given ray with the given triangle and returns true if they
 * indeed intersect. If they do intersect, the passed t value and local
 * geometry structure are filled in with information about the point of
 * intersection.
 */
bool IntersectRayTri(const SkinnyRay& ray, const Triangle& tri, float* t,
 LocalGeometry* local);

} // namespace fr
