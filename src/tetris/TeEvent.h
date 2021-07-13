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
  (TeXYEvent) \
  (TeUIEvent) \
  (TeTickMoveEvent)

namespace tetris {

enum TeTargetEnum {
  XY = 1,
};

enum ETeEvent : core::EventType {
  ETeContextEvent  = core::kContextEvent8,
  ETeXYEvent,
  ETeUIEvent       = core::kUIEvent9,
  ETeTickEvent     = core::kTickEvent10,
  ETeTickMoveEvent,
};

class TeEvent : public core::Event {
public:
  
  static void humanReader(core::Event* pRawEvent) {
    humanReaderPrefix(pRawEvent);
    std::cout << "\n";
  }
};

EventClass(TeXYEvent, TeEvent)

  void apply(uint16_t& x_, uint16_t& y_) {
    x_ = _x;
    y_ = _y;
  }

  static void humanReader(core::Event* pRawEvent) {
    auto* pEvent = core::EventFactory::castEvent<TeXYEvent*>(pRawEvent);
    humanReaderPrefix(pRawEvent);
    std::cout << "x=" << pEvent->_x << " y=" << pEvent->_y << "\n";
  }

public:
  uint16_t _x;
  uint16_t _y;

};

EventClass(TeUIEvent, TeEvent)
};

EventClass(TeTickMoveEvent, TeEvent)
};


class TeEventFactory : public core::EventFactory {
  
public:
  TeEventFactory() {
    BOOST_PP_SEQ_FOR_EACH(Event_Setup_Functor, core::Event, AllTeEvents) ;
  }
};


}

#endif /* TeEvent_h */
