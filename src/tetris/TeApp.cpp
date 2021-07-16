//
//  tApp.cpp
//  tetris
//
//  Created by Steve Cheng on 14/6/2021.
//

#include "TeApp.h"
#include <memory>
#include "util/UCPU.h"
#include "TeEvalMove.h"

namespace tetris {

void TeApp::evaluateTimer() {
  if (!config().isServer()) { return ; }

  if (_moveTick.pass()) {
    auto* pEvent = eventFactory().createEvent<TeTickMoveEvent>();
    uint32_t PLAYER0 = 0;
    pEvent->handle(PLAYER0);
    bool b = core::push(evalEvents(), pEvent);
    if (!b) {
      std::cout << "evalEvents().push failure\n";
      core::EventFactory::releaseEvent(pEvent);
    }
  }
}

void TeApp::evaluate(core::EventPtr pEvent) {
  switch(pEvent->eventType()) {
    case ETeTickMoveEvent: { return TeEvalMove::evaluate(pEvent);  }
  }
}

void TeApp::updateContext(core::EventPtr pBaseEvent) {
  switch(pBaseEvent->eventType()) {
    case ETeXYEvent:
    {
      auto* pEvent = core::EventFactory::castEvent<TeXYEvent*>(pBaseEvent);
      TeEvalMove::updateContext(pEvent);
    }
  }
}





}
