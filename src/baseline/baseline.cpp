#include <cstdlib>
#include <cstdint>
#include <string>
#include <iostream>
#include <sstream>

#include "engine.hpp"
#include "utils.hpp"

using std::string;
using std::stringstream;
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

    uint32_t intervals = 10;
    {
        string intervals_str = FlagValue(argc, argv, "-i", "--intervals");
        if (intervals_str != "") {
            stringstream stream(intervals_str);
            stream >> intervals;
        }
    }

    uint32_t jobs = 10;
    {
        string jobs_str = FlagValue(argc, argv, "-j", "--jobs");
        if (jobs_str != "") {
            stringstream stream(jobs_str);
            stream >> jobs;
        }
    }

    TOUTLN("Baseline starting.");

    EngineInit(config_file, scene_file, intervals, jobs);
    EngineRun();

    TOUTLN("Baseline done.");
    return EXIT_SUCCESS;
}
