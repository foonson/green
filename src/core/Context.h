//
//  AppContext.hpp
//  green
//
//  Created by Steve Cheng on 15/6/2021.
//

#ifndef AppContext_hpp
#define AppContext_hpp


namespace core {

template<typename T>
class Context {
  
public:

  bool initialize() {
    return true;
  }
  
  void shutdown() {
  }

};

}

#endif /* AppContext_hpp */
