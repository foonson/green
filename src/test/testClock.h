#pragma once

#include <time.h>
#include <stdlib.h>
#include <functional>
#include "util/UNum.h"

namespace test::clock {

  long elapse(const std::function<void()>& func_) {
    timespec ts1;
    timespec ts2;
    ::clock_gettime(CLOCK_MONOTONIC, &ts1);
    func_();
    ::clock_gettime(CLOCK_MONOTONIC, &ts2);
    if (ts2.tv_sec==ts1.tv_sec) { return ts2.tv_nsec - ts1.tv_nsec; }
    if (ts2.tv_sec==ts1.tv_sec+1) { return ts2.tv_nsec - ts1.tv_nsec + 1000000000; }
    return -1;
  }

  void test() {
    util::stats<long> av; 
    util::stats<long> avD;
    long countD = 0;
    while (true) {
 
      long e = elapse([](){
        for(int i=0;i<10;i++) {}
      });
      if (e==0) continue;

      long lastAverageD = 0;
      countD++;
      av.add(e);
      bool bPrint = false;
      if (e>av.getAvg()*10) {
        lastAverageD = avD.getAvg();
        avD.add(countD);
        if (abs(lastAverageD-avD.getAvg())>10) 
        {
          bPrint = true;
        }
        countD = 0;
      }
      bPrint = true;

      if (bPrint) {
        printf("\r%ld e=%ld avg=%ld, max=%ld min=%ld D=%ld Dmax=%ld Dmin=%ld ", 
          av._count, 
          e, 
          av.getAvg(), 
          av._max, 
          av._min, 
          avD.getAvg(),
          avD._max,
          avD._min
        );
      }
    };
    /*
    timespec ts1;
    timespec ts2;
    ::clock_gettime(CLOCK_MONOTONIC, &ts1);
    ::clock_gettime(CLOCK_MONOTONIC, &ts2);
    printf("%d %d\n", ts1.tv_sec, ts1.tv_nsec);
    printf("%d %d\n", ts2.tv_sec, ts2.tv_nsec);
    */
  }
}