//
//  TeConfig.hpp
//  green
//
//  Created by Steve Cheng on 3/7/2021.
//

#ifndef TeConfig_hpp
#define TeConfig_hpp

#include "core/includes.h"
#include "core/Config.h"

namespace tetris {
class TeConfig : public core::Config {

public:
  
  bool initialize(int argc_, const char * argv_[])
  {
    std::cout << "argc=" << argc_ << "\n";
    for (int i=0;i<argc_;i++) {
      std::cout << "argv[" << i << "]=" << argv_[i] << "\n";
    }
    
    if (argc_==2) {
      _isServer = false; // client
    } else {
      _isServer = true; // server
    }
    
    if (isServer()) {
      _listenPort = 10000;
      _connectPort = 20000;
      std::cout << "isServer listen:" << _listenPort << " connect:" << _connectPort << "\n";
    } else {
      _listenPort = 20000;
      _connectPort = 10000;
      std::cout << "isClient listen:" << _listenPort << " connect:" << _connectPort << "\n";
    }
    return true;
  }
  
  bool isServer() { return _isServer; }
  
private:
  bool _isServer;
};
}
#endif /* TeConfig_hpp */
