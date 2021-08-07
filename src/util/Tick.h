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

using TickInterval=uint64_t;

class Tick {
  
public:
  Tick() {
    _next = 0;
  } ;
  
  // copy constructor
  Tick(const Tick& tick_) {
    _intervalMicro = tick_._intervalMicro;
    _tickPerMilli = tick_._tickPerMilli;
    _next = 0;
  }
  
  Tick operator=(const Tick& tick_) {
    _intervalMicro = tick_._intervalMicro;
    _tickPerMilli = tick_._tickPerMilli;
    _next = 0;
    return (*this);
  }
  
  auto tickPerMilli()                 { return _tickPerMilli; }
  void tickPerMilli(TickInterval u_ ) { _tickPerMilli = u_; }
  auto intervalMilli()                { return _intervalMicro/1000; }
  void intervalMilli(TickInterval u_) { _intervalMicro = u_*1000; }
  auto intervalMicro()                { return _intervalMicro; }
  void intervalMicro(TickInterval u_) { _intervalMicro = u_; }
  
  bool pass();
  
private:
  TickInterval _intervalMicro;
  //uint64_t _last;
  TickInterval _next;
  TickInterval _tickPerMilli;

};

}

#endif /* Tick_h */
