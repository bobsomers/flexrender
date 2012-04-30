#pragma once

#include <string>

#include "scripting/script.hpp"

namespace fr {

class Library;
struct Mesh;

class SceneScript : public Script {
public:
    explicit SceneScript();

    /**
     * Parses the given scene script and all of its resources (including
     * shaders, textures, mesh data, etc.) out to a library. Returns
     * true if it succeeded, false if it failed.
     */
    bool Parse(const std::string& filename, Library *lib);

    // Script function handlers.
    FR_SCRIPT_DECLARE(Camera);
    FR_SCRIPT_DECLARE(Texture);
    FR_SCRIPT_DECLARE(Shader);
    FR_SCRIPT_DECLARE(Material);
    FR_SCRIPT_DECLARE(Mesh);
    FR_SCRIPT_DECLARE(Triangle);

private:
    Library* _lib;
    Mesh *_active_mesh;
    glm::vec3 _centroid_num;
    float _centroid_denom;
};

} // namespace fr
