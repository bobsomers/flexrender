#pragma once

#include <cstdint>

#include <vector>
#include <string>
#include <limits>

#include "glm/glm.hpp"
#include "msgpack.hpp"

namespace fr {

struct Config {
    explicit Config() :
     width(640),
     height(480),
     antialiasing(0),
     name("output"),
     workers(),
     buffers() {
        min.x = std::numeric_limits<float>::quiet_NaN();
        min.y = std::numeric_limits<float>::quiet_NaN();
        min.z = std::numeric_limits<float>::quiet_NaN();

        max.x = std::numeric_limits<float>::quiet_NaN();
        max.y = std::numeric_limits<float>::quiet_NaN();
        max.z = std::numeric_limits<float>::quiet_NaN();
    }

    /// Width of the rendered image.
    int16_t width;

    /// Height of the rendered image.
    int16_t height;

    /// The minimum bounds of the scene.
    glm::vec3 min;

    /// The maximum bounds of the scene.
    glm::vec3 max;

    /// The antialiasing grid size (for stratified supersampling).
    uint16_t antialiasing;

    /// Name of the scene.
    std::string name;

    /// List of the workers involved.
    std::vector<std::string> workers;

    /// List of auxiliary render buffers (you get "R", "G", and "B" for free).
    std::vector<std::string> buffers;

    MSGPACK_DEFINE(width, height, min, max, antialiasing, name, workers, buffers);
};

} // namespace fr
