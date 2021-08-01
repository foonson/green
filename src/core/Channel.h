//
//  Channel.h
//  green
//
//  Created by Steve Cheng on 30/7/2021.
//

#ifndef Channel_h
#define Channel_h

#include "util/USocket.h"

namespace core {

class Channel {

public:
  Channel() {}

  bool initializeAsListener(std::string_view ListenHost_, uint16_t ListenPort_) {
    return _netListener.listen(ListenHost_, ListenPort_);
  }
  
  bool connect(std::string_view remoteHost_, uint16_t remotePort_) {
    return _netSender.connect(remoteHost_, remotePort_);
  }
  
  bool sendEvent(core::Event* pEvent_) {
    return _netSender.send(pEvent_, pEvent_->size());
  }
  
  bool acceptClient() {
    return _netListener.acceptClient();
  }
  
  bool recv(char* buffer_, EventSize size_) {
    long ret = _netListener.recv(buffer_, size_);
    if (ret==0)  { return false; } //util::USocket::SOCKET_RC::DISCONNECTED;
    if (ret==-1) { return false; } //util::USocket::SOCKET_RC::NO_DATA;
    return true;
  }
  
private:
  util::ServerSocket _netListener;
  util::ClientSocket _netSender; // TODO: multiple client
};

}

#endif /* Channel_h */
