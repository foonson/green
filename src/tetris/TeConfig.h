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
  
  using Base=core::Config;
  
  std::string_view appName() override { return "tetris"; } ; // TODO: read from config?
  
  bool initialize(int argc_, const char * argv_[])
  {
    Base::initialize(argc_, argv_);
    
    std::cout << "argc=" << argc_ << "\n";
    for (int i=0;i<argc_;i++) {
      std::cout << "argv[" << i << "]=" << argv_[i] << "\n";
    }
        
    if (asMaster()) {
      _listenPort = 10000;
      _connectPort = 20000;
      //_journalPathName = "/Users/steve/green/tetris.master.journal";
      std::cout << "Master listen:" << _listenPort << " connect:" << _connectPort << "\n";
    } else {
      _listenPort = 20000;
      _connectPort = 10000;
      //_journalPathName = "/Users/steve/green/tetris.journal";
      std::cout << "Client listen:" << _listenPort << " connect:" << _connectPort << "\n";
    }
      
    return true;
  }
    
private:
  //bool _isServer; // TODO: move to core
};
}
#endif /* TeConfig_hpp */
