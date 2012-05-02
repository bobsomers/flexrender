#include "types/buffer.hpp"

#include <limits>

using std::numeric_limits;

namespace fr {

Buffer::Buffer(uint64_t id, int16_t width, int16_t height, float value) :
 id(id),
 _width(width),
 _height(height),
 _data(width * height, value) {

}

Buffer::Buffer() :
 _data() {
    id = numeric_limits<uint64_t>::max();
    _width = numeric_limits<int64_t>::min();
    _height = numeric_limits<int64_t>::min();
}
 
void Buffer::Merge(const Buffer* other) {
    for (size_t i = 0; i < other->_data.size(); i++) {
        _data[i] += other->_data[i];
    }
}

} // namespace fr
