#pragma once
// 2024.12.05

#include <iostream>
#include <map>
#include "util/UPerf.h"

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
      std::cout << "Deallocate " << n_ << "\n";
      //::operator delete(p_);
    }

    int _index = 0;
    T _data[100000];


  };

  template <typename T, typename U>
  bool operator==(const MyMemAlloc<T>&, const MyMemAlloc<U>&) { return true;}

  template <typename T, typename U>
  bool operator!=(const MyMemAlloc<T>&, const MyMemAlloc<U>&) { return false;}

  void test() {


    using PII = std::pair<const uint64_t,uint64_t>;
    std::map<uint64_t,uint64_t,std::less<uint64_t>,MyMemAlloc<PII>> m;
    //std::map<uint64_t,uint64_t> m;
    uint64_t i=0;
    for (uint64_t i=0;i<20000;i++) {
        i++;
        uint64_t sum=0;
        auto e = util::elapse2([&m,i,&sum](){
            m.emplace(i,i);
            for (uint64_t j=0;j<i;j++) {
              sum+=m[j];
            }
        });
        printf("i=%ld sum=%ld e=%ld\r",i, sum, e);
    }

  }
}