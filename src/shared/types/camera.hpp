#pragma once

#include <limits>

#include "glm/glm.hpp"
#include "msgpack.hpp"

#include "types/config.hpp"
#include "utils/tostring.hpp"
#include "utils/printers.hpp"

namespace fr {

struct Ray;

struct Camera {
    explicit Camera(const Config* config) :
     up(0.0f, 1.0f, 0.0f),
     rotation(0.0f),
     ratio(static_cast<float>(_config->width) / static_cast<float>(_config->height)),
     _config(config) {
        eye.x = std::numeric_limits<float>::quiet_NaN();
        eye.y = std::numeric_limits<float>::quiet_NaN();
        eye.z = std::numeric_limits<float>::quiet_NaN();

        look.x = std::numeric_limits<float>::quiet_NaN();
        look.y = std::numeric_limits<float>::quiet_NaN();
        look.z = std::numeric_limits<float>::quiet_NaN();
    }

    // FOR MSGPACK ONLY!
    explicit Camera() :
     up(0.0f, 1.0f, 0.0f),
     rotation(0.0f),
     ratio(4.0f / 3.0f),
     _config(nullptr) {
        eye.x = std::numeric_limits<float>::quiet_NaN();
        eye.y = std::numeric_limits<float>::quiet_NaN();
        eye.z = std::numeric_limits<float>::quiet_NaN();

        look.x = std::numeric_limits<float>::quiet_NaN();
        look.y = std::numeric_limits<float>::quiet_NaN();
        look.z = std::numeric_limits<float>::quiet_NaN();
    }

    /// The eye position.
    glm::vec3 eye;

    /// The "look at" position.
    glm::vec3 look;

    /// The world "up" vector.
    glm::vec3 up;

    /// The amount of rotation (counter-clockwise) about the gaze vector (in
    /// radians).
    float rotation;

    /// The aspect ratio of the camera.
    float ratio;

    MSGPACK_DEFINE(eye, look, up, rotation, ratio);

    TOSTRINGABLE(Camera);

    inline void SetConfig(const Config* config) { _config = config; }

    /**
     * Generates a single primary ray based on the passed config and the
     * camera settings. Call this successively to keep generating primary rays.
     * Once all rays have been generated, returns false and the contents of ray
     * are undefined.
     */
    bool GeneratePrimary(Ray &ray);

private:
    const Config* _config;
};

inline std::string ToString(const Camera& camera, const std::string& indent = "") {
    std::stringstream stream;
    stream << "Camera {" << std::endl <<
     indent << "| eye = " << ToString(camera.eye) << std::endl <<
     indent << "| look = " << ToString(camera.look) << std::endl <<
     indent << "| up = " << ToString(camera.up) << std::endl <<
     indent << "| rotation = " << camera.rotation << std::endl <<
     indent << "| ratio = " << camera.ratio << std::endl <<
     indent << "}";
    return stream.str();
}

} // namespace fr
