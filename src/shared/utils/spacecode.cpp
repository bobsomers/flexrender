#include "utils/spacecode.hpp"

using glm::vec3;

namespace fr {

const uint64_t SPACECODE_MIN = 0;
const uint64_t SPACECODE_MAX = 0x7fffffffffffffff; // msb = 0, everything else 1's

uint64_t SpaceEncode(vec3 point, vec3 min, vec3 max) {
    // 21 (x) + 21 (y) + 21 (z) = 63 (< 64) bits total.
    const uint32_t bits_per_component = 21;
    
    // Scaled value between 0 -> 1 on each axis.
    vec3 scaled = (point - min) / (max - min);

    // Multiply by 2^bits_per_component to get the discrete value along each
    // axis.
    uint32_t factor = 0x1 << bits_per_component;
    uint32_t discrete_x = scaled.x * factor;
    uint32_t discrete_y = scaled.y * factor;
    uint32_t discrete_z = scaled.z * factor;

    // Interleave the bits by shifting in the x/z/y components
    // bits_per_component times.
    uint64_t morton = 0;
    for (int32_t bit = bits_per_component - 1; bit >= 0; bit--) {
        // Set the bitmask.
        uint32_t mask = 0x1 << bit;

        // Extract the next bit from each component.
        uint32_t bit_x = (discrete_x & mask) >> bit;
        uint32_t bit_y = (discrete_y & mask) >> bit;
        uint32_t bit_z = (discrete_z & mask) >> bit;

        // Make space on the right hand side of the Morton code.
        morton = morton << 3;

        // Set the x/y/z components.
        morton |= ((bit_x << 2) | (bit_z << 1) | (bit_y << 0));
    }

    return morton;
}

} // namespace fr
