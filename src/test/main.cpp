// 2024.11

#include <stdio.h>
#include <csignal>
#include <thread>
#include <iostream>
#include <functional>
#include <unistd.h>

#include "util/UCommon.h"
#include "util/UThread.h"
#include "util/UConsole.h"
#include "util/UCPU.h"
#include "testClock.h"
#include "testMemAlloc.h"

void mainEnd() {
  uint64_t tick = util::cpuTick();
  printf("main end %ld\n", tick);
  util::showCursor();
}

void mySignalHandler(int signal_) {
  mainEnd();
  std::exit(-1);
}

void registerSignalHandler() {
  std::signal(SIGTERM, mySignalHandler);
  std::signal(SIGINT,  mySignalHandler);
  std::signal(SIGSEGV, mySignalHandler);
}

int main(int argc_, char* argv_[]) {

  util::StopWatch sw;
  sw.start();
  printf("-c CPUID\n"
  "-t 1:MyMemAlloc\n"
  "-t 2:Default memAlloc\n"
  "-t 3:clock\n"
  "-t 4:new delete memorypool\n"
  "-t 5:new delete default\n"
  "-t 6:concat string\n"

  );

  uint64_t tickPerMilli = util::getTickPerMilli();

  int cpuID = 2;
  int testcaseID = 4;

  int opt = 0;
  while ((opt=getopt(argc_, argv_, "c:t:"))!=-1) {
    switch (opt) {
      case 'c': cpuID = atoi(optarg); break;
      case 't': testcaseID = atoi(optarg); break;
    }
  }

  util::log("cpu=", cpuID, "testcase=", testcaseID);

  uint64_t tick = util::cpuTick();
  printf("main started %ld Core:%d\n", tick, util::coreCount());
  registerSignalHandler();
  util::hideCursor();    //util::showCursor();

  std::string s;
  auto f = [&s](const size_t i){
        s += (char)((i%52) + 'A') ;
        //s += s;
      };
  std::thread t(util::runThreadAtCore, cpuID, [testcaseID, &f, &s](){ 
    switch(testcaseID) {
      case 1: test::memAlloc::test(1); break;
      case 2: test::memAlloc::test(2); break;
      case 3: test::clock::test(); break;
      case 4: test::newdelete::test(1); break;
      case 5: test::newdelete::test(2); break;
      case 6: { 
        util::benchmark(f, 100000000);
        printf("%d\n",s.length());
      }  break;
    }
  });
  t.join();

  sw.stop();
  printf("millisecond:%dms\n", sw.getElapsedMilli());
  mainEnd();
  return -1;
}
