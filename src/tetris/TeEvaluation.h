//
//  TeEvaluation.h
//  green
//
//  Created by Steve Cheng on 17/7/2021.
//

#ifndef TeEvaluation_h
#define TeEvaluation_h
#include "core/SDLUI.h"

namespace tetris {
class TeEvaluation {
public:
  static auto& app()           { return TeApp::app(); }
  static const auto& config()  { return app().config(); }
  static auto& context()       { return TeApp::app().context(); }
  static auto& eventFactory()  { return TeApp::app().eventFactory(); }
  static auto& uiEvents()      { return TeApp::app().uiEvents(); }
  static auto& contextEvents() { return TeApp::app().contextEvents(); }

  static void evaluateKey(core::EventPtr pBaseEvent_) {
    auto* pEvent = core::EventFactory::castEvent<TeKeyEvent*>(pBaseEvent_);
    switch (pEvent->_keyCode) {
      case core::KEY_ESC: {
        auto* pEvalEvent = eventFactory().createEvent<TeSystemEvent>();
        pEvalEvent->handle(app().hPlayer());
        bool b = core::push(app().evalEvents(), pEvalEvent);
        if (!b) {
          std::cout << "evalEvents().push pEvalEvent failure\n";
          core::EventFactory::releaseEvent(pEvent);
        }
        break;
      }
    }
  }

};
}

#endif /* TeEvaluation_h */
