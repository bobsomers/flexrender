#include "types/image.hpp"

#include <limits>

#include "OpenEXR/ImfOutputFile.h"
#include "OpenEXR/ImfChannelList.h"

using std::numeric_limits;
using std::string;

namespace fr {

Image::Image(int16_t width, int16_t height) :
 _width(width),
 _height(height),
 _buffers() {
     AddBuffer("R");
     AddBuffer("G");
     AddBuffer("B");
}

Image::Image() :
 _buffers() {
    _width = numeric_limits<int16_t>::min();
    _height = numeric_limits<int16_t>::min();
}

void Image::AddBuffer(const string& name) {
    _buffers[name] = Buffer(_width, _height, 0.0f);
}

void Image::Merge(const Image* other) {
    for (const auto& kv_pair : other->_buffers) {
        _buffers[kv_pair.first].Merge(kv_pair.second);
    }
}

void Image::ToEXRFile(const string& filename) const {
    // Create the header and channel list.
    Imf::Header header(_width, _height);
    for (const auto& kv_pair : _buffers) {
        const auto& name = kv_pair.first;
        header.channels().insert(name.c_str(), Imf::Channel(Imf::FLOAT));
    }

    // Create the output file and frame buffer.
    Imf::OutputFile file(filename.c_str(), header);
    Imf::FrameBuffer frame;

    // Set up the memory layout.
    for (const auto& kv_pair : _buffers) {
        const auto& name = kv_pair.first;
        const auto& buffer = kv_pair.second;
        frame.insert(name.c_str(), Imf::Slice(Imf::FLOAT,
         const_cast<char*>(reinterpret_cast<const char*>(&(buffer._data[0]))),
         sizeof(float), _width * sizeof(float)));
    }

    // Write it out.
    file.setFrameBuffer(frame);
    file.writePixels(_height);
}

} // namespace fr
