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
#include <cassert>
#include <unordered_map>

//#define TEEVENT_CASE_ETOSTRING(r, d, EVENT) \
//case BOOST_PP_CAT(E,EVENT) : return BOOST_PP_STRINGIZE( EVENT );

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
  /*
  static std::string_view name(uint16_t eventType_) {
    switch(eventType_) {
        BOOST_PP_SEQ_FOR_EACH(TEEVENT_CASE_ETOSTRING, TeEvent, AllTeEvents) ;
      default:
        assert(false && "unknown event");
        return "Unknown";
    }
  }
*/
  static void journal(core::Event* pRawEvent) {
    //std::cout << "functor\n";
  }
};

EventClass(TeXYEvent, TeEvent)

  void apply(uint16_t& x_, uint16_t& y_) {
    x_ = _x;
    y_ = _y;
  }

  static void journal(core::Event* pRawEvent) {
    auto* pEvent = castEvent<TeXYEvent*>(pRawEvent);
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


#define TeEvent_Setup_Functor(r, d, EVENT) { \
  TeEventFunctor functor;                    \
  functor.journal   = EVENT::journal;        \
  functor.eventName = EVENT::eventName;      \
  _functors[ BOOST_PP_CAT(E,EVENT) ] = functor; \
}

struct TeEventFunctor {
  void             (*journal)   (core::Event* pEvent);
  std::string_view (*eventName) ();
};

class TeEventFunctors {
  
public:
  TeEventFunctors() {
    BOOST_PP_SEQ_FOR_EACH(TeEvent_Setup_Functor, TeEvent, AllTeEvents) ;
  }
  
  auto eventName(const core::Event* pEvent) {
    return _functors[pEvent->eventType()].eventName();
  }

  auto journal(core::Event* pEvent) {
    return _functors[pEvent->eventType()].journal(pEvent);
  }

private:
  
  std::unordered_map<core::EventType, TeEventFunctor> _functors;
};


}

#endif /* TeEvent_h */
