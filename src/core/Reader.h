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

  using TEventFactory=typename TAppTraits::TEventFactory;
  
  void readJournal(std::string_view journalPathName_) {

    std::ifstream js(journalPathName_);
    
    TEventFactory factory;
    
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

      auto* pEvent = factory.castEvent(buffer, eventSize);
      
      pEvent->humanReader();

    } while (true);

    js.close();
  }
    
};

}

#endif /* Reader_h */
