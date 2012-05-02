#include "utils/printers.hpp"

#include <sstream>

using std::string;
using std::stringstream;
using std::endl;
using glm::vec2;
using glm::vec3;
using glm::vec4;

namespace fr {

string ToString(vec2 vec, const string& indent) {
    stringstream stream;
    stream << indent << "<" << vec.x << ", " << vec.y << ">";
    return stream.str();
}

string ToString(vec3 vec, const string& indent) {
    stringstream stream;
    stream << indent << "<" << vec.x << ", " << vec.y << ", " << vec.z << ">";
    return stream.str();
}

string ToString(vec4 vec, const string& indent) {
    stringstream stream;
    stream << indent <<
     "<" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ">";
    return stream.str();
}

} // namespace fr
