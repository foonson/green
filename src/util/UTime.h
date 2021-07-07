//
//  UTime.hpp
//  green
//
//  Created by Steve Cheng on 24/6/2021.
//

#ifndef UTime_hpp
#define UTime_hpp

#include <chrono>
#include <thread>
namespace util::UTime {

  std::chrono::time_point<std::chrono::system_clock> now();

  template <typename TimePt>
  double diffMilli(TimePt t1, TimePt t2) {
    std::chrono::duration<double, std::milli> d = t2 - t1;
    return d.count();
  }
}

namespace util {
void sleep(uint16_t milli_);
}

#endif /* UTime_hpp */
