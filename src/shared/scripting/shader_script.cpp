#include "scripting/shader_script.hpp"

#include <cstdlib>
#include <stdio.h>
#include <errno.h>

#include "types.hpp"
#include "utils.hpp"

using std::string;
using glm::vec2;
using glm::vec3;

namespace fr {

ShaderScript::ShaderScript(const string& code, const Library* lib) :
 _lib(lib),
 _has_direct(false),
 _has_indirect(false),
 _has_emissive(false) {
    // TODO: Shader scripts shouldn't have access to the whole standard
    // library...
    FR_SCRIPT_INIT(ShaderScript, ScriptLibs::STANDARD_LIBS);

    // Register function handlers with the interpreter.
    FR_SCRIPT_REGISTER("accumulate", ShaderScript, Accumulate1);
    FR_SCRIPT_REGISTER("accumulate2", ShaderScript, Accumulate2);
    FR_SCRIPT_REGISTER("accumulate3", ShaderScript, Accumulate3);
    FR_SCRIPT_REGISTER("accumulate4", ShaderScript, Accumulate4);
    FR_SCRIPT_REGISTER("write", ShaderScript, Write1);
    FR_SCRIPT_REGISTER("write2", ShaderScript, Write2);
    FR_SCRIPT_REGISTER("write3", ShaderScript, Write3);
    FR_SCRIPT_REGISTER("write4", ShaderScript, Write4);
    FR_SCRIPT_REGISTER("texture", ShaderScript, Texture1);
    FR_SCRIPT_REGISTER("texture2", ShaderScript, Texture2);
    FR_SCRIPT_REGISTER("texture3", ShaderScript, Texture3);
    FR_SCRIPT_REGISTER("texture4", ShaderScript, Texture4);
    FR_SCRIPT_REGISTER("trace", ShaderScript, Trace);

    // Evaluate the shader.
    if (luaL_dostring(_state, code.c_str())) {
        TERRLN(lua_tostring(_state, -1));
        exit(EXIT_FAILURE);
    }
    
    // Check to see which function exist.
    lua_getglobal(_state, "direct");
    _has_direct = lua_isfunction(_state, -1) != 0;
    lua_pop(_state, 1);

    lua_getglobal(_state, "indirect");
    _has_direct = lua_isfunction(_state, -1) != 0;
    lua_pop(_state, 1);

    lua_getglobal(_state, "emissive");
    _has_direct = lua_isfunction(_state, -1) != 0;
    lua_pop(_state, 1);

    // Do they have local aliases for vec2's and vec3's? If so, we can set
    // vector metatables appropriately when we push arguments onto the stack.
    lua_getglobal(_state, "vec2");
    _has_vec2 = lua_istable(_state, -1) != 0;
    lua_pop(_state, 1);

    lua_getglobal(_state, "vec3");
    _has_vec3 = lua_istable(_state, -1) != 0;
    lua_pop(_state, 1);

    // Initialize lock.
    if (sem_init(&_lock, 0, 1) < 0) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }
}

void ShaderScript::Direct(vec3 view, vec3 normal, vec2 texcoord, vec3 light,
 vec3 illumination, WorkResults* results) {
    if (!_has_direct) return;

    // Acquire the interpreter lock.
    if (sem_wait(&_lock) < 0) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }

    // Locate the function.
    lua_getglobal(_state, "direct");

    // Push the arguments onto the stack.
    PushFloat3(view);
    PushFloat3(normal);
    PushFloat2(texcoord);
    PushFloat3(light);
    PushFloat3(illumination);

    // Call the function.
    CallFunc(5, 0);
    lua_pop(_state, 1);

    // Release the interpreter lock.
    if (sem_post(&_lock) < 0) {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }
}

void ShaderScript::Indirect(vec3 view, vec3 normal, vec2 texcoord,
 WorkResults* results) {
    if (!_has_indirect) return;

    if (sem_wait(&_lock) < 0) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }

    // TODO
    
    if (sem_post(&_lock) < 0) {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }
}

void ShaderScript::Emissive(vec3 view, vec3 normal, vec2 texcoord,
 WorkResults* results) {
    if (!_has_emissive) return;

    if (sem_wait(&_lock) < 0) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }

    // TODO
    
    if (sem_post(&_lock) < 0) {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }
}

void ShaderScript::Accumulate() {
    // TODO
}

void ShaderScript::Write() {
    // TODO
}

void ShaderScript::Texture() {
    // TODO
}

FR_SCRIPT_FUNCTION(ShaderScript, Accumulate1) {
    // TODO
    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Accumulate2) {
    // TODO
    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Accumulate3) {
    // TODO
    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Accumulate4) {
    // TODO
    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Write1) {
    // TODO
    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Write2) {
    // TODO
    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Write3) {
    // TODO
    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Write4) {
    // TODO
    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Texture1) {
    // TODO
    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Texture2) {
    // TODO
    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Texture3) {
    // TODO
    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Texture4) {
    // TODO
    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Trace) {
    // TODO
    return 0;
}

} // namespace fr
