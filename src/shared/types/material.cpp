#include "types/material.hpp"

#include <limits>
#include <iostream>
#include <sstream>

using std::numeric_limits;
using std::string;
using std::stringstream;
using std::endl;

namespace fr {

Material::Material(uint32_t id) :
 id(id),
 textures(),
 emissive(false) {
    shader = numeric_limits<uint32_t>::max();
}

Material::Material(uint32_t id, uint32_t shader) :
 id(id),
 shader(shader),
 textures(),
 emissive(false) {}

Material::Material(uint32_t id, uint32_t shader, bool emissive) :
 id(id),
 shader(shader),
 textures(),
 emissive(emissive) {}

Material::Material() :
 textures(),
 emissive(false) {
    id = numeric_limits<uint32_t>::max();
    shader = numeric_limits<uint32_t>::max();
}

string ToString(const Material& mat, const string& indent) {
    stringstream stream;
    stream << "Material {" << endl <<
     indent << "| id = " << mat.id << endl <<
     indent << "| shader = " << mat.shader << endl <<
     indent << "| textures = {" << endl;
    for (const auto& binding : mat.textures) {
        stream << indent << "| | " << binding.first << " -> " << binding.second << endl;
    }
    stream << indent << "| }" << endl <<
     indent << "| emissive = " << (mat.emissive ? "true" : "false") << endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
