#pragma once

#include <string>
#include <cstdint>

namespace fr {

void EngineInit(const std::string& config_file, const std::string& scene_file,
 uint32_t intervals, uint32_t jobs, int16_t offset, uint16_t chunk_size);

void EngineRun();

} // namespace fr
