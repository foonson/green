//
//  TeEvent.h
//  green
//
//  Created by Steve Cheng on 24/6/2021.
//

#ifndef TeEvent_h
#define TeEvent_h

#include "core/Event.h"
#include <boost/preprocessor.hpp>
//#include <cassert>

#define AllTeEvents \
  (TeXYEvent)       \
  (TeDirEvent)      \
  (TeUIEvent)       \
  (TeTickMoveEvent) \
  (TeKeyEvent)      \
  (TeSystemEvent)

namespace tetris {

/*
enum TeTargetEnum {
  XY = 1,
};
 */

enum ETeEvent : core::EventType {
  ETeContextEvent  = core::kContextEvent9,
  ETeXYEvent,
  ETeDirEvent,
  ETeUIEvent       = core::kUIEvent10,
  ETeTickEvent     = core::kTickEvent11,
  ETeTickMoveEvent,
  ETeKeyEvent      = core::kKeyEvent12,
  ETeSystemEvent   = core::kSystemEvent13,
};

class TeEvent : public core::Event {
public:
  
  static void humanReader(core::Event* pRawEvent) {
    humanReaderPrefix(pRawEvent);
    std::cout << "\n";
  }
};

class_Event(TeDirEvent, TeEvent)

  static void humanReader(core::Event* pRawEvent) {
    auto* pEvent = core::EventFactory::castEvent<Type*>(pRawEvent);
    humanReaderPrefix(pRawEvent);
    std::cout << "Player" << pEvent->handle() << " dir=" << pEvent->_direction << "\n";
  }

public:
  uint8_t _direction;
};


class_Event(TeXYEvent, TeEvent)

  static void humanReader(core::Event* pRawEvent) {
    auto* pEvent = core::EventFactory::castEvent<Type*>(pRawEvent);
    humanReaderPrefix(pRawEvent);
    std::cout << "Player" << pEvent->handle() << " (" << pEvent->_x << "," << pEvent->_y << ")\n";
  }

public:
  uint16_t _x;
  uint16_t _y;
};

class_Event(TeKeyEvent, TeEvent)
  static void humanReader(core::Event* pRawEvent) {
    auto* pEvent = core::EventFactory::castEvent<Type*>(pRawEvent);
    humanReaderPrefix(pRawEvent);
    std::cout << "Player" << pEvent->handle() << " " << pEvent->_keyCode << "\n";
  }

public:
  SDL_Keycode _keyCode;
};

class_Event(TeSystemEvent, TeEvent)
  static void humanReader(core::Event* pRawEvent) {
    auto* pEvent = core::EventFactory::castEvent<Type*>(pRawEvent);
    humanReaderPrefix(pRawEvent);
    std::cout << "Player" << pEvent->handle() << " " << pEvent->_systemAction << "\n";
  }

public:
  uint8_t _systemAction;
};


class_Event(TeUIEvent, TeEvent)
};

class_Event(TeTickMoveEvent, TeEvent)
};


class TeEventFactory : public core::EventFactory {
  
public:
  TeEventFactory() {
    BOOST_PP_SEQ_FOR_EACH(Event_Setup_Functor, core::Event, AllTeEvents) ;
  }
};

}

#endif /* TeEvent_h */
