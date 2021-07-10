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

#define TEEVENT_CASE_ETOSTRING(r, d, elem) \
  case elem : return BOOST_PP_STRINGIZE( elem );

#define AllTeEvents \
  (ETeContextEvent) \
  (ETeXYEvent) \
  (ETeUIEvent) \
  (ETeTickEvent) \
  (ETeTickMoveEvent)

namespace  tetris {

enum TeTargetEnum {
  XY = 1,
};

enum ETeEvent : uint16_t {
  ETeContextEvent  = core::kContextEvent8,
  ETeXYEvent,
  ETeUIEvent       = core::kUIEvent9,
  ETeTickEvent     = core::kTickEvent10,
  ETeTickMoveEvent,
};

class TeEvent : public core::Event {
public:
  static std::string_view name(uint16_t eventType_) {
    switch(eventType_) {
        BOOST_PP_SEQ_FOR_EACH(TEEVENT_CASE_ETOSTRING, TeEvent, AllTeEvents) ;
      default:
        assert(false && "unknown event");
        return "Unknown";
    }
  }
  
  std::string_view name() {
    return name(eventType());
  }
};

EventClass(TeXYEvent, TeEvent)

  void apply(uint16_t& x_, uint16_t& y_) {
    x_ = _x;
    y_ = _y;
  }
  
public:
  uint16_t _x;
  uint16_t _y;

};

EventClass(TeUIEvent, TeEvent)
};

EventClass(TeTickMoveEvent, TeEvent)
};


}

#endif /* TeEvent_h */
