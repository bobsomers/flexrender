#include "types/config.hpp"

#include <limits>
#include <iostream>
#include <sstream>

#include "utils/printers.hpp"

using std::numeric_limits;
using std::string;
using std::stringstream;
using std::endl;

namespace fr {

Config::Config() :
 width(640),
 height(480),
 antialiasing(0),
 samples(10),
 bounce_limit(5),
 transmittance_threshold(0.0f),
 name("output"),
 workers(),
 buffers() {
    min.x = numeric_limits<float>::quiet_NaN();
    min.y = numeric_limits<float>::quiet_NaN();
    min.z = numeric_limits<float>::quiet_NaN();

    max.x = numeric_limits<float>::quiet_NaN();
    max.y = numeric_limits<float>::quiet_NaN();
    max.z = numeric_limits<float>::quiet_NaN();
}

string ToString(const Config& config, const string& indent) {
    stringstream stream;
    string pad = indent + "| | ";
    stream << "Config {" << endl <<
     indent << "| width = " << config.width << endl <<
     indent << "| height = " << config.height << endl <<
     indent << "| min = " << ToString(config.min) << endl <<
     indent << "| max = " << ToString(config.max) << endl <<
     indent << "| antialiasing = " << config.antialiasing << endl <<
     indent << "| samples = " << config.samples << endl <<
     indent << "| bounce_limit = " << config.bounce_limit << endl <<
     indent << "| transmittance_threshold = " << config.transmittance_threshold << endl <<
     indent << "| name = " << config.name << endl <<
     indent << "| workers = {" << endl;
    for (const auto& worker : config.workers) {
        stream << pad << worker << endl;
    }
    stream << indent << "| }" << endl <<
     indent << "| buffers = {" << endl;
    for (const auto& buffer : config.buffers) {
        stream << pad << buffer << endl;
    }
    stream << indent << "| }" << endl <<
     indent << "}" << endl;
    return stream.str();
}

} // namespace fr
