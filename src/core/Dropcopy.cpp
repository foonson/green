//
//  Dropcopy.cpp
//  green
//
//  Created by Steve Cheng on 10/7/2021.
//

#include "core/includes.h"
#include "Dropcopy.h"
#include "util/UCPU.h"
#include "util//USocket.h"
#include "core/App.h"

namespace core {

bool Dropcopy::initialize(std::string_view journalPathName_) {
  
  auto& js = journalStream();
  js.open(journalPathName_.data(), std::ios::binary);
  
  return true;
}

bool Dropcopy::dropcopy(core::Event* pEvent, util::ClientSocket& client_) {
  
  //auto tid=std::this_thread::get_id();
  pEvent->dcSeqno(_dcSeqno++);
  client_.sendBuffer(pEvent, pEvent->size());
  
  if (_dcSeqno%100==0) {
    std::cout << _dcSeqno << "\n";
  }
  //std::cout << util::UCPU::cpuTick() << " ";
  //pEvent->humanReader();
  
  journalStream().write(pEvent->asCharBuffer(), pEvent->size());
  journalStream().flush(); // TODO: not working
  return true;
}

bool Dropcopy::shutdown() {
  journalStream().close();
  return true;
}


}
