#include <cstdlib>
#include <cstdint>
#include <cassert>
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
    uint16_t port = 19400;
    {
        string port_str = FlagValue(argc, argv, "-p", "--port");
        if (port_str != "") {
            stringstream stream(port_str);
            stream >> port;
        }
    }

    TOUTLN("FlexWorker starting.");

    EngineInit("0.0.0.0", port);
    TOUTLN("Listening on port " << port << ".");

    EngineRun();

    TOUTLN("FlexWorker done.");
    return EXIT_SUCCESS;
}
