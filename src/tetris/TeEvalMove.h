//
//  TeEvalMove.h
//  green
//
//  Created by Steve Cheng on 17/7/2021.
//

#ifndef TeEvalMove_h
#define TeEvalMove_h
#include "TeApp.h"
#include "TeEvaluation.h"

namespace tetris {

class TeEvalMove : public TeEvaluation {
public:
  
  static void pollTimer() {
    //if (!config().isServer()) { return ; }

    if (app().moveTick().pass()) {
      auto* pEvent = eventFactory().createEvent<TeTickMoveEvent>();
      pEvent->handle(app().hPlayer());
      bool b = core::push(app().evalEvents(), pEvent);
      if (!b) {
        std::cout << "evalEvents().push TeTickMoveEvent failure\n";
        core::EventFactory::releaseEvent(pEvent);
      }
    }
  }

  static void evaluate(core::EventPtr pEvent) {
    auto& ctx = context();
    
    auto* xyEvent = eventFactory().createEvent<TeXYEvent>();
    core::Handle hPlayer = pEvent->handle();
    xyEvent->handle(hPlayer);
    
    auto xy = ctx.getXYPtr(hPlayer);    
    xyEvent->_x = *std::get<0>(xy);
    xyEvent->_y = *std::get<1>(xy);
    
    auto* pDir = ctx.getDirectionPtr(hPlayer);

    switch (*pDir) {
      case 0: { xyEvent->_x+=1; break;}
      case 1: { xyEvent->_x-=1; break;}
      case 2: { xyEvent->_y+=1; break;}
      case 3: { xyEvent->_y-=1; break;}
    }
    
    if (xyEvent->_x>800) { xyEvent->_x = 1; }
    if (xyEvent->_y>400) { xyEvent->_y = 1; }
    if (xyEvent->_x==0)  { xyEvent->_x = 800; }
    if (xyEvent->_y==0)  { xyEvent->_y = 400; }
    
    auto b = core::push(contextEvents(), xyEvent);
    if (!b) {
      std::cout << "contextEvents().push failure\n";
      core::EventFactory::releaseEvent(pEvent);
    }

    // TODD: update ui after context update
    auto* uiEvent = eventFactory().createEvent<TeUIEvent>();
    b = core::push(uiEvents(), uiEvent);
    if (!b) {
      std::cout << "uiEvents().push failure\n";
      core::EventFactory::releaseEvent(uiEvent);
    }
  }

  static void updateContext(core::EventPtr pBaseEvent_) {
    auto* pEvent = core::EventFactory::castEvent<TeXYEvent*>(pBaseEvent_);
    uint16_t* px=nullptr;
    uint16_t* py=nullptr;
    std::tie(px, py) = context().getXYPtr(pEvent->handle());
    *px = pEvent->_x;
    *py = pEvent->_y;
  }
}; // TeEvalMove

class TeEvalDir : public TeEvaluation {
  public:

  static void evaluateKey(core::EventPtr pBaseEvent_) {
    auto* pEvent = core::EventFactory::castEvent<TeKeyEvent*>(pBaseEvent_);
    uint8_t direction = -1;
    
    switch (pEvent->_keyCode) {
      case core::KEY_RIGHT: { direction = 0; break; }
      case core::KEY_LEFT:  { direction = 1; break; }
      case core::KEY_DOWN:  { direction = 2; break; }
      case core::KEY_UP:    { direction = 3; break; }
    }

    if (direction!=-1) {
      auto* pCtxEvent = eventFactory().createEvent<TeDirEvent>();
      pCtxEvent->handle(app().hPlayer());
      pCtxEvent->_direction = direction;
      bool b = core::push(app().contextEvents(), pCtxEvent);
      if (!b) {
        std::cout << "contextEvents().push TeDirEvent failure\n";
        core::EventFactory::releaseEvent(pEvent);
      }
    }
  }

  static void updateContext(core::EventPtr pBaseEvent_) {
    auto* pEvent = core::EventFactory::castEvent<TeDirEvent*>(pBaseEvent_);
    auto* dir = context().getDirectionPtr(pEvent->handle());
    *dir = pEvent->_direction;
  }
};
}

#endif /* TeEvalMove_h */
