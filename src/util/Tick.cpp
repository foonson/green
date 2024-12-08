//
//  Tick.cpp
//  green
//
//  Created by Steve Cheng on 7/8/2021.
//

#include "Tick.h"
#include "UCPU.h"

namespace util {

bool Tick::pass() {
  auto now = util::cpuTick();
  if (now < _next) { return false; }
  
  //_last = now;
  _next = now + _intervalMicro * _tickPerMilli / 1000;
  return true;
}

}
