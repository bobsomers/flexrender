#include "scripting/script.hpp"

#include <sstream>
#include <limits>

#include "utils/tout.hpp"

using std::string;
using std::stringstream;
using std::istringstream;
using std::function;
using std::numeric_limits;
using glm::vec2;
using glm::vec3;
using glm::vec4;

namespace fr {

Script::Script() :
 _state(nullptr) {
    // Empty.
}

Script::~Script() {
    if (_state != nullptr) {
        lua_close(_state);
    }
}

void Script::Init(ScriptLibs libs) {
    // Create a new interpreter state.
    _state = luaL_newstate();
    if (_state == nullptr) {
        TERRLN("Failed to allocate new script interpreter state.");
        exit(EXIT_FAILURE);
    }

    // Open libraries, if requested.
    if (libs == ScriptLibs::STANDARD_LIBS) {
        luaL_openlibs(_state);
    }
}

void Script::DumpStack() {
    TOUTLN("Script Stack {");

    // From Programming in Lua, listing 24.2, with modifications.
    int top = lua_gettop(_state);
    for (int i = 1; i <= top; i++) {
        TOUT("[" << i << "] = ");
        int t = lua_type(_state, i);
        switch(t) {
            case LUA_TSTRING: {
                TOUT("'" << lua_tostring(_state, i) << "'");
            } break;

            case LUA_TBOOLEAN: {
                TOUT((lua_toboolean(_state, i) ? "true" : "false"));
            } break;

            case LUA_TNUMBER: {
                TOUT(lua_tonumber(_state, i));
            } break;

            default: {
                TOUT("<" << lua_typename(_state, t) << ">");
            } break;
        }
        TOUTLN("");
    }

    TOUTLN("}");
}

void Script::ScriptError(const string& message) {
    luaL_error(_state, message.c_str());
}

void Script::CallFunc(int num_args, int num_returns) {
    if (lua_pcall(_state, num_args, num_returns, 0)) {
        ScriptError(lua_tostring(_state, -1));
    }
}

void Script::BeginTableCall() {
    luaL_checktype(_state, 1, LUA_TTABLE);
    lua_pushvalue(_state, 1);
}

void Script::EndTableCall() {
    lua_pop(_state, 1);
}

int Script::ReturnResourceID(uint64_t id) {
    stringstream stream;
    stream << id;
    lua_pushstring(_state, stream.str().c_str());
    return 1;
}

uint64_t Script::DecodeResourceID(string value) {
    istringstream stream(value);
    uint64_t id;
    stream >> id;
    return id;
}

void Script::TypeCheck(int type, const string& name) {
    if (lua_type(_state, -1) != type) {
        stringstream message;
        message << "expected ";
        if (name != "") {
            message << name << " to be a ";
        }
        message << lua_typename(_state, type) << ", got a " <<
         lua_typename(_state, lua_type(_state, -1)) << " instead";
        ScriptError(message.str());
    }
}

bool Script::PushField(const char* name, int field_type) {
    lua_getfield(_state, -1, name);
    if (lua_isnil(_state, -1)) {
        return false;
    }
    TypeCheck(field_type, name);
    return true;
}

void Script::PopField() {
    lua_pop(_state, 1);
}

void Script::ForEachIndex(function<void (size_t)> func) {
    for (size_t i = 1; i <= lua_objlen(_state, -1); i++) {
        func(i);
    }
}

void Script::PushIndex(size_t index, int index_type) {
    lua_rawgeti(_state, -1, index);
    stringstream name;
    name << "[" << index << "]";
    TypeCheck(index_type, name.str());
}

void Script::PopIndex() {
    lua_pop(_state, 1);
}

void Script::ForEachKeyVal(function<void (const string &key)> func) {
    lua_pushnil(_state); // for first iteration
    while (lua_next(_state, -2) != 0) {
        string key = string(lua_tostring(_state, -2));
        func(key);
        lua_pop(_state, 1);
    }
}

string Script::FetchString() {
    return string(lua_tostring(_state, -1));
}

bool Script::FetchBool() {
    return lua_toboolean(_state, -1) ? true : false;
}

float Script::FetchFloat() {
    return static_cast<float>(lua_tonumber(_state, -1));
}

void Script::PushFloat(float f) {
    lua_pushnumber(_state, f);
}

vec2 Script::FetchFloat2() {
    vec2 vec;
    vec.x = numeric_limits<float>::quiet_NaN();
    vec.y = numeric_limits<float>::quiet_NaN();

    uint32_t count = 0;
    ForEachIndex([this, &vec, &count](size_t index) {
        if (count > 1) {
            ScriptError("expected 2 numbers in table");
        }

        PushIndex(index, LUA_TNUMBER);
        vec[index - 1] = FetchFloat();
        PopIndex();
        count++;
    });

    if (count != 2) {
        ScriptError("expected 2 numbers in table");
    }

    return vec;
}

void Script::PushFloat2(vec2 v) {
    lua_newtable(_state);
    lua_pushnumber(_state, v.x);
    lua_rawseti(_state, -2, 1);
    lua_pushnumber(_state, v.y);
    lua_rawseti(_state, -2, 2);
}

vec3 Script::FetchFloat3() {
    vec3 vec;
    vec.x = numeric_limits<float>::quiet_NaN();
    vec.y = numeric_limits<float>::quiet_NaN();
    vec.z = numeric_limits<float>::quiet_NaN();

    uint32_t count = 0;
    ForEachIndex([this, &vec, &count](size_t index) {
        if (count > 2) {
            ScriptError("expected 3 numbers in table");
        }

        PushIndex(index, LUA_TNUMBER);
        vec[index - 1] = FetchFloat();
        PopIndex();
        count++;
    });

    if (count != 3) {
        ScriptError("expected 3 numbers in table");
    }

    return vec;
}

void Script::PushFloat3(vec3 v) {
    lua_newtable(_state);
    lua_pushnumber(_state, v.x);
    lua_rawseti(_state, -2, 1);
    lua_pushnumber(_state, v.y);
    lua_rawseti(_state, -2, 2);
    lua_pushnumber(_state, v.z);
    lua_rawseti(_state, -2, 3);
}

vec4 Script::FetchFloat4() {
    vec4 vec;
    vec.x = numeric_limits<float>::quiet_NaN();
    vec.y = numeric_limits<float>::quiet_NaN();
    vec.z = numeric_limits<float>::quiet_NaN();
    vec.w = numeric_limits<float>::quiet_NaN();

    uint32_t count = 0;
    ForEachIndex([this, &vec, &count](size_t index) {
        if (count > 3) {
            ScriptError("expected 4 numbers in table");
        }

        PushIndex(index, LUA_TNUMBER);
        vec[index - 1] = FetchFloat();
        PopIndex();
        count++;
    });

    if (count != 4) {
        ScriptError("expected 4 numbers in table");
    }

    return vec;
}

void Script::PushFloat4(vec4 v) {
    lua_newtable(_state);
    lua_pushnumber(_state, v.x);
    lua_rawseti(_state, -2, 1);
    lua_pushnumber(_state, v.y);
    lua_rawseti(_state, -2, 2);
    lua_pushnumber(_state, v.z);
    lua_rawseti(_state, -2, 3);
    lua_pushnumber(_state, v.w);
    lua_rawseti(_state, -2, 4);
}

} // namespace fr
