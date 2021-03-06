//
//  Dropcopy.hpp
//  green
//
//  Created by Steve Cheng on 10/7/2021.
//

#ifndef Dropcopy_hpp
#define Dropcopy_hpp

#include "core/Event.h"
//#include "core/Journal.h"
//#include "core/Channel.h"
#include <fstream>

namespace core {

class Channel;

class Dropcopy {
public:
  bool initialize(std::string_view journalPathName_);
  bool dropcopy(core::EventPtr pEvent, Channel& clientsChannel_);
  bool shutdown();
  
  auto& journalStream() { return _journalStream; }

private:
  
  uint32_t _dcSeqno = 0;
  std::ofstream _journalStream;

};

}

#endif /* Dropcopy_hpp */
