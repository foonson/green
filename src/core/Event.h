//
//  Event.h
//  green
//
//  Created by Steve Cheng on 24/6/2021.
//

#ifndef Event_h
#define Event_h


namespace core {

const uint16_t  kContextEvent8=0x1 << 9;
const uint16_t       kUIEvent9=0x1 << 10;
const uint16_t    kTickEvent10=0x1 << 11;
const uint16_t   kContextEvent=kContextEvent8;
const uint16_t        kUIEvent=kUIEvent9;
const uint16_t      kTickEvent=kTickEvent10;


typedef uint32_t Handle;

class Event {
public:

  typedef uint16_t EventType;

  void size(uint16_t u_)        { _size = u_; }
  void eventType(EventType u_)   { _eventType = u_; }
  void tick(uint64_t u_)        { _tick = u_; }
  void handle(Handle u_)        { _handle = u_; }
  void isFromDropcopy(bool u_)  { _isFromDropcopy = u_; }

  auto size()           { return _size; }
  auto eventType()      { return _eventType; }
  auto tick()           { return _tick; }
  auto handle()         { return _handle; }
  auto isFromDropcopy() { return _isFromDropcopy; }
  
  bool isContextEvent() { return isContextEvent(eventType()); }
  bool isUIEvent()      { return isUIEvent     (eventType()); }
  bool isTickEvent()    { return isTickEvent   (eventType()); }

  static bool isContextEvent(EventType type_ ) {
    return (type_&kContextEvent) == kContextEvent;
  }

  static bool isUIEvent(EventType type_ ) {
    return (type_&kUIEvent) == kUIEvent;
  }

  static bool isTickEvent(EventType type_ ) {
    return (type_&kTickEvent) == kTickEvent;
  }

private:
  uint16_t  _size;
  EventType _eventType;
  uint64_t  _tick;
  Handle    _handle;
  bool      _isFromDropcopy = false;
};

using EventSPtr=std::shared_ptr<core::Event>;

}

#endif /* Event_h */
