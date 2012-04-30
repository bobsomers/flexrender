#pragma once

#include <cstdint>
#include <string>

namespace fr {

void EngineInit(const std::string& ip, uint16_t port);

void EngineRun();

} // namespace fr
