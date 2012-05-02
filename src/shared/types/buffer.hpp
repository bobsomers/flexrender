#pragma once

#include <cstdint>
#include <vector>

#include "msgpack.hpp"

namespace fr {

class Buffer {
public:
    explicit Buffer(uint64_t id, int16_t width, int16_t height, float value = 0.0f);

    // FOR MSGPACK ONLY!
    explicit Buffer();

    /// Resource ID of the buffer.
    uint64_t id;

    /// Merges the contents of the other buffer with this one. (Accumulate()'s
    /// pixel-wise.)
    void Merge(const Buffer* other);

    /// Overwrites the value at position <x, y> in the buffer with the given
    /// one.
    inline void Write(int16_t x, int16_t y, float value) {
        size_t index = (y * _width) + x;
        _data[index] = value;
    }

    /// Accumulates the the given value with the current value in the buffer
    /// at position <x, y>.
    inline void Accumulate(int16_t x, int16_t y, float value) {
        size_t index = (y * _width) + x;
        _data[index] += value;
    }

    MSGPACK_DEFINE(id, _width, _height, _data);

private:
    int16_t _width;
    int16_t _height;
    std::vector<float> _data;
};

} // namespace fr
