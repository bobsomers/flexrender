#include "types/camera.hpp"

#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <cassert>

#include "types/config.hpp"
#include "types/ray.hpp"

using std::numeric_limits;
using glm::vec3;
using glm::normalize;
using glm::cross;

namespace fr {

bool Camera::GeneratePrimary(Ray &ray) {
    static int16_t x = 0;
    static int16_t y = 0;
    static uint16_t i = 0;
    static uint16_t j = 0;
    static float l = numeric_limits<float>::quiet_NaN();
    static float b = numeric_limits<float>::quiet_NaN();
    static vec3 u, v, w;
    static bool initialized = false;

    assert(_config != nullptr);

    if (!initialized) {
        // Compute the bottom left screen space extents.
        l = ratio / -2.0f;
        b = -0.5f;

        // Compute the camera gaze vector.
        w = normalize(look - eye);

        // Compute the camera up vector (not factoring in rotation yet).
        vec3 temp = normalize(cross(w, up));
        v = normalize(cross(temp, w));

        // Find the point on the tip of the up vector.
        vec3 v_pt = eye + v;

        // Rotate that point around the gaze vector.
        // Credit: http://www.blitzbasic.com/Community/posts.php?topic=57616#645017
        vec3 rotated_v_pt;
        rotated_v_pt.x = w.x * (w.x * v_pt.x + w.y * v_pt.y + w.z * v_pt.z) + (v_pt.x * (w.y * w.y + w.z * w.z) - w.x * (w.y * v_pt.y + w.z * v_pt.z)) * cosf(rotation * (float)M_PI / 180.0f) + (-w.z * v_pt.y + w.y * v_pt.z) * sinf(rotation * (float)M_PI / 180.0f);
        rotated_v_pt.y = w.y * (w.x * v_pt.x + w.y * v_pt.y + w.z * v_pt.z) + (v_pt.y * (w.x * w.x + w.z * w.z) - w.y * (w.x * v_pt.x + w.z * v_pt.z)) * cosf(rotation * (float)M_PI / 180.0f) + (w.z * v_pt.x - w.x * v_pt.z) * sinf(rotation * (float)M_PI / 180.0f);
        rotated_v_pt.z = w.z * (w.x * v_pt.x + w.y * v_pt.y + w.z * v_pt.z) + (v_pt.z * (w.x * w.x + w.y * w.y) - w.z * (w.x * v_pt.x + w.y * v_pt.y)) * cosf(rotation * (float)M_PI / 180.0f) + (-w.y * v_pt.x + w.x * v_pt.y) * sinf(rotation * (float)M_PI / 180.0f);

        // Recalcualte the rotated up vector by subtracting off the eye position.
        v = normalize(rotated_v_pt - eye);

        // Compute the camera u vector.
        u = normalize(cross(w, v));

        initialized = true;
    }

    // Termination condition.
    if (x >= _config->width) {
        return false;
    }

    float us = 0.0f;
    float vs = 0.0f;
    float ws = 0.0f;
    float transmittance = 0.0f;

    // Calculate screen space <u, v, w>.
    if (_config->antialiasing <= 1) {
        // No antialiasing.
        us = l + (ratio * (x + 0.5f) / _config->width);
        vs = b + (1.0f * (y + 0.5f) / _config->height);
        ws = 1.0f;
        transmittance = 1.0f;
    } else {
        // Stratified supersampling on an AxA grid.
        float cell_size = 1.0f / _config->antialiasing;
        float rand_offset = static_cast<float>(rand()) / RAND_MAX;
        us = l + (ratio * (x + (i * cell_size) + (rand_offset * cell_size)) / _config->width);
        rand_offset = static_cast<float>(rand()) / RAND_MAX;
        vs = b + (1.0f * (y + (j * cell_size) + (rand_offset * cell_size)) / _config->height);
        ws = 1.0f;
        transmittance = 1.0f / (_config->antialiasing * _config->antialiasing);
    }

    // Convert screen space coordinates into world coordinates.
    vec3 screen_pt = eye +
                     (u * vec3(us, us, us)) +
                     (v * vec3(vs, vs, vs)) +
                     (w * vec3(ws, ws, ws));

    // Fill in the ray.
    ray.kind = Ray::Kind::INTERSECT;
    ray.x = x;
    ray.y = y;
    ray.bounces = 0;
    ray.origin = eye;
    ray.direction = normalize(screen_pt - eye);
    ray.transmittance = transmittance;
    ray.weak.worker = 0;
    ray.strong.worker = 0;

    // Advance our internal counters.
    j++;
    if (j >= _config->antialiasing) {
        j = 0;
        i++;
        if (i >= _config->antialiasing) {
            i = 0;
            y++;
            if (y >= _config->height) {
                y = 0;
                x++;
            }
        }
    }

    return true;
}

} // namespace fr
