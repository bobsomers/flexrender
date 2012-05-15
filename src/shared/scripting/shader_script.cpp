#include "scripting/shader_script.hpp"

#include <cstdlib>
#include <cassert>
#include <limits>
#include <stdio.h>
#include <errno.h>

#include "scripting/texture_script.hpp"
#include "types.hpp"
#include "utils.hpp"

using std::numeric_limits;
using std::string;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::normalize;

namespace fr {

ShaderScript::ShaderScript(const string& code, const Library* lib) :
 Script(),
 _lib(lib),
 _ray(nullptr),
 _results(nullptr),
 _has_direct(false),
 _has_indirect(false),
 _has_emissive(false),
 _has_vec2(false),
 _has_vec3(false),
 _has_vec4(false) {
     _hit.x = numeric_limits<float>::quiet_NaN();
     _hit.y = numeric_limits<float>::quiet_NaN();
     _hit.z = numeric_limits<float>::quiet_NaN();

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
    _has_indirect = lua_isfunction(_state, -1) != 0;
    lua_pop(_state, 1);

    lua_getglobal(_state, "emissive");
    _has_emissive = lua_isfunction(_state, -1) != 0;
    lua_pop(_state, 1);

    // Do they have global aliases for vector types? If so, we can set
    // vector metatables appropriately when we push arguments onto the stack.
    lua_getglobal(_state, "vec2");
    _has_vec2 = lua_istable(_state, -1) != 0;
    lua_pop(_state, 1);

    lua_getglobal(_state, "vec3");
    _has_vec3 = lua_istable(_state, -1) != 0;
    lua_pop(_state, 1);

    lua_getglobal(_state, "vec4");
    _has_vec4 = lua_istable(_state, -1) != 0;
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
    vec3 light = -ray->slim.direction;
    vec3 illumination = ray->emission;

    // Acquire the interpreter lock.
    if (sem_wait(&_lock) < 0) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }

    // Set the current data we're operating on.
    _ray = ray;
    _hit = hit;
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

void ShaderScript::Indirect(const FatRay* ray, vec3 hit, WorkResults* results) {
    if (!_has_indirect) return;

    Camera* cam = _lib->LookupCamera();

    // Compute the vectors we're passing to the shader.
    vec3 view = normalize(cam->eye - hit);
    vec3 normal = ray->strong.geom.n;
    vec2 texcoord = ray->strong.geom.t;

    // Acquire the interpreter lock.
    if (sem_wait(&_lock) < 0) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }

    // Set the current data we're operating on.
    _ray = ray;
    _hit = hit;
    _results = results;

    // Locate the function.
    lua_getglobal(_state, "indirect");

    // Push the arguments onto the stack.
    PushFloat3(view);
    PushFloat3(normal);
    PushFloat2(texcoord);

    // Set metatables for arguments if we can.
    if (_has_vec2) {
        lua_getglobal(_state, "vec2");
        lua_setmetatable(_state, -2); // texcoord (metatable is at -1)
    }
    if (_has_vec3) {
        lua_getglobal(_state, "vec3");
        lua_setmetatable(_state, -4); // view (metatable is at -1)
        lua_getglobal(_state, "vec3");
        lua_setmetatable(_state, -3); // normal (metatable is at -1)
    }

    // Call the function.
    CallFunc(3, 0);
    // No need to pop, 0 return values.

    // Release the interpreter lock.
    if (sem_post(&_lock) < 0) {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }
}

vec3 ShaderScript::Emissive(vec2 texcoord) {
    if (!_has_emissive) return vec3(0.0f, 0.0f, 0.0f);

    // Acquire the interpreter lock.
    if (sem_wait(&_lock) < 0) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }

    // Locate the function.
    lua_getglobal(_state, "emissive");

    // Push the arguments onto the stack.
    PushFloat2(texcoord);

    // Set metatables for arguments if we can.
    if (_has_vec2) {
        lua_getglobal(_state, "vec2");
        lua_setmetatable(_state, -2); // texcoord (metatable is at -1)
    }

    // Call the function.
    CallFunc(1, 1);
    luaL_checktype(_state, -1, LUA_TTABLE);
    vec3 value = FetchFloat3();
    lua_pop(_state, 1);

    // Release the interpreter lock.
    if (sem_post(&_lock) < 0) {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }

    return value;
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
    string sampler(luaL_checkstring(_state, 1));

    luaL_checktype(_state, 2, LUA_TTABLE);
    lua_pushvalue(_state, 2);
    vec2 texcoord = FetchFloat2();
    lua_pop(_state, 1);

    // Look up the material to get texture bindings.
    Mesh* mesh = _lib->LookupMesh(_ray->strong.mesh);
    assert(mesh != nullptr);
    Material* mat = _lib->LookupMaterial(mesh->material);
    assert(mat != nullptr);
    auto iter = mat->textures.find(sampler);
    if (iter == mat->textures.end()) {
        TERRLN("No texture bound with name '" << sampler << "'!");
        exit(EXIT_FAILURE);
    }
    Texture* tex = _lib->LookupTexture((*iter).second);
    assert(tex != nullptr);

    // Sample the texture.
    float value = tex->Sample(texcoord);

    // Return the sampled value.
    PushFloat(value);

    return 1;
}

FR_SCRIPT_FUNCTION(ShaderScript, Texture2) {
    string sampler1(luaL_checkstring(_state, 1));
    string sampler2(luaL_checkstring(_state, 2));

    luaL_checktype(_state, 3, LUA_TTABLE);
    lua_pushvalue(_state, 3);
    vec2 texcoord = FetchFloat2();
    lua_pop(_state, 1);

    // Look up the material to get texture bindings.
    Mesh* mesh = _lib->LookupMesh(_ray->strong.mesh);
    assert(mesh != nullptr);
    Material* mat = _lib->LookupMaterial(mesh->material);
    assert(mat != nullptr);

    auto iter1 = mat->textures.find(sampler1);
    if (iter1 == mat->textures.end()) {
        TERRLN("No texture bound with name '" << sampler1 << "'!");
        exit(EXIT_FAILURE);
    }
    Texture* tex1 = _lib->LookupTexture((*iter1).second);
    assert(tex1 != nullptr);

    auto iter2 = mat->textures.find(sampler2);
    if (iter2 == mat->textures.end()) {
        TERRLN("No texture bound with name '" << sampler2 << "'!");
        exit(EXIT_FAILURE);
    }
    Texture* tex2 = _lib->LookupTexture((*iter2).second);
    assert(tex2 != nullptr);

    // Sample the texture.
    vec2 value(tex1->Sample(texcoord),
               tex2->Sample(texcoord));

    // Return the sampled value.
    PushFloat2(value);
    if (_has_vec2) {
        lua_getglobal(_state, "vec2");
        lua_setmetatable(_state, -2); // value (metatable is at -1)
    }

    return 1;
}

FR_SCRIPT_FUNCTION(ShaderScript, Texture3) {
    string sampler1(luaL_checkstring(_state, 1));
    string sampler2(luaL_checkstring(_state, 2));
    string sampler3(luaL_checkstring(_state, 3));

    luaL_checktype(_state, 4, LUA_TTABLE);
    lua_pushvalue(_state, 4);
    vec2 texcoord = FetchFloat2();
    lua_pop(_state, 1);

    // Look up the material to get texture bindings.
    Mesh* mesh = _lib->LookupMesh(_ray->strong.mesh);
    assert(mesh != nullptr);
    Material* mat = _lib->LookupMaterial(mesh->material);
    assert(mat != nullptr);

    auto iter1 = mat->textures.find(sampler1);
    if (iter1 == mat->textures.end()) {
        TERRLN("No texture bound with name '" << sampler1 << "'!");
        exit(EXIT_FAILURE);
    }
    Texture* tex1 = _lib->LookupTexture((*iter1).second);
    assert(tex1 != nullptr);

    auto iter2 = mat->textures.find(sampler2);
    if (iter2 == mat->textures.end()) {
        TERRLN("No texture bound with name '" << sampler2 << "'!");
        exit(EXIT_FAILURE);
    }
    Texture* tex2 = _lib->LookupTexture((*iter2).second);
    assert(tex2 != nullptr);

    auto iter3 = mat->textures.find(sampler3);
    if (iter3 == mat->textures.end()) {
        TERRLN("No texture bound with name '" << sampler3 << "'!");
        exit(EXIT_FAILURE);
    }
    Texture* tex3 = _lib->LookupTexture((*iter3).second);
    assert(tex3 != nullptr);

    // Sample the texture.
    vec3 value(tex1->Sample(texcoord),
               tex2->Sample(texcoord),
               tex3->Sample(texcoord));

    // Return the sampled value.
    PushFloat3(value);
    if (_has_vec3) {
        lua_getglobal(_state, "vec3");
        lua_setmetatable(_state, -2); // value (metatable is at -1)
    }

    return 1;
}

FR_SCRIPT_FUNCTION(ShaderScript, Texture4) {
    string sampler1(luaL_checkstring(_state, 1));
    string sampler2(luaL_checkstring(_state, 2));
    string sampler3(luaL_checkstring(_state, 3));
    string sampler4(luaL_checkstring(_state, 4));

    luaL_checktype(_state, 5, LUA_TTABLE);
    lua_pushvalue(_state, 5);
    vec2 texcoord = FetchFloat2();
    lua_pop(_state, 1);

    // Look up the material to get texture bindings.
    Mesh* mesh = _lib->LookupMesh(_ray->strong.mesh);
    assert(mesh != nullptr);
    Material* mat = _lib->LookupMaterial(mesh->material);
    assert(mat != nullptr);

    auto iter1 = mat->textures.find(sampler1);
    if (iter1 == mat->textures.end()) {
        TERRLN("No texture bound with name '" << sampler1 << "'!");
        exit(EXIT_FAILURE);
    }
    Texture* tex1 = _lib->LookupTexture((*iter1).second);
    assert(tex1 != nullptr);

    auto iter2 = mat->textures.find(sampler2);
    if (iter2 == mat->textures.end()) {
        TERRLN("No texture bound with name '" << sampler2 << "'!");
        exit(EXIT_FAILURE);
    }
    Texture* tex2 = _lib->LookupTexture((*iter2).second);
    assert(tex2 != nullptr);

    auto iter3 = mat->textures.find(sampler3);
    if (iter3 == mat->textures.end()) {
        TERRLN("No texture bound with name '" << sampler3 << "'!");
        exit(EXIT_FAILURE);
    }
    Texture* tex3 = _lib->LookupTexture((*iter3).second);
    assert(tex3 != nullptr);

    auto iter4 = mat->textures.find(sampler4);
    if (iter4 == mat->textures.end()) {
        TERRLN("No texture bound with name '" << sampler4 << "'!");
        exit(EXIT_FAILURE);
    }
    Texture* tex4 = _lib->LookupTexture((*iter4).second);
    assert(tex4 != nullptr);

    // Sample the texture.
    vec4 value(tex1->Sample(texcoord),
               tex2->Sample(texcoord),
               tex3->Sample(texcoord),
               tex4->Sample(texcoord));

    // Return the sampled value.
    PushFloat4(value);
    if (_has_vec4) {
        lua_getglobal(_state, "vec4");
        lua_setmetatable(_state, -2); // value (metatable is at -1)
    }

    return 1;
}

FR_SCRIPT_FUNCTION(ShaderScript, Trace) {
    luaL_checktype(_state, 1, LUA_TTABLE);
    lua_pushvalue(_state, 1);
    vec3 direction = FetchFloat3();
    lua_pop(_state, 1);

    float partial = luaL_checknumber(_state, 2);

    int16_t bounce = _ray->bounces + 1;
    float transmittance = _ray->transmittance * partial;

    Config* config = _lib->LookupConfig();

    // Bounce limit and transmittance threshold rejection checks.
    if (bounce > config->bounce_limit ||
        transmittance < config->transmittance_threshold) {
        return 0;
    }

    // Create a new tracer ray that inherits many of its properties from the
    // source ray.
    FatRay* tracer = new FatRay(FatRay::Kind::INTERSECT, _ray->x, _ray->y);

    // The origin is at the intersection point, plus some epsilon along the
    // new direction to ensure no self intersection.
    tracer->slim.origin = _hit + direction * SELF_INTERSECT_EPSILON;
    tracer->slim.direction = direction;

    // Set the bounce number and transmittance.
    tracer->bounces = bounce;
    tracer->transmittance = transmittance;

    // It hasn't hit anything yet.
    tracer->weak.worker = 0;
    tracer->strong.worker = 0;

    _results->forwards.emplace_back(tracer, nullptr);

    return 0;
}

} // namespace fr
