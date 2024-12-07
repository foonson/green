#pragma once
// 2024.11

#include <time.h>     // clock_gettime
#include <stdlib.h>   // abs
#include "util/UNum.h"
#include "util/UPerf.h"

namespace test::clock {

  void test() {

    util::NumStats<long> av; 
    util::NumStats<long> avD;
    long countD = 0;
    for (int x=0;x<10000000;x++) {
 
      long e = util::elapse2([](){
        ;
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
        printf("\r%ld e=%ld avg=%ld, max=%ld min=%ld var=%ld D=%ld Dmax=%ld Dmin=%ld ", 
          av._count, 
          e, 
          av.getAvg(), 
          av._max, 
          av._min, 
          av.getVariance(), 
          avD.getAvg(),
          avD._max,
          avD._min
        );
      }
    };
    printf("\n");
  }
}