#pragma once

#include <string>
#include <cstdlib>

#include "uv.h"

#include "utils/tout.hpp"

namespace fr {

inline void CheckUVResult(int result, const std::string& message) {
    if (result != 0) {
        uv_err_t err = uv_last_error(uv_default_loop());
        TERRLN(message << ": " << uv_strerror(err));
        exit(EXIT_FAILURE);
    }
}

} // namespace fr
