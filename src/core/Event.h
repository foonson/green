//
//  Event.h
//  green
//
//  Created by Steve Cheng on 24/6/2021.
//

#ifndef Event_h
#define Event_h


#define EventClass(CLASSNAME, BASECLASS) \
class CLASSNAME : public BASECLASS { \
public: \
  CLASSNAME() { \
    size(sizeof(CLASSNAME)); \
    eventType(E##CLASSNAME); \
  }


namespace core {

const uint16_t  kContextEvent8=0x1 << 9;
const uint16_t       kUIEvent9=0x1 << 10;
const uint16_t    kTickEvent10=0x1 << 11;
const uint16_t   kContextEvent=kContextEvent8;
const uint16_t        kUIEvent=kUIEvent9;
const uint16_t      kTickEvent=kTickEvent10;


typedef uint32_t Handle;
using EventSize=uint16_t;

class Event {
public:

  template<typename T>
  static T* createEvent() {
    auto* pEvent = new T;
    pEvent->_createdFromBuffer = false;
    return pEvent;
  }
  
  static Event* createEvent(char* pBuffer_, EventSize eventSize_) {
    auto pEvent = castEvent(pBuffer_, eventSize_);
    pEvent->_createdFromBuffer = true;
    return pEvent;
  }
  
  static void releaseEvent(Event* pEvent_) {
    if (pEvent_->_createdFromBuffer) {
      delete [] ((char*)pEvent_);
    } else {
      delete pEvent_;
    }
  }
  
  template<typename T>
  static T castEvent(Event* pEvent) { return reinterpret_cast<T>(pEvent); }

  static Event* castEvent(char* pBuffer_, EventSize eventSize_) {
    *(reinterpret_cast<EventSize*>(pBuffer_)) = eventSize_;
    auto* pEvent = reinterpret_cast<Event*>(pBuffer_);
    return pEvent;
  }

  
  typedef uint16_t EventType;

  void size(uint16_t u_)        { _size = u_; }
  void eventType(EventType u_)  { _eventType = u_; }
  void tick(uint64_t u_)        { _tick = u_; }
  void handle(Handle u_)        { _handle = u_; }
  void isFromDropcopy(bool u_)  { _isFromDropcopy = u_; }
  void dcSeqno(uint32_t u_)     { _dcSeqno = u_; }

  auto size()           { return _size; }
  auto eventType()      { return _eventType; }
  auto tick()           { return _tick; }
  auto handle()         { return _handle; }
  auto isFromDropcopy() { return _isFromDropcopy; }
  auto dcSeqno()        { return _dcSeqno; }

  bool isContextEvent() { return isContextEvent(eventType()); }
  bool isUIEvent()      { return isUIEvent     (eventType()); }
  bool isTickEvent()    { return isTickEvent   (eventType()); }
  
  const char* asCharBuffer() { return reinterpret_cast<char*>(this); }

  /*
  std::string_view name() {
    return static_cast<T*>(this)->name(eventType());
  }
   */
  
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
  uint32_t  _dcSeqno;
  uint64_t  _tick;
  Handle    _handle;
  bool      _isFromDropcopy = false;
  
  // internal
  bool      _createdFromBuffer = false; // for memory mgmt
  
};

//EventClass(HeaderEvent)

using EventPtr=core::Event*;

template<typename QUEUE, typename EVENT>
bool push(QUEUE& q_, EVENT* pEvent_) {
  return q_.push( static_cast<core::EventPtr>(pEvent_) );
}

}

#endif /* Event_h */
