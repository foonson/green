//
//  UTime.cpp
//  green
//
//  Created by Steve Cheng on 24/6/2021.
//

#include "UTime.h"

namespace util::UTime {
  std::chrono::time_point<std::chrono::system_clock> now() {
    return std::chrono::system_clock::now();
  }
}

namespace util {
  void sleep(uint16_t milli_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milli_));
  }
}
