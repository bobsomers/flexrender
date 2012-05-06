#pragma once

#include <string>

#define TOSTRINGABLE(type) friend std::string ToString(const type &, const std::string&);
