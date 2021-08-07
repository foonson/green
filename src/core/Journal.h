//
//  Reader.h
//  green
//
//  Created by Steve Cheng on 11/7/2021.
//

#ifndef Reader_h
#define Reader_h

#include "core/includes.h"
#include "core/Event.h"
#include "core/Queue.h"


namespace core {

template<typename TAppTraits>

// TODO: Journal is a sub class of Channel

class Journal {
public:

  using       TContext=typename TAppTraits::TContext;
  //using        TConfig=typename TAppTraits::TConfig;
  //using            TUI=typename TAppTraits::TUI;
  using  TEventFactory=typename TAppTraits::TEventFactory;


  bool initialize(
    std::string_view journalPathName_,
    TEventFactory&   eventFactory_
  ) {
    _journalPathName = journalPathName_;
    _pEventFactory   = &eventFactory_;
    return true;
  }
    
  bool recoverFromJournal(EventQueue& evalEvents_, std::function<void()> fnEvalOnce ) {
    
    std::ifstream js(pathName());
    do {
      Event* pEvent = eventFactory().createEventFromStream(
        [&js](char* buffer_, EventSize size_) -> bool {
          if (!js.read(buffer_, size_)) { return false; }
          return true;
        }
      );
      
      if (pEvent==nullptr) {
        break;
      }
      pEvent->humanReader();

      core::push(evalEvents_, pEvent);
      fnEvalOnce();
      
    } while (true);
    return true;
  }
  
  void readJournal() {

    std::cout << _journalPathName << "\n";
    std::ifstream js(_journalPathName);
    
    do {

      Event* pEvent = eventFactory().createEventFromStream(
        [&js](char* buffer_, EventSize size_) -> bool {
          if (!js.read(buffer_, size_)) { return false; }
          return true;
        }
      );

      if (pEvent==nullptr) {
        break;
      }

      pEvent->humanReader();
      TEventFactory::releaseEvent(pEvent);

    } while (true);

    
    
    /*
    EventSize eventSize = 0;
    char buffer[100]; //TODO: 

    uint8_t offset = sizeof(EventSize);
    do {
      if(!js.read(reinterpret_cast<char*>(&eventSize), offset)) {
        break;
      }
      if (eventSize <= offset) {
        assert(false && "invalid event. readJournal");
      }
      if(!js.read(buffer+offset, eventSize-offset)) {
        break;
      }

      auto* pEvent = factory.castEvent(buffer, eventSize);
      
      pEvent->humanReader();
      
    } while (true);
     */

    js.close();
  }
  
  auto  pathName()     { return _journalPathName; }
  auto&       eventFactory()       { return *_pEventFactory; }
  const auto& eventFactory() const { return *_pEventFactory; }

private:
  std::string_view _journalPathName;
  TEventFactory* _pEventFactory;  // non-own pointer
  
};

}

#endif /* Reader_h */
