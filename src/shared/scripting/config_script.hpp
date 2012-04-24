#pragma once

#include <string>

#include "scripting/script.hpp"

namespace fr {

struct Config;

class ConfigScript : public Script {
public:
    explicit ConfigScript();

    /**
     * Parses the passed file as a config script and sets the passed config
     * object. If it succeeds, it returns true. If it fails, it returns
     * false.
     */
    bool Parse(const std::string& filename, Config *config);

    // Script function handlers.
    FR_SCRIPT_DECLARE(Network);
    FR_SCRIPT_DECLARE(Output);
    FR_SCRIPT_DECLARE(Render);

private:
    Config* _config;
};

} // namespace fr
