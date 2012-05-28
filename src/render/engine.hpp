#pragma once

#include <string>
#include <cstdint>

namespace fr {

void EngineInit(const std::string& config_file, const std::string& scene_file,
 uint32_t intervals, bool linear_scan);

void EngineRun();

} // namespace fr
