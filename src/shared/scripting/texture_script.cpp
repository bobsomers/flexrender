#include "scripting/texture_script.hpp"

#include <cstdlib>
#include <stdio.h>
#include <errno.h>

#include "types.hpp"
#include "utils.hpp"

using std::string;
using glm::vec2;
using glm::vec3;

namespace fr {

TextureScript::TextureScript(const string& code) :
 Script(),
 _has_vec2(false) {
    // TODO: Shader scripts shouldn't have access to the whole standard
    // library...
    FR_SCRIPT_INIT(TextureScript, ScriptLibs::STANDARD_LIBS);

    // Evaluate the shader.
    if (luaL_dostring(_state, code.c_str())) {
        TERRLN(lua_tostring(_state, -1));
        exit(EXIT_FAILURE);
    }

    // Make sure the texture function exists.
    lua_getglobal(_state, "texture");
    if (lua_isfunction(_state, -1) == 0) {
        TERRLN("Procedural textures must define a texture() function!");
        exit(EXIT_FAILURE);
    }
    lua_pop(_state, 1);

    // Do they have local aliases for vec2's? If so, we can set vectors
    // metatables appropriately when we push arguments onto the stack.
    lua_getglobal(_state, "vec2");
    _has_vec2 = lua_istable(_state, -1) != 0;
    lua_pop(_state, 1);

    // Initialize lock.
    if (sem_init(&_lock, 0, 1) < 0) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }
}

vec3 TextureScript::Evaluate(vec2 texcoord) {
    // Acquire the interpreter lock.
    if (sem_wait(&_lock) < 0) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }

    // Locate the function.
    lua_getglobal(_state, "texture");

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

} // namespace fr
