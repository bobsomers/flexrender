#include "types/camera.hpp"

#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <iostream>
#include <sstream>
#include <limits>

#include "types/config.hpp"
#include "types/ray.hpp"
#include "utils/printers.hpp"

using std::numeric_limits;
using std::string;
using std::stringstream;
using std::endl;
using glm::vec3;
using glm::normalize;
using glm::cross;

namespace fr {

Camera::Camera(const Config* config) :
 up(0.0f, 1.0f, 0.0f),
 rotation(0.0f),
 ratio(static_cast<float>(config->width) / static_cast<float>(config->height)),
 _config(config),
 _x(0),
 _y(0),
 _i(0),
 _j(0),
 _initialized(false) {
    eye.x = numeric_limits<float>::quiet_NaN();
    eye.y = numeric_limits<float>::quiet_NaN();
    eye.z = numeric_limits<float>::quiet_NaN();

    look.x = numeric_limits<float>::quiet_NaN();
    look.y = numeric_limits<float>::quiet_NaN();
    look.z = numeric_limits<float>::quiet_NaN();

    _l = numeric_limits<float>::quiet_NaN();
    _b = numeric_limits<float>::quiet_NaN();

    _u.x = numeric_limits<float>::quiet_NaN();
    _u.y = numeric_limits<float>::quiet_NaN();
    _u.z = numeric_limits<float>::quiet_NaN();

    _v.x = numeric_limits<float>::quiet_NaN();
    _v.y = numeric_limits<float>::quiet_NaN();
    _v.z = numeric_limits<float>::quiet_NaN();

    _w.x = numeric_limits<float>::quiet_NaN();
    _w.y = numeric_limits<float>::quiet_NaN();
    _w.z = numeric_limits<float>::quiet_NaN();
}

Camera::Camera() :
 up(0.0f, 1.0f, 0.0f),
 rotation(0.0f),
 ratio(4.0f / 3.0f),
 _config(nullptr),
 _x(0),
 _y(0),
 _i(0),
 _j(0),
 _initialized(false) {
    eye.x = numeric_limits<float>::quiet_NaN();
    eye.y = numeric_limits<float>::quiet_NaN();
    eye.z = numeric_limits<float>::quiet_NaN();

    look.x = numeric_limits<float>::quiet_NaN();
    look.y = numeric_limits<float>::quiet_NaN();
    look.z = numeric_limits<float>::quiet_NaN();

    _l = numeric_limits<float>::quiet_NaN();
    _b = numeric_limits<float>::quiet_NaN();

    _u.x = numeric_limits<float>::quiet_NaN();
    _u.y = numeric_limits<float>::quiet_NaN();
    _u.z = numeric_limits<float>::quiet_NaN();

    _v.x = numeric_limits<float>::quiet_NaN();
    _v.y = numeric_limits<float>::quiet_NaN();
    _v.z = numeric_limits<float>::quiet_NaN();

    _w.x = numeric_limits<float>::quiet_NaN();
    _w.y = numeric_limits<float>::quiet_NaN();
    _w.z = numeric_limits<float>::quiet_NaN();
}

bool Camera::GeneratePrimary(Ray* ray) {
    assert(_config != nullptr);

    if (!_initialized) {
        // Compute the bottom left screen space extents.
        _l = ratio / -2.0f;
        _b = -0.5f;

        // Compute the camera gaze vector.
        _w = normalize(look - eye);

        // Compute the camera up vector (not factoring in rotation yet).
        vec3 temp = normalize(cross(_w, up));
        _v = normalize(cross(temp, _w));

        // Find the point on the tip of the up vector.
        vec3 v_pt = eye + _v;

        // Rotate that point around the gaze vector.
        // Credit: http://www.blitzbasic.com/Community/posts.php?topic=57616#645017
        vec3 rotated_v_pt;
        rotated_v_pt.x = _w.x * (_w.x * v_pt.x + _w.y * v_pt.y + _w.z * v_pt.z) + (v_pt.x * (_w.y * _w.y + _w.z * _w.z) - _w.x * (_w.y * v_pt.y + _w.z * v_pt.z)) * cosf(rotation * (float)M_PI / 180.0f) + (-_w.z * v_pt.y + _w.y * v_pt.z) * sinf(rotation * (float)M_PI / 180.0f);
        rotated_v_pt.y = _w.y * (_w.x * v_pt.x + _w.y * v_pt.y + _w.z * v_pt.z) + (v_pt.y * (_w.x * _w.x + _w.z * _w.z) - _w.y * (_w.x * v_pt.x + _w.z * v_pt.z)) * cosf(rotation * (float)M_PI / 180.0f) + (_w.z * v_pt.x - _w.x * v_pt.z) * sinf(rotation * (float)M_PI / 180.0f);
        rotated_v_pt.z = _w.z * (_w.x * v_pt.x + _w.y * v_pt.y + _w.z * v_pt.z) + (v_pt.z * (_w.x * _w.x + _w.y * _w.y) - _w.z * (_w.x * v_pt.x + _w.y * v_pt.y)) * cosf(rotation * (float)M_PI / 180.0f) + (-_w.y * v_pt.x + _w.x * v_pt.y) * sinf(rotation * (float)M_PI / 180.0f);

        // Recalcualte the rotated up vector by subtracting off the eye position.
        _v = normalize(rotated_v_pt - eye);

        // Compute the camera u vector.
        _u = normalize(cross(_w, _v));

        _initialized = true;
    }

    // Termination condition.
    if (_x >= _config->width) {
        ray = nullptr;
        return false;
    }

    float us = 0.0f;
    float vs = 0.0f;
    float ws = 0.0f;
    float transmittance = 0.0f;

    // Calculate screen space <u, v, w>.
    if (_config->antialiasing <= 1) {
        // No antialiasing.
        us = _l + (ratio * (_x + 0.5f) / _config->width);
        vs = _b + (1.0f * (_y + 0.5f) / _config->height);
        ws = 1.0f;
        transmittance = 1.0f;
    } else {
        // Stratified supersampling on an AxA grid.
        float cell_size = 1.0f / _config->antialiasing;
        float rand_offset = static_cast<float>(rand()) / RAND_MAX;
        us = _l + (ratio * (_x + (_i * cell_size) + (rand_offset * cell_size)) / _config->width);
        rand_offset = static_cast<float>(rand()) / RAND_MAX;
        vs = _b + (1.0f * (_y + (_j * cell_size) + (rand_offset * cell_size)) / _config->height);
        ws = 1.0f;
        transmittance = 1.0f / (_config->antialiasing * _config->antialiasing);
    }

    // Convert screen space coordinates into world coordinates.
    vec3 screen_pt = eye +
                     (_u * vec3(us, us, us)) +
                     (_v * vec3(vs, vs, vs)) +
                     (_w * vec3(ws, ws, ws));

    // Fill in the ray.
    ray->kind = Ray::Kind::INTERSECT;
    ray->x = _x;
    ray->y = _y;
    ray->bounces = 0;
    ray->origin = eye;
    ray->direction = normalize(screen_pt - eye);
    ray->transmittance = transmittance;
    ray->weak.worker = 0;
    ray->strong.worker = 0;

    // Advance our internal counters.
    _j++;
    if (_j >= _config->antialiasing) {
        _j = 0;
        _i++;
        if (_i >= _config->antialiasing) {
            _i = 0;
            _y++;
            if (_y >= _config->height) {
                _y = 0;
                _x++;
            }
        }
    }

    return true;
}

string ToString(const Camera& camera, const string& indent) {
    stringstream stream;
    stream << "Camera {" << endl <<
     indent << "| eye = " << ToString(camera.eye) << endl <<
     indent << "| look = " << ToString(camera.look) << endl <<
     indent << "| up = " << ToString(camera.up) << endl <<
     indent << "| rotation = " << camera.rotation << endl <<
     indent << "| ratio = " << camera.ratio << endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
