//
//  tApp.cpp
//  tetris
//
//  Created by Steve Cheng on 14/6/2021.
//

#include "TeApp.h"
#include <memory>
#include "util/UCPU.h"

namespace tetris {

void TeApp::evaluateTimer() {
  if (!config().isServer()) { return ; }

  if (_moveTick.pass()) {
    auto* pEvent = core::Event::createEvent<TeTickMoveEvent>();    
    bool b = core::push(evalEvents(), pEvent);
    if (!b) {
      std::cout << "evalEvents().push failure\n";
      core::Event::releaseEvent(pEvent);
    }
  }
}

void TeApp::evaluate(core::EventPtr pEvent) {

  switch(pEvent->eventType()) {
    case ETeTickMoveEvent: { return evaluateMove(pEvent);  }
  }

}

void TeApp::evaluateMove(core::EventPtr pEvent) {
  auto& rctx = context();
  
  auto x = rctx._x;
  auto* xyEvent = core::Event::createEvent<TeXYEvent>();
  xyEvent->handle(TeTargetEnum::XY);

  xyEvent->_y = 0;
  if (x<800) {
    xyEvent->_x = x + 1;
  } else {
    xyEvent->_x = 0;
  }
  
  auto b = core::push(contextEvents(), xyEvent);
  if (!b) {
    std::cout << "contextEvents().push failure\n";
    core::Event::releaseEvent(pEvent);
  }

  auto* uiEvent = core::Event::createEvent<TeUIEvent>();
  b = core::push(uiEvents(), uiEvent);
  if (!b) {
    std::cout << "uiEvents().push failure\n";
    core::Event::releaseEvent(uiEvent);
  }
  
}


}
