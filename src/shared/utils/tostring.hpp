#pragma once

#include <string>

// TODO: Remove
#include <iostream>
#include <sstream>
// TODO: Remove

#define TOSTRINGABLE(type) friend std::string ToString(const type &, const std::string&);
