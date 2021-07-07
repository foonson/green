//
//  Tick.h
//  green
//
//  Created by Steve Cheng on 24/6/2021.
//

#ifndef Tick_h
#define Tick_h

#include "UTime.h"

namespace util {

class Tick {
  
public:
  Tick() {} ;
  auto tickPerMilli()             { return _tickPerMilli; }
  void tickPerMilli(uint64_t u_ ) { _tickPerMilli = u_; }
  auto intervalMilli()            { return _intervalMicro/1000; }
  void intervalMilli(uint64_t u_) { _intervalMicro = u_*1000; }
  auto intervalMicro()            { return _intervalMicro; }
  void intervalMicro(uint64_t u_) { _intervalMicro = u_; }

  
  bool pass() {
    auto now = util::UCPU::cpuTick();
    if (now < _next) { return false; }
    
    //_last = now;
    _next = now + _intervalMicro * _tickPerMilli / 1000;
    return true;
  }
  
private:
  uint64_t _intervalMicro;
  //uint64_t _last;
  uint64_t _next;
  uint64_t _tickPerMilli;

};

}

#endif /* Tick_h */
