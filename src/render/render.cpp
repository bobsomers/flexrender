#include <cstdlib>
#include <string>
#include <iostream>

#include "scripting.hpp"
#include "types.hpp"
#include "utils.hpp"

using std::string;
using std::cerr;
using std::endl;

using namespace fr;

int main(int argc, char *argv[]) {
    // Grab relevant command line arguments.
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <config.lua> <scene.lua>" << endl;
        exit(EXIT_FAILURE);
    }
    string config_file = ArgumentValue(argc, argv, 1);
    string scene_file = ArgumentValue(argc, argv, 2);

    TOUTLN("FlexRender starting.");

    Library* lib = new Library;

    // Parse the config file.
    ConfigScript config_script;
    TOUTLN("Loading config from " << config_file << ".");
    if (!config_script.Parse(config_file, lib)) {
        TERRLN("Can't continue with bad config.");
        exit(EXIT_FAILURE);
    }
    TOUTLN("Config loaded.");

    // Parse and distribute the scene.
    SceneScript scene_script;
    TOUTLN("Loading scene from " << scene_file << ".");
    if (!scene_script.Parse(scene_file, lib)) {
        TERRLN("Can't continue with bad scene.");
        exit(EXIT_FAILURE);
    }
    TOUTLN("Scene loaded.");

    TOUTLN(ToString(*lib->LookupShader(1)));
    TOUTLN(ToString(*lib->LookupTexture(1)));
    TOUTLN(ToString(*lib->LookupTexture(2)));
    TOUTLN(ToString(*lib->LookupMaterial(1)));
    TOUTLN(ToString(*lib->LookupMesh(1)));

    delete lib;

    TOUTLN("FlexRender done.");
    return EXIT_SUCCESS;
}
