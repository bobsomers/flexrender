#include "scripting/config_script.hpp"

#include <cstdint>

#include "types/config.hpp"
#include "utils/library.hpp"
#include "utils/tout.hpp"

using glm::vec2;
using glm::vec3;

namespace fr {

using std::string;

ConfigScript::ConfigScript() :
 Script(),
 _config(nullptr) {
    // Config files should have access to the entire standard library.
    FR_SCRIPT_INIT(ConfigScript, ScriptLibs::STANDARD_LIBS);

    // Register function handlers with the interpreter.
    FR_SCRIPT_REGISTER("network", ConfigScript, Network);
    FR_SCRIPT_REGISTER("output", ConfigScript, Output);
    FR_SCRIPT_REGISTER("render", ConfigScript, Render);
}

bool ConfigScript::Parse(const string& filename, Library *lib) {
    _config = new Config;

    // Evaluate the file.
    if (luaL_dofile(_state, filename.c_str())) {
        TERRLN(lua_tostring(_state, -1));
        return false;
    }

    lib->StoreConfig(_config);
    _config = nullptr;

    return true;
}

FR_SCRIPT_FUNCTION(ConfigScript, Network) {
    BeginTableCall();

    // "workers" is a required array of strings.
    if (!PushField("workers", LUA_TTABLE)) {
        ScriptError("network.workers is required");
    }
    ForEachIndex([this](size_t index) {
        PushIndex(index, LUA_TSTRING);
        _config->workers.push_back(FetchString());
        PopIndex();
    });
    PopField();

    EndTableCall();
    return 0;
}

FR_SCRIPT_FUNCTION(ConfigScript, Output) {
    BeginTableCall();

    // "name" is an optional string.
    if (PushField("name", LUA_TSTRING)) {
        _config->name = FetchString();
    }
    PopField();

    // "size" is an optional vec2 of int16.
    if (PushField("size", LUA_TTABLE)) {
        vec2 size = FetchFloat2();
        _config->width = static_cast<int16_t>(size.x);
        _config->height = static_cast<int16_t>(size.y);
    }
    PopField();

    // "buffers" is an optional array of strings
    if (PushField("buffers", LUA_TTABLE)) {
        ForEachIndex([this](size_t index) {
            PushIndex(index, LUA_TSTRING);
            _config->buffers.push_back(FetchString());
            PopIndex();
        });
    }
    PopField();

    EndTableCall();
    return 0;
}

FR_SCRIPT_FUNCTION(ConfigScript, Render) {
    BeginTableCall();

    // "antialiasing" is an optional uint16
    if (PushField("antialiasing", LUA_TNUMBER)) {
        _config->antialiasing = static_cast<uint16_t>(FetchFloat());
    }
    PopField();

    // "samples" is an optional uint16
    if (PushField("samples", LUA_TNUMBER)) {
        _config->samples = static_cast<uint16_t>(FetchFloat());
    }
    PopField();

    // "bounces" is an optional int16
    if (PushField("bounces", LUA_TNUMBER)) {
        _config->bounce_limit = static_cast<int16_t>(FetchFloat());
    }
    PopField();

    // "threshold" is an optional float
    if (PushField("threshold", LUA_TNUMBER)) {
        _config->transmittance_threshold = FetchFloat();
    }
    PopField();

    // "min" is a required float3
    if (!PushField("min", LUA_TTABLE)) {
        ScriptError("render.min is required");
    }
    _config->min = FetchFloat3();
    PopField();

    // "max" is a required float3
    if (!PushField("max", LUA_TTABLE)) {
        ScriptError("render.max is required");
    }
    _config->max = FetchFloat3();
    PopField();

    EndTableCall();
    return 0;
}

} // namespace fr
