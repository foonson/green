//
//  TeEvaluation.h
//  green
//
//  Created by Steve Cheng on 17/7/2021.
//

#ifndef TeEvaluation_h
#define TeEvaluation_h
#include "TeApp.h"
//#include "core/SDLUI.h"

namespace tetris {

class TeEvaluation {
public:
  static auto& app()           { return TeApp::app(); }
  static auto& busCenter()    { return app().busCenter(); }
  static const auto& config()  { return app().config(); }
  static auto& context()       { return TeApp::app().context(); }
  static auto& eventFactory()  { return TeApp::app().eventFactory(); }
  static auto& uiEvents()      { return busCenter().uiEvents(); }
  static auto& contextEvents() { return busCenter().contextEvents(); }

  static void evaluateKey(core::EventPtr pBaseEvent_) {
    auto* pEvent = core::EventFactory::castEvent<TeKeyEvent*>(pBaseEvent_);
    switch (pEvent->_keyCode) {
      case core::KEY_ESC: {
        auto* pEvalEvent = eventFactory().createEvent<TeSystemEvent>();
        pEvalEvent->handle(busCenter().hPlayer());
        bool b = core::push(busCenter().evalEvents(), pEvalEvent);
        if (!b) {
          std::cout << "evalEvents().push pEvalEvent failure\n";
          core::EventFactory::releaseEvent(pBaseEvent_);
        }
        break;
      }
      default: {
        core::EventFactory::releaseEvent(pBaseEvent_);
      }
    }
  }

};
}

#endif /* TeEvaluation_h */
