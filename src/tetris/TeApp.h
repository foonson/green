//
//  tApp.hpp
//  tetris
//
//  Created by Steve Cheng on 14/6/2021.
//

#ifndef tApp_hpp
#define tApp_hpp

#include "core/App.h"
#include "TeUI.h"
#include "TeContext.h"
#include "TeConfig.h"

#include <memory>

namespace tetris {

struct TeAppTraits {
  using        TConfig=tetris::TeConfig;
  using       TContext=tetris::TeContext;
  using            TUI=tetris::TeUI;
  using  TEventFactory=tetris::TeEventFactory;
};

class TeApp : public core::App<TeApp, TeAppTraits > {

typedef core::App<TeApp, TeAppTraits> Base;
  
public:

  static const bool REQUIRE_DROPCOPY_THREAD=true;

  bool initialize(int argc_, const char * argv_[]) {
    if (!Base::initialize(argc_, argv_)) {
      return false;
    }
    _moveTick.intervalMilli(1);
    _moveTick.tickPerMilli(tickPerMilli());
    return true;
  }

  void evaluateTimer();
  void evaluate(core::EventPtr pEvent);
  void updateContext(core::Event* pEvent);

private:
  util::Tick _moveTick;
  
};

}

#endif /* tApp_hpp */
