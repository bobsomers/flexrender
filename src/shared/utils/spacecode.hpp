#pragma once

#include <cstdint>

#include "glm/glm.hpp"

// Currently uses Morton coding for a Z-order curve.
// See: http://en.wikipedia.org/wiki/Z-order_(curve)

namespace fr {

/// Minimum value of the space encoding.
extern const uint64_t SPACECODE_MIN;

/// Maximum value of the space encoding.
extern const uint64_t SPACECODE_MAX;

/**
 * Encodes a point in 3D space to a designated space encoding (currently Morton
 * coding for a Z-order curve), given the associated minimum and maximum bounds
 * of the space.
 *
 * @param   point   The 3D point to encode.
 * @param   min     The minimum corner of an axis-aligned bounding box that
 *                  encloses the space.
 * @param   max     The maximum corner of an axis-aligned bounding box that
 *                  encloses the space.
 */
uint64_t SpaceEncode(const glm::vec3& point, const glm::vec3& min, const glm::vec3& max);

} // namespace fr
