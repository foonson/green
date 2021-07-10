//
//  TeContext.hpp
//  green
//
//  Created by Steve Cheng on 15/6/2021.
//

#ifndef TeContext_hpp
#define TeContext_hpp

#include "core/AppContext.h"
#include "TeEvent.h"

namespace tetris {

class TeContext : public core::AppContext<TeContext> {
public:
  
  typedef core::AppContext<TeContext> Base;
  
  void updateContext(core::Event* pBaseEvent) {
    switch(pBaseEvent->handle()) {
      case TeTargetEnum::XY:
      {
        auto* pEvent = core::Event::castEvent<TeXYEvent*>(pBaseEvent);
        pEvent->apply(_x, _y);
      }
    }
  }
  
  bool initialize() {
    Base::initialize();
    _x = 0;
    _y = 0;
    return true;
  }
  
  void shutdown() {
    Base::shutdown();
  }
  
  
  uint16_t _x = 10;
  uint16_t _y = 10;
};

}

#endif /* TeContext_hpp */
