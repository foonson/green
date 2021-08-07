//
//  TeBusCenter.cpp
//  tetris
//
//  Created by Steve Cheng on 7/8/2021.
//

#include "TeBusCenter.h"
#include "TeEvalMove.h"
#include "TeEvent.h"
#include "TeAppTraits.h"

namespace tetris {
bool TeBusCenter::initializeEval() {

  if (config().asMaster()) {
    _hPlayer = PLAYER0;
  } else {
    _hPlayer = PLAYER1;
  }
  _moveTick.intervalMilli(1);
  _moveTick.tickPerMilli(evalThrottle().tickPerMilli());

  return true;
}

bool TeBusCenter::isArrowKey(SDL_Keycode keyCode_) const {
  
  switch(keyCode_) {
    case core::KEY_LEFT:
    case core::KEY_RIGHT:
    case core::KEY_UP:
    case core::KEY_DOWN:
      return true;
  }
  
  return false;
}

bool TeBusCenter::isConcernKey(SDL_Keycode keyCode_) const {
  switch(keyCode_) {
    case core::KEY_ESC:
      return true;
  }

  return isArrowKey(keyCode_);
/*
  rp0.mapKeyAction(119, ACTION::UP); // w
  rp0.mapKeyAction(97,  ACTION::LEFT); // a
  rp0.mapKeyAction(100, ACTION::RIGHT); // d
  rp0.mapKeyAction(115, ACTION::DOWN); // s
  rp0.name("Naruto");
*/
}


void TeBusCenter::pollInput() {
  SDL_Keycode keyCode=0;
  
  if (!ui().pollKeyboard(keyCode)) { return; }
    
  if (isConcernKey(keyCode)) {
    auto* pEvent = eventFactory().createEvent<TeKeyEvent>();
    pEvent->handle(hPlayer());
    pEvent->_keyCode = keyCode;
    bool b = core::push(evalEvents(), pEvent);
    if (!b) {
      std::cout << "contextEvents().push TeDirEvent failure\n";
      core::EventFactory::releaseEvent(pEvent);
    }
  }

}

void TeBusCenter::pollTimer() {
  return TeEvalMove::pollTimer();
}

void TeBusCenter::evaluate(core::EventPtr pEvent) {
  switch(pEvent->eventType()) {
    case ETeTickMoveEvent: { return TeEvalMove::evaluate(pEvent);  }
    case ETeKeyEvent:      { return evaluateKey(pEvent); }
    case ETeSystemEvent:   { return evaluateSystem(pEvent); }
  }
}

void TeBusCenter::updateContext(core::EventPtr pEvent) {
  switch(pEvent->eventType()) {
    case ETeXYEvent:  { return TeEvalMove::updateContext(pEvent); }
    case ETeDirEvent: { return TeEvalDir::updateContext(pEvent); }
  }
}

void TeBusCenter::evaluateKey(core::EventPtr pBaseEvent_) {
  auto* pEvent = core::EventFactory::castEvent<TeKeyEvent*>(pBaseEvent_);

  if (isArrowKey(pEvent->_keyCode)) {
    return TeEvalDir::evaluateKey(pEvent);
  }
  return TeEvaluation::evaluateKey(pEvent);
}

void TeBusCenter::evaluateSystem(core::EventPtr pBaseEvent_) {
  auto* pEvent = core::EventFactory::castEvent<TeSystemEvent*>(pBaseEvent_);
  setExitFlag();
  return ;
}

}
