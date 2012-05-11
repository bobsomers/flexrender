#pragma once

#include <semaphore.h>

#include "glm/glm.hpp"

#include "scripting/script.hpp"

namespace fr {

class Library;
struct FatRay;
struct WorkResults;

class ShaderScript : public Script {
public:
    explicit ShaderScript(const std::string& code, const Library *lib);

    /**
     * Runs the direct lighting function of the shader (if it exists) with the
     * given arguments, potentially appending buffer writes to the work
     * results.
     */
    void Direct(const FatRay* ray, glm::vec3 hit, WorkResults* results);

    /**
     * Runs the indirect lighting function of the shader (if it exists) with
     * the given arguments, potentially appending buffer writes to the work
     * results.
     */
    void Indirect(glm::vec3 view, glm::vec3 normal, glm::vec2 texcoord,
     WorkResults* results);

    /**
     * Runs the emissive lighting function of the shader (if it exists) with
     * the given arguments, potentially appending buffer writes to the work
     * results.
     */
    void Emissive(glm::vec3 view, glm::vec3 normal, glm::vec2 texcoord,
     WorkResults *results);

    // Shader built-ins.
    FR_SCRIPT_DECLARE(Accumulate);
    FR_SCRIPT_DECLARE(Accumulate2);
    FR_SCRIPT_DECLARE(Accumulate3);
    FR_SCRIPT_DECLARE(Accumulate4);
    FR_SCRIPT_DECLARE(Write);
    FR_SCRIPT_DECLARE(Write2);
    FR_SCRIPT_DECLARE(Write3);
    FR_SCRIPT_DECLARE(Write4);
    FR_SCRIPT_DECLARE(Texture);
    FR_SCRIPT_DECLARE(Texture2);
    FR_SCRIPT_DECLARE(Texture3);
    FR_SCRIPT_DECLARE(Texture4);
    FR_SCRIPT_DECLARE(Trace);

private:
    const Library* _lib;
    const FatRay* _ray;
    WorkResults* _results;
    bool _has_direct;
    bool _has_indirect;
    bool _has_emissive;
    bool _has_vec2;
    bool _has_vec3;
    sem_t _lock;
};

} // namespace fr
