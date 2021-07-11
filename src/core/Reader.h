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

namespace core {

template<typename TAppTraits>
class Reader {
public:

  using TEventFunctors=typename TAppTraits::TEventFunctors;
  
  void readJournal(std::string_view journalPathName_) {

    std::ifstream js(journalPathName_);
    
    TEventFunctors functors;
    
    EventSize eventSize = 0;
    char buffer[100]; //TODO: 

    uint8_t offset = sizeof(EventSize);
    do {
      if(!js.read(reinterpret_cast<char*>(&eventSize), offset)) {
        break;
      }
      if (eventSize <= offset) {
        assert(false && "invalid event");
      }
      if(!js.read(buffer+offset, eventSize-offset)) {
        break;
      }

      auto* pEvent = core::Event::castEvent(buffer, eventSize);
      
      functors.journal(pEvent);
      auto eventName = functors.eventName(pEvent);
      std::cout << pEvent->dcSeqno() << " " << pEvent->size() << "bytes " << pEvent->eventType() << " " << eventName << "\n";

    } while (true);

    js.close();
  }
    
};

}

#endif /* Reader_h */
