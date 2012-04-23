#pragma once

#include <iostream>
#include <ctime>

#define TOUT(str)                                     \
    {                                                 \
        std::time_t _FR_time_ = std::time(NULL);      \
        char _FR_time_buf_[16];                       \
        std::strftime(_FR_time_buf_, 16, "%H:%M:%S ", \
         std::localtime(&_FR_time_));                 \
        std::cout << _FR_time_buf_ << str;            \
    }

#define TOUTLN(str) TOUT(str << std::endl)

#define TERR(str)                                     \
    {                                                 \
        std::time_t _FR_time_ = std::time(NULL);      \
        char _FR_time_buf_[16];                       \
        std::strftime(_FR_time_buf_, 16, "%H:%M:%S ", \
         std::localtime(&_FR_time_));                 \
        std::cerr << _FR_time_buf_ << str;            \
    }

#define TERRLN(str) TERR(str << std::endl)
