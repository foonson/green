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

  using   TEvent=typename TAppTraits::TEvent; // for event display

  
  void readJournal(std::string_view journalPathName_) {

    std::ifstream js(journalPathName_);
    
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
      auto* ptEvent = core::Event::castEvent<TEvent*>(pEvent);
      std::cout << ptEvent->dcSeqno() << " " << ptEvent->size() << "bytes " << ptEvent->eventType() << " " << ptEvent->name()  << "\n";

    } while (true);

    js.close();
  }
    
};

}

#endif /* Reader_h */
