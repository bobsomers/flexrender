#include "utils/printers.hpp"

#include <sstream>
#include <iostream>

using std::string;
using std::stringstream;
using std::endl;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

namespace fr {

string ToString(vec2 vec, const string& indent) {
    stringstream stream;
    stream << "<" << vec.x << ", " << vec.y << ">";
    return stream.str();
}

string ToString(vec3 vec, const string& indent) {
    stringstream stream;
    stream << "<" << vec.x << ", " << vec.y << ", " << vec.z << ">";
    return stream.str();
}

string ToString(vec4 vec, const string& indent) {
    stringstream stream;
    stream << "<" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ">";
    return stream.str();
}

string ToString(mat4 mat, const string& indent) {
    stringstream stream;
    stream << "{" << endl <<
     indent << "| " << mat[0][0] << "\t" << mat[1][0] << "\t" << mat[2][0] << "\t" << mat[3][0] << endl <<
     indent << "| " << mat[0][1] << "\t" << mat[1][1] << "\t" << mat[2][1] << "\t" << mat[3][1] << endl <<
     indent << "| " << mat[0][2] << "\t" << mat[1][2] << "\t" << mat[2][2] << "\t" << mat[3][2] << endl <<
     indent << "| " << mat[0][3] << "\t" << mat[1][3] << "\t" << mat[2][3] << "\t" << mat[3][3] << endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
