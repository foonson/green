//
//  main.cpp
//  busstop
//
//  Created by Steve Cheng on 12/6/2021.
//

#include <core/includes.h>
//#include <sys/sysctl.h>
template<typename T>
class A {
public:
  void foo() {
    std::cout << "A:foo()";
  }
};

template<typename T>
class B : public A<B<T> > {
public:
  void foo() {
    std::cout << "B:foo()";
  }
};

class C : public B<C> {
public:
  void foo() {
    std::cout << "C:foo()";
  }
};



int main(int argc, const char * argv[]) {
    // insert code here...
    //std::cout << "Hello, busstop!\n";
    //return 0;
  //C o;
  //o.foo();


}
