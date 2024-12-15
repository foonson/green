#pragma once
// 2024.11

#include <tuple>

namespace util {
  template<typename T=uint64_t>
  class NumStats {
  public:
    NumStats () {}
    void add(T i_) {
        if (_count==0) {
            _max = i_;
            _min = i_;
        }
        if (i_>_max) { _max = i_; }
        if (i_<_min) { _min = i_; }

        _sum2 += (i_*i_);
        _sum += i_;
        _count++;
    }

    T getAvg() {
        return _sum / _count;
    }

    T getVariance() {
        T avg = getAvg();
        return _sum2/_count - (avg*avg);
    }

    T _sum   = 0;
    T _sum2  = 0; // sum of square
    T _count = 0;
    T _max   = 0;
    T _min   = 0;
  };

  
}