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
  }                          \
  static std::string_view eventName() { return BOOST_PP_STRINGIZE(CLASSNAME); }

namespace core {


using Handle=uint32_t;
using EventSize=uint16_t;
using EventType=uint16_t;

const EventType  kContextEvent8=0x1 << 9;
const EventType       kUIEvent9=0x1 << 10;
const EventType    kTickEvent10=0x1 << 11;
const EventType   kContextEvent=kContextEvent8;
const EventType        kUIEvent=kUIEvent9;
const EventType      kTickEvent=kTickEvent10;

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

  void size(uint16_t u_)        { _size = u_; }
  void eventType(EventType u_)  { _eventType = u_; }
  void tick(uint64_t u_)        { _tick = u_; }
  void handle(Handle u_)        { _handle = u_; }
  void isFromDropcopy(bool u_)  { _isFromDropcopy = u_; }
  void dcSeqno(uint32_t u_)     { _dcSeqno = u_; }

  auto size()           const { return _size; }
  auto eventType()      const { return _eventType; }
  auto tick()           const { return _tick; }
  auto handle()         const { return _handle; }
  auto isFromDropcopy() const { return _isFromDropcopy; }
  auto dcSeqno()        const { return _dcSeqno; }

  bool isContextEvent() const { return isContextEvent(eventType()); }
  bool isUIEvent()      const { return isUIEvent     (eventType()); }
  bool isTickEvent()    const { return isTickEvent   (eventType()); }
  
  const char* asCharBuffer() const { return reinterpret_cast<const char*>(this); }
  
  static bool isContextEvent(EventType type_ ) {
    return (type_&kContextEvent) == kContextEvent;
  }

  static bool isUIEvent(EventType type_ ) {
    return (type_&kUIEvent) == kUIEvent;
  }

  static bool isTickEvent(EventType type_ ) {
    return (type_&kTickEvent) == kTickEvent;
  }

  /*
  static void journalPrefix() {
    std::cout << ptEvent->dcSeqno() << " " << ptEvent->size() << "bytes " << ptEvent->eventType() << " " << ptEvent->name()  << "\n";
  }
   */
  
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
