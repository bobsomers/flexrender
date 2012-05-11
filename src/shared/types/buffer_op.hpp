#pragma once

#include <cstdint>
#include <string>

namespace fr {

struct BufferOp {
    enum class Kind {
        WRITE,
        ACCUMULATE
    };

    explicit BufferOp(Kind kind, const std::string& buffer, int16_t x, int16_t y,
     float value) :
     kind(kind),
     buffer(buffer),
     x(x),
     y(y),
     value(value) {}

    /// The kind of buffer operation we're doing.
    Kind kind;

    /// The buffer we're writing into.
    std::string buffer;

    /// The x coordinate of the pixel.
    int16_t x;

    /// The y coordinate of the pixel.
    int16_t y;

    /// The value we're writing.
    float value;
};

} // namespace fr
