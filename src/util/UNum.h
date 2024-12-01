#pragma once

#include <tuple>

namespace util {
  template<typename T>
  class stats {
  public:
    stats () : _sum(0), _count(0) {}
    std::tuple<T,T> add(T i_) {
        if (_count==0) {
            _max = i_;
            _min = i_;
        }
        if (i_>_max) {
            _max = i_;
        }
        if (i_<_min) {
            _min = i_;
        }

        _sum += i_;
        _count++;
        return {_count, _sum};
    }

    T getAvg() {
        return _sum / _count;
    }

    T _sum;
    T _count;
    T _max;
    T _min;
  };

  
}