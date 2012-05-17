#pragma once

#include <cstdint>
#include <vector>

#include "glm/glm.hpp"
#include "msgpack.hpp"

#include "utils/tostring.hpp"

namespace fr {

struct Config {
    explicit Config();

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

    /// The number of samples per light (one triangle is one light).
    uint16_t samples;

    /// The maximum number of bounces before a ray is considered dead.
    int16_t bounce_limit;

    /// The threshold below which we consider the transmittance of a ray 0.
    float transmittance_threshold;

    /// The maximum amount (in percent, 0.0f -> 100.0f) that any worker is
    /// allowed to get ahead of the slowest worker in generating primary rays.
    float runaway;

    /// Name of the scene.
    std::string name;

    /// List of the workers involved.
    std::vector<std::string> workers;

    /// List of auxiliary render buffers (you get "R", "G", and "B" for free).
    std::vector<std::string> buffers;

    MSGPACK_DEFINE(width, height, min, max, antialiasing, samples, bounce_limit,
     transmittance_threshold, runaway, name, workers, buffers);

    TOSTRINGABLE(Config);
};

std::string ToString(const Config& config, const std::string& indent = "");

} // namespace fr
