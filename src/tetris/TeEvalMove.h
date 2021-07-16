//
//  TeEvalMove.h
//  green
//
//  Created by Steve Cheng on 17/7/2021.
//

#ifndef TeEvalMove_h
#define TeEvalMove_h
#include "TeApp.h"

namespace tetris {

class TeEvalMove {
public:
  
  static auto& app()           { return TeApp::app(); }
  static auto& context()       { return app().context(); }
  static auto& eventFactory()  { return app().eventFactory(); }
  static auto& uiEvents()      { return app().uiEvents(); }
  static auto& contextEvents() { return app().contextEvents(); }

  
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
      case 3: { xyEvent->_x-=1; break;}
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

  static void updateContext(TeXYEvent* pEvent) {
    uint16_t* px=nullptr;
    uint16_t* py=nullptr;
    std::tie(px, py) = TeApp::app().context().getXYPtr(pEvent->handle());
    *px = pEvent->_x;
    *py = pEvent->_y;
  }

    
};
}

#endif /* TeEvalMove_h */
