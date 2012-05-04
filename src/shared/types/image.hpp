#pragma once

#include <cstdint>
#include <string>
#include <map>

#include "types/buffer.hpp"

namespace fr {

class Image {
public:
    explicit Image(int16_t width, int16_t height);

    // FOR MSGPACK ONLY!
    explicit Image();

    /// Adds a new buffer to the image with the given name.
    void AddBuffer(const std::string& name);

    /// Merges the other image with this one by folding each buffer into its
    /// corresponding buffer using accumulation.
    void Merge(const Image* other);

    /// Overwrites the value at location <x, y> with the given value in the
    /// named buffer.
    inline void Write(const std::string& buffer, int16_t x, int16_t y, float value) {
        _buffers[buffer].Write(x, y, value);
    }

    /// Accumulates the given value with the existing value at location <x, y>
    /// in the named buffer.
    inline void Accumulate(const std::string& buffer, int16_t x, int16_t y, float value) {
        _buffers[buffer].Accumulate(x, y, value);
    }

    /// Dumps all the buffers out to an EXR file.
    void ToEXRFile(const std::string& filename) const;

    MSGPACK_DEFINE(_width, _height, _buffers);

private:
    int16_t _width;
    int16_t _height;
    std::map<std::string, Buffer> _buffers;
};

} // namespace fr
