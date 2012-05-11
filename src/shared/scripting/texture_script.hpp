#pragma once

#include <semaphore.h>

#include "glm/glm.hpp"

#include "scripting/script.hpp"

namespace fr {

class TextureScript : public Script {
public:
    explicit TextureScript(const std::string& code);

    glm::vec3 Evaluate(glm::vec2 texcoord);

private:
    bool _has_vec2;
    sem_t _lock;
};

} // namespace fr
