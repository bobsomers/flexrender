#pragma once

#include <string>
#include <iostream>
#include <sstream>

#define TOSTRINGABLE(type) friend std::string ToString(const type &, const std::string&);
