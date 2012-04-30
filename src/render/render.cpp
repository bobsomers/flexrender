#include <cstdlib>
#include <string>
#include <iostream>

#include "engine.hpp"
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

    EngineInit(config_file, scene_file);
    EngineRun();

    TOUTLN("FlexRender done.");
    return EXIT_SUCCESS;
}
