#include "types/shader.hpp"

#include <limits>
#include <iostream>
#include <sstream>

#include "scripting/shader_script.hpp"

using std::numeric_limits;
using std::string;
using std::stringstream;
using std::endl;

namespace fr {

Shader::Shader(uint64_t id) :
 id(id),
 code(""),
 script(nullptr) {}

Shader::Shader(uint64_t id, const string& code) :
 id(id),
 code(code),
 script(nullptr) {}

Shader::Shader() :
 code(""),
 script(nullptr) {
    id = numeric_limits<uint64_t>::max(); 
}

Shader::~Shader() {
    if (script != nullptr) delete script;
}

string ToString(const Shader& shader, const string& indent) {
    stringstream stream;
    stream << "Shader {" << endl <<
     indent << "| id = " << shader.id << endl <<
     indent << "| code = ..." << endl <<
"======================================================================" << endl <<
shader.code << endl <<
"======================================================================" << endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
