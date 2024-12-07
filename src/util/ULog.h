#pragma once
// 2024.12.07

#include "UCommon.h"
#include <iostream>

namespace util {

    void log(std::string_view log_) {
        std::cout << log_ << "\n";
    }
    void logError(std::string_view error_) {
        std::cout << error_ << "\n";
    }
}