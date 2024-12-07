#pragma once
// 2024.12.05

#include <iostream>
#include <map>
#include "util/UPerf.h"
#include "util/UNum.h"

namespace test::memAlloc {

  template <typename T>
  struct MyMemAlloc {
    using value_type = T;

    MyMemAlloc() noexcept = default;

    template <typename U>
    MyMemAlloc(MyMemAlloc<U>&) noexcept {}; 

    T* allocate(std::size_t n_) {
      //std::cout << "Allocate " << n_ << "\n";
      //return static_cast<T*> (::operator new (n_*sizeof(T)));
      
      auto p =  &(_data[_index]);
      _index++;
      return p;
    }

    void deallocate(T* p_, std::size_t n_) noexcept {
      //std::cout << "Deallocate " << n_ << "\n";
      //::operator delete(p_);
    }

    int _index = 0;
    T _data[100000];
  };

  template <typename T, typename U>
  bool operator==(const MyMemAlloc<T>&, const MyMemAlloc<U>&) { return true;}

  template <typename T, typename U>
  bool operator!=(const MyMemAlloc<T>&, const MyMemAlloc<U>&) { return false;}

  int noShiftIndex(int u_, int ) { return u_; }

  int shiftIndex(int u_, int total_) {
    if (u_%2==0) {
      return u_/2;
    } else {
      //return total_/2 + u_/2 ;
      return total_-1 - u_/2;
    }
  }

  template <typename Map>
  void testBase(Map& map_, std::function<int(int,int)>(fnShiftIndex_)) {
    uint64_t i=0;
    util::NumStats<long> statsInsert;
    util::NumStats<long> statsAccess;
    uint64_t total = 10000;
    for (uint64_t u=0;u<total;u++) {
        i = fnShiftIndex_(u, total);

        uint64_t sum=0;
        auto eInsert = util::elapse2([&map_,i](){
          map_.emplace(i,i);
        });

        statsInsert.add(eInsert);
        for (uint64_t v=0;v<u;v++) {
          uint64_t j = shiftIndex(v, total);
          auto eAccess = util::elapse2([&map_,j,&sum](){
            sum+=map_[j];
          });
          statsAccess.add(eAccess);
        }

        printf("i=%ld sum=%ld Add:%ld %ld [%ld %ld] Read:%ld %ld [%ld %ld]\r", i, sum, 
          statsInsert.getAvg(),
          statsInsert.getVariance(),
          statsInsert._min,
          statsInsert._max,
          statsAccess.getAvg(),
          statsAccess.getVariance(),
          statsAccess._min,
          statsAccess._max
        );
    }
    printf("\n");
  }

  void runWithMyMemAlloc() {
    using PII = std::pair<const uint64_t,uint64_t>;
    {
      std::map<uint64_t,uint64_t,std::less<uint64_t>,MyMemAlloc<PII>> m1;
      testBase(m1, shiftIndex);
    }
    { // avoid stack overflow
      std::map<uint64_t,uint64_t,std::less<uint64_t>,MyMemAlloc<PII>> m2;
      testBase(m2, noShiftIndex);
    }
    return;
  }

  void runWithoutMyMemAlloc() {
    std::map<uint64_t,uint64_t> m1;
    testBase(m1, shiftIndex);

    std::map<uint64_t,uint64_t> m2;
    testBase(m1, noShiftIndex);
  }

  void test() {
    printf("MyMemAlloc\n");
    runWithMyMemAlloc();
    printf("Default MemAlloc\n");
    runWithoutMyMemAlloc();
  }
}