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
  std::string_view eventName = "";
  std::cout << util::UCPU::cpuTick() << "[" << pEvent->dcSeqno() << "]: Send " << eventName << " " << std::dec << pEvent->size() << "bytes\n";
  client_.sendBuffer(pEvent, pEvent->size());
  
  journalStream().write(pEvent->asCharBuffer(), pEvent->size());
  journalStream().flush();
  return true;
}

bool Dropcopy::shutdown() {
  journalStream().close();
  return true;
}


}
