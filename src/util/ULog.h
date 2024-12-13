#pragma once
// 2024.12.07

#include "UCommon.h"
#include <iostream>

namespace util {

  inline void log(std::string_view log_) {
    std::cout << log_ << "\n";
  }
  inline void logError(std::string_view error_) {
    std::cout << error_ << "\n";
  }

  template<typename T, typename...TARGS>
  void log(T first_, TARGS... other_) {
    std::cout << first_ << " ";
    if constexpr (sizeof...(other_)>0) {
      log(other_...);
      return;
    }
    std::cout << "\n";
  }
}