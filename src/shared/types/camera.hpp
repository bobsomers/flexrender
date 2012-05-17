#pragma once

#include "glm/glm.hpp"
#include "msgpack.hpp"

#include "utils/tostring.hpp"

namespace fr {

struct Config;
struct FatRay;

struct Camera {
    explicit Camera(const Config* config);

    // FOR MSGPACK ONLY!
    explicit Camera();

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

    inline void SetConfig(const Config* config) { _config = config; }

    inline void SetRange(int16_t offset, uint16_t chunk_size) {
        _offset = offset;
        _chunk_size = chunk_size;
        _x = offset;
        _end = offset + chunk_size;
    }

    /**
     * Generates a single primary ray based on the passed config and the
     * camera settings. Call this successively to keep generating primary rays.
     * Once all rays have been generated, returns false and the contents of ray
     * are undefined.
     */
    bool GeneratePrimary(FatRay* ray);

    inline float Progress() const { return _progress; }

    MSGPACK_DEFINE(eye, look, up, rotation, ratio);

    TOSTRINGABLE(Camera);

private:
    const Config* _config;
    int16_t _x;
    int16_t _y;
    uint16_t _i;
    uint16_t _j;
    int16_t _end;
    int16_t _offset;
    uint16_t _chunk_size;
    float _l;
    float _t;
    glm::vec3 _u, _v, _w;
    bool _initialized;
    float _progress;
};

std::string ToString(const Camera& camera, const std::string& indent = "");

} // namespace fr
