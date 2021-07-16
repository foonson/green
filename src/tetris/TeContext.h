//
//  TeContext.hpp
//  green
//
//  Created by Steve Cheng on 15/6/2021.
//

#ifndef TeContext_hpp
#define TeContext_hpp

#include "core/Context.h"
#include "TeEvent.h"

namespace tetris {

struct TePlayerContext {
  uint8_t  direction = 0;
  uint16_t x = 1;
  uint16_t y = 1;
};

class TeContext : public core::Context<TeContext> {
public:
  
  typedef core::Context<TeContext> Base;

  
  TePlayerContext& getPlayerContext(core::Handle hPlayer_) {
    return _player[hPlayer_];
  }
  
  auto getXYPtr(core::Handle hPlayer_) {
    auto& p = getPlayerContext(hPlayer_);
    return std::tuple<uint16_t*, uint16_t*> {&p.x, &p.y} ;
  }

  auto getDirectionPtr(core::Handle hPlayer_) {
    auto& p = getPlayerContext(hPlayer_);
    return &p.direction ;
  }
    
  bool initialize() {
    Base::initialize();
    return true;
  }
  
  void shutdown() {
    Base::shutdown();
  }
  
private:
  TePlayerContext _player[2];
};

}

#endif /* TeContext_hpp */
