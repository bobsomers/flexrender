#pragma once

#include <string>
#include <cstdint>

namespace fr {

void EngineInit(const std::string& config_file, const std::string& scene_file,
 uint32_t intervals, uint32_t jobs);

void EngineRun();

} // namespace fr
