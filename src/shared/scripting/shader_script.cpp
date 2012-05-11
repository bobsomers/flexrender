#include "scripting/shader_script.hpp"

#include <cstdlib>
#include <stdio.h>
#include <errno.h>
#include <string>

#include "types.hpp"
#include "utils.hpp"

using std::string;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::normalize;

namespace fr {

ShaderScript::ShaderScript(const string& code, const Library* lib) :
 _lib(lib),
 _ray(nullptr),
 _results(nullptr),
 _has_direct(false),
 _has_indirect(false),
 _has_emissive(false) {
    // TODO: Shader scripts shouldn't have access to the whole standard
    // library...
    FR_SCRIPT_INIT(ShaderScript, ScriptLibs::STANDARD_LIBS);

    // Register function handlers with the interpreter.
    FR_SCRIPT_REGISTER("accumulate", ShaderScript, Accumulate);
    FR_SCRIPT_REGISTER("accumulate2", ShaderScript, Accumulate2);
    FR_SCRIPT_REGISTER("accumulate3", ShaderScript, Accumulate3);
    FR_SCRIPT_REGISTER("accumulate4", ShaderScript, Accumulate4);
    FR_SCRIPT_REGISTER("write", ShaderScript, Write);
    FR_SCRIPT_REGISTER("write2", ShaderScript, Write2);
    FR_SCRIPT_REGISTER("write3", ShaderScript, Write3);
    FR_SCRIPT_REGISTER("write4", ShaderScript, Write4);
    FR_SCRIPT_REGISTER("texture", ShaderScript, Texture);
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

void ShaderScript::Direct(const FatRay* ray, vec3 hit, WorkResults *results) {
    if (!_has_direct) return;

    Camera* cam = _lib->LookupCamera();

    // Compute the vectors we're passing to the shader.
    vec3 view = normalize(cam->eye - hit);
    vec3 normal = ray->strong.geom.n;
    vec2 texcoord = ray->strong.geom.t;
    vec3 light = -ray->skinny.direction;
    vec3 illumination = ray->emission;

    // Acquire the interpreter lock.
    if (sem_wait(&_lock) < 0) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }

    // Set the current data we're operating on.
    _ray = ray;
    _results = results;

    // Locate the function.
    lua_getglobal(_state, "direct");

    // Push the arguments onto the stack.
    PushFloat3(view);
    PushFloat3(normal);
    PushFloat2(texcoord);
    PushFloat3(light);
    PushFloat3(illumination);

    // Set metatables for arguments if we can.
    if (_has_vec2) {
        lua_getglobal(_state, "vec2");
        lua_setmetatable(_state, -4); // texcoord (metatable is at -1)
    }
    if (_has_vec3) {
        lua_getglobal(_state, "vec3");
        lua_setmetatable(_state, -6); // view (metatable is at -1)
        lua_getglobal(_state, "vec3");
        lua_setmetatable(_state, -5); // normal (metatable is at -1)
        lua_getglobal(_state, "vec3");
        lua_setmetatable(_state, -3); // light (metatable is at -1)
        lua_getglobal(_state, "vec3");
        lua_setmetatable(_state, -2); // illumination (metatable is at -1)
    }

    // Call the function.
    CallFunc(5, 0);
    // No need to pop, 0 return values.

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

FR_SCRIPT_FUNCTION(ShaderScript, Accumulate) {
    string buffer(luaL_checkstring(_state, 1));
    float value = static_cast<float>(luaL_checknumber(_state, 2));

    _results->ops.emplace_back(BufferOp::Kind::ACCUMULATE, buffer,
     _ray->x, _ray->y, value * _ray->transmittance);

    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Accumulate2) {
    string buffer1(luaL_checkstring(_state, 1));
    string buffer2(luaL_checkstring(_state, 2));

    luaL_checktype(_state, 3, LUA_TTABLE);
    lua_pushvalue(_state, 3);
    vec2 value = FetchFloat2();
    lua_pop(_state, 1);

    _results->ops.emplace_back(BufferOp::Kind::ACCUMULATE, buffer1,
     _ray->x, _ray->y, value.x * _ray->transmittance);
    _results->ops.emplace_back(BufferOp::Kind::ACCUMULATE, buffer2,
     _ray->x, _ray->y, value.y * _ray->transmittance);

    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Accumulate3) {
    string buffer1(luaL_checkstring(_state, 1));
    string buffer2(luaL_checkstring(_state, 2));
    string buffer3(luaL_checkstring(_state, 3));

    luaL_checktype(_state, 4, LUA_TTABLE);
    lua_pushvalue(_state, 4);
    vec3 value = FetchFloat3();
    lua_pop(_state, 1);

    _results->ops.emplace_back(BufferOp::Kind::ACCUMULATE, buffer1,
     _ray->x, _ray->y, value.x * _ray->transmittance);
    _results->ops.emplace_back(BufferOp::Kind::ACCUMULATE, buffer2,
     _ray->x, _ray->y, value.y * _ray->transmittance);
    _results->ops.emplace_back(BufferOp::Kind::ACCUMULATE, buffer3,
     _ray->x, _ray->y, value.z * _ray->transmittance);

    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Accumulate4) {
    string buffer1(luaL_checkstring(_state, 1));
    string buffer2(luaL_checkstring(_state, 2));
    string buffer3(luaL_checkstring(_state, 3));
    string buffer4(luaL_checkstring(_state, 4));

    luaL_checktype(_state, 5, LUA_TTABLE);
    lua_pushvalue(_state, 5);
    vec4 value = FetchFloat4();
    lua_pop(_state, 1);

    _results->ops.emplace_back(BufferOp::Kind::ACCUMULATE, buffer1,
     _ray->x, _ray->y, value.x * _ray->transmittance);
    _results->ops.emplace_back(BufferOp::Kind::ACCUMULATE, buffer2,
     _ray->x, _ray->y, value.y * _ray->transmittance);
    _results->ops.emplace_back(BufferOp::Kind::ACCUMULATE, buffer3,
     _ray->x, _ray->y, value.z * _ray->transmittance);
    _results->ops.emplace_back(BufferOp::Kind::ACCUMULATE, buffer4,
     _ray->x, _ray->y, value.w * _ray->transmittance);

    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Write) {
    string buffer(luaL_checkstring(_state, 1));
    float value = static_cast<float>(luaL_checknumber(_state, 2));

    _results->ops.emplace_back(BufferOp::Kind::WRITE, buffer,
     _ray->x, _ray->y, value * _ray->transmittance);

    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Write2) {
    string buffer1(luaL_checkstring(_state, 1));
    string buffer2(luaL_checkstring(_state, 2));

    luaL_checktype(_state, 3, LUA_TTABLE);
    lua_pushvalue(_state, 3);
    vec2 value = FetchFloat2();
    lua_pop(_state, 1);

    _results->ops.emplace_back(BufferOp::Kind::WRITE, buffer1,
     _ray->x, _ray->y, value.x * _ray->transmittance);
    _results->ops.emplace_back(BufferOp::Kind::WRITE, buffer2,
     _ray->x, _ray->y, value.y * _ray->transmittance);

    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Write3) {
    string buffer1(luaL_checkstring(_state, 1));
    string buffer2(luaL_checkstring(_state, 2));
    string buffer3(luaL_checkstring(_state, 3));

    luaL_checktype(_state, 4, LUA_TTABLE);
    lua_pushvalue(_state, 4);
    vec3 value = FetchFloat3();
    lua_pop(_state, 1);

    _results->ops.emplace_back(BufferOp::Kind::WRITE, buffer1,
     _ray->x, _ray->y, value.x * _ray->transmittance);
    _results->ops.emplace_back(BufferOp::Kind::WRITE, buffer2,
     _ray->x, _ray->y, value.y * _ray->transmittance);
    _results->ops.emplace_back(BufferOp::Kind::WRITE, buffer3,
     _ray->x, _ray->y, value.z * _ray->transmittance);

    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Write4) {
    string buffer1(luaL_checkstring(_state, 1));
    string buffer2(luaL_checkstring(_state, 2));
    string buffer3(luaL_checkstring(_state, 3));
    string buffer4(luaL_checkstring(_state, 4));

    luaL_checktype(_state, 5, LUA_TTABLE);
    lua_pushvalue(_state, 5);
    vec4 value = FetchFloat4();
    lua_pop(_state, 1);

    _results->ops.emplace_back(BufferOp::Kind::WRITE, buffer1,
     _ray->x, _ray->y, value.x * _ray->transmittance);
    _results->ops.emplace_back(BufferOp::Kind::WRITE, buffer2,
     _ray->x, _ray->y, value.y * _ray->transmittance);
    _results->ops.emplace_back(BufferOp::Kind::WRITE, buffer3,
     _ray->x, _ray->y, value.z * _ray->transmittance);
    _results->ops.emplace_back(BufferOp::Kind::WRITE, buffer4,
     _ray->x, _ray->y, value.w * _ray->transmittance);

    return 0;
}

FR_SCRIPT_FUNCTION(ShaderScript, Texture) {
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
