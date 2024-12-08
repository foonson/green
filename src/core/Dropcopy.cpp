//
//  Dropcopy.cpp
//  green
//
//  Created by Steve Cheng on 10/7/2021.
//

#include "core/includes.h"
#include "Dropcopy.h"
#include "util/UCPU.h"
#include "core/Channel.h"
//#include "core/App.h"

namespace core {

bool Dropcopy::initialize(std::string_view journalPathName_) {
  
  auto& js = journalStream();
  js.open(journalPathName_.data(), std::ios::binary|std::ofstream::app);
  
  return true;
}

bool Dropcopy::dropcopy(core::Event* pEvent, core::Channel& clientsChannel_) {
  
  //auto tid=std::this_thread::get_id();
  pEvent->dcSeqno(_dcSeqno++);
  bool bSuccess = true;
  if (!pEvent->isFromPartner()) {
    bSuccess = clientsChannel_.sendEvent(pEvent);
    if (!bSuccess) {
      std::cerr << "sendEvent to partner failed\n";
    }
  }
  
  if (_dcSeqno%100==0) {
    std::cout << _dcSeqno << "\n";
  }
  std::cout << util::cpuTick() << " ";
  pEvent->humanReader();
  if (pEvent->eventType()==kKeyEvent12) {
    std::cout << util::cpuTick() << " ";
    ;
  }
  
  journalStream().write(pEvent->asCharBuffer(), pEvent->size());
  journalStream().flush(); // TODO: not working
  return bSuccess;
}

bool Dropcopy::shutdown() {
  journalStream().close();
  return true;
}


}
