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
    evalEvents().push(core::EventSPtr(new TeTickMoveEvent()));
  }
}

void TeApp::evaluate(core::EventSPtr pEvent) {

  switch(pEvent->eventType()) {
    case ETeTickMoveEvent: { evaluateMove(pEvent); return; }
  }

  if (pEvent->isContextEvent()) {
    contextEvents().push(pEvent);
    // update UI
    auto* uiEvent = new TeUIEvent();
    uiEvent->size(sizeof(uiEvent));
    uiEvents().push(core::EventSPtr(uiEvent));
  }
}

void TeApp::evaluateMove(core::EventSPtr pEvent) {
  auto& rctx = context();
  
  auto x = rctx._x;
  auto* xyEvent = new TeXYEvent();
  xyEvent->handle(TeTargetEnum::XY);

  xyEvent->_y = 0;
  if (x<800) {
    xyEvent->_x = x + 1;
  } else {
    xyEvent->_x = 0;
  }
  contextEvents().push(core::EventSPtr(xyEvent));

  auto* uiEvent = new TeUIEvent();
  uiEvent->size(sizeof(uiEvent));
  uiEvents().push(core::EventSPtr(uiEvent));
  
}


}
