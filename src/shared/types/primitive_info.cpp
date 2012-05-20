#include "types/primitive_info.hpp"

#include <iostream>
#include <sstream>

#include "utils/printers.hpp"

using std::string;
using std::stringstream;
using std::endl;
using glm::vec3;

namespace fr {

PrimitiveInfo::PrimitiveInfo(size_t index, const BoundingBox& bounds) :
 index(index),
 bounds(bounds) {
    centroid = 0.5f * bounds.min + 0.5f * bounds.max;
}

string ToString(const PrimitiveInfo& info, const string& indent) {
    stringstream stream;
    stream << "PrimitiveInfo {" << endl <<
     indent << "| index = " << info.index << endl <<
     indent << "| centroid = " << ToString(info.centroid) << endl <<
     indent << "| bounds = " << ToString(info.bounds) << endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
