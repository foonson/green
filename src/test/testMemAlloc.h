#pragma once
// 2024.12.05

#include <iostream>
#include <map>
#include "util/UPerf.h"
#include "util/UNum.h"

namespace test {
  int noShiftIndex(const int u_, const int ) { return u_; }

  int shiftIndex(const int u_, const int total_) {
    if (u_%2==0) {
      return u_/2;
    } else {
      //return total_/2 + u_/2 ;
      return total_-1 - u_/2;
    }
  }
}

namespace test::newdelete {

  struct HugeObject {
    bool dirty=false;
    char buffer[10000];
  };

  template <typename T, size_t N>
  class NaiveMemoryPool {
  public:
    NaiveMemoryPool() {
      size_t pageNum;
      if ((N % pageSize)==0) {
        pageNum = N/pageSize;
      } else {
        pageNum = N/pageSize+1;
      }
      for (size_t i;i<pageNum;i++) {
        _pages[i] = new T[pageSize];
      }
    }

    T* newObject() {
      size_t pageNum = _index/pageSize;
      auto page = _pages[pageNum];
      auto p = &(page[_index%pageSize]);
      _index++;
      return p;
    }

    void releaseObject(T* o_) {
    }

    static constexpr int pageSize = 10000;
    std::array<T*, N/pageSize+1> _pages;
    size_t _index = 0;
  };

  template <typename T>
  class NaiveMemoryPoolOndemand {

  public:
    //NaiveMemoryPoolOndemand() {
    //}

    T* newObject() {
      return new T;
    }

    void releaseObject(T* o_) {
      delete o_;
    }
  };

  template<typename POOL>
  void testBase(const size_t total_) {
 
    HugeObject* pdata[total_];

    util::NumStats newStats;
    util::NumStats deleteStats;

    util::StopWatch swTest;

    swTest.start();
    POOL pool;
    auto poolTime = swTest.getElapsedMilli();


    swTest.start();
    for (size_t i=0;i<total_;i++) {
      util::StopWatch sw;
      sw.start();
      pdata[i] = pool.newObject();
      sw.stop();
      pdata[i]->buffer[0] = (i%52) + 'A';
      pdata[i]->buffer[2] = '\0';
      newStats.add(sw.getElapsedTick());
    }

    for (size_t i=0;i<total_;i++) {
      auto j = shiftIndex(i, total_);
      pdata[i]->buffer[1] = (j%52) + 'A';
      //printf("%s",pdata[j]->buffer);
    }
    //printf("\n");

    for (size_t i=0;i<total_;i++) {
      auto j = shiftIndex(i, total_);
      util::StopWatch sw;
      sw.start();
      pool.releaseObject(pdata[j]);
      sw.stop();
      deleteStats.add(sw.getElapsedTick());
    }

    std::printf("Total:%d alloc new:%ld %ld delete:%ld %ld pre:%ldms loop:%ldms\n", 
      total_, 
      newStats.getAvg(), 
      newStats.getVariance(), 
      deleteStats.getAvg(),
      deleteStats.getVariance(),
      poolTime,
      swTest.getElapsedMilli()
    );
  }

  void test(int mode_) {
    constexpr int total=100000;
    if (mode_==1) {
      printf("NaiveMemoryPool\n");
      testBase<NaiveMemoryPool<HugeObject, total>>(total);;
    } else if (mode_==2) {
      printf("NaiveMemoryPoolOndemand\n");
      testBase<NaiveMemoryPoolOndemand<HugeObject>>(total);;
    }
  }
}

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
    T _data[10001];
  };

  template <typename T, typename U>
  bool operator==(const MyMemAlloc<T>&, const MyMemAlloc<U>&) { return true;}

  template <typename T, typename U>
  bool operator!=(const MyMemAlloc<T>&, const MyMemAlloc<U>&) { return false;}

  template <typename Map>
  void testBase(Map& map_, std::function<int(int,int)>(fnShiftIndex_)) {
    uint64_t i=0;
    util::NumStats<long> statsInsert;
    util::NumStats<long> statsAccess;
    uint64_t total = 10000;
    uint64_t sum=0;
    for (uint64_t u=0;u<total;u++) {
      i = fnShiftIndex_(u, total);

      auto eInsert = util::elapse2([&map_,i,u](){
        map_.emplace(i,u);
      });
      statsInsert.add(eInsert);

      for (uint64_t v=0;v<u;v++) {
        uint64_t j = fnShiftIndex_(v, total);
        auto eAccess = util::elapse2([&map_,j,&sum](){
          if (map_.find(j)==map_.end()) {
            //assert(false, "item not in map");
            std::abort();
          }
          sum+=map_[j];
        });
        statsAccess.add(eAccess);
      }
    }
    printf("sum=%ld Add:%ld %ld [%ld %ld] Read:%ld %ld [%ld %ld] ", sum, 
      statsInsert.getAvg(),
      statsInsert.getVariance(),
      statsInsert._min,
      statsInsert._max,
      statsAccess.getAvg(),
      statsAccess.getVariance(),
      statsAccess._min,
      statsAccess._max
    );


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

  void test(int para_) {
    if (para_==1) {
      printf("MyMemAlloc\n");
      runWithMyMemAlloc();
    }
    if (para_==2) {
      printf("Default MemAlloc\n");
      runWithoutMyMemAlloc();
    }
  }
}