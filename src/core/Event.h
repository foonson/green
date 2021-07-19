//
//  Event.h
//  green
//
//  Created by Steve Cheng on 24/6/2021.
//

#ifndef Event_h
#define Event_h

#include "core/includes.h"
#include <unordered_map>

#define class_Event(CLASSNAME, BASECLASS) \
class CLASSNAME : public BASECLASS { \
public: \
  using Type = CLASSNAME;    \
  CLASSNAME() {              \
    BASECLASS();             \
    size(sizeof(CLASSNAME)); \
    eventType(E##CLASSNAME); \
  }                          \
  static std::string_view eventName() { return BOOST_PP_STRINGIZE(CLASSNAME); }



namespace core {

class Event;

using Handle=uint32_t;
using EventSize=uint16_t;
using EventType=uint16_t;

const EventType  kContextEvent9=0x1 << 9;
const EventType      kUIEvent10=0x1 << 10;
const EventType    kTickEvent11=0x1 << 11;
const EventType     kKeyEvent12=0x1 << 12;
const EventType  kSystemEvent13=0x1 << 13;
const EventType   kContextEvent=kContextEvent9;
const EventType        kUIEvent=kUIEvent10;
const EventType      kTickEvent=kTickEvent11;
const EventType       kKeyEvent=kKeyEvent12;
const EventType    kSystemEvent=kSystemEvent13;

#define Event_Setup_Functor(r, d, EVENT) {   \
  core::EventFunctor functor;                      \
  functor.humanReader = EVENT::humanReader;        \
  functor.eventName   = EVENT::eventName;      \
  _functors[ BOOST_PP_CAT(E,EVENT) ] = functor; \
}

struct EventFunctor {
  void             (*humanReader) (core::Event* pEvent);
  std::string_view (*eventName)   ();
};

#define FLAG_ACCESSER(FLAG_TYPE)    \
  bool FLAG_TYPE () const  { return (_flag & Flag_##FLAG_TYPE) == Flag_##FLAG_TYPE; } \
  void FLAG_TYPE (bool u_) {          \
    if (u_) {                       \
      _flag |= Flag_ ## FLAG_TYPE;  \
    } else {                        \
      _flag &= !Flag_ ## FLAG_TYPE; \
    }                               \
  }                                 \

class Event {

private:
  using Flag=uint32_t;
  const static Flag Flag_isFromDropcopy     = 1;
  const static Flag Flag_createdFromBuffer  = 1 << 1;

public:
  Event();

  FLAG_ACCESSER(createdFromBuffer)
  FLAG_ACCESSER(isFromDropcopy)

  void size(uint16_t u_)        { _size = u_; }
  void eventType(EventType u_)  { _eventType = u_; }
  void tick(uint64_t u_)        { _tick = u_; }
  void handle(Handle u_)        { _handle = u_; }
  void dcSeqno(uint32_t u_)     { _dcSeqno = u_; }
  void setFunctorPtr(EventFunctor* f_) { _functorPtr = f_; }

  auto size()           const { return _size; }
  auto eventType()      const { return _eventType; }
  auto tick()           const { return _tick; }
  auto handle()         const { return _handle; }
  auto dcSeqno()        const { return _dcSeqno; }
  auto functorPtr()     const { return _functorPtr; }

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
  
  auto eventName() {
    assert(_functorPtr!=nullptr);
    return (_functorPtr->eventName)();
  }

  auto humanReader() {
    assert(_functorPtr!=nullptr);
    return (_functorPtr->humanReader)(this);
  }
  
  static void humanReaderPrefix(Event* pEvent) {
    std::cout << pEvent->tick()
      << " [" << pEvent->dcSeqno() << "] "
      << pEvent->size() << "bytes "
      << pEvent->eventName() << " ";
  }
  
private:
  uint16_t  _size      = 0;
  EventType _eventType = 0;
  uint32_t  _dcSeqno   = 0;
  uint64_t  _tick      = 0;
  Handle    _handle    = 0;
  
  Flag  _flag      = 0;
  
  // virtual pointer to manual virtual table
  EventFunctor* _functorPtr = nullptr;

};


// TODO: header event
//EventClass(HeaderEvent)

using EventPtr=core::Event*;

template<typename QUEUE, typename EVENT>
bool push(QUEUE& q_, EVENT* pEvent_) {
  return q_.push( static_cast<core::EventPtr>(pEvent_) );
}

class EventFactory {
  
public:
  friend class Event;

  template<typename T>
  T* createEvent() {
    auto* pEvent = new T;
    pEvent->createdFromBuffer(false);
    auto* fp = functorPtr(pEvent->eventType());
    pEvent->setFunctorPtr(fp);
    return pEvent;
  }
  
  Event* createEvent(char* pBuffer_, EventSize eventSize_) {
    auto pEvent = castEvent(pBuffer_, eventSize_);
    pEvent->createdFromBuffer(true);
    return pEvent;
  }
  
  static void releaseEvent(Event* pEvent_) {
    if (pEvent_->createdFromBuffer()) {
      delete [] ((char*)pEvent_);
    } else {
      delete pEvent_;
    }
  }
  
  template<typename T>
  static T castEvent(Event* pEvent) { return reinterpret_cast<T>(pEvent); }

  Event* castEvent(char* pBuffer_, EventSize eventSize_) {
    *(reinterpret_cast<EventSize*>(pBuffer_)) = eventSize_;
    auto* pEvent = reinterpret_cast<Event*>(pBuffer_);
    auto* fp = functorPtr(pEvent->eventType());
    pEvent->setFunctorPtr(fp);
    return pEvent;
  }
  
  EventFunctor* functorPtr(EventType eventType_) {
    return &(_functors[eventType_]);
  }
  
protected:
  std::unordered_map<core::EventType, core::EventFunctor> _functors;
};



}

#endif /* Event_h */
