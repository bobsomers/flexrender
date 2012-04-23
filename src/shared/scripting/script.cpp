#include "scripting/script.hpp"

#include "utils/tout.hpp"

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

} // namespace fr
