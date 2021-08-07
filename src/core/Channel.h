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

  bool initializeAsListener(std::string_view listenHost_, uint16_t listenPort_) {
    bool ret = _netListener.listen(listenHost_, listenPort_);
    _isClient = false;
    //std::cout << "Master listen:" << ListenHost_ << ": connect:" << _connectPort << "\n";
    return ret;
  }
  
  bool connect(std::string_view remoteHost_, uint16_t remotePort_) {
    _isClient = true;
    return _netClient.connect(remoteHost_, remotePort_);
  }

  bool acceptClient() {
    assert(!_isClient && "client cannot acceptClient");
    return _netListener.acceptClient();
  }
  
  bool sendEvent(core::Event* pEvent_) {
    if (_isClient) {
      return _netClient.send(pEvent_, pEvent_->size());
    } else {
      return _netListener.send(pEvent_, pEvent_->size());
    }
  }
  
  bool recv(char* buffer_, EventSize size_) {
    long ret = 0;
    if (_isClient) {
      ret = _netClient.recv(buffer_, size_);
    } else {
      ret = _netListener.recv(buffer_, size_);
    }
    if (ret==0)  { return false; } //util::USocket::SOCKET_RC::DISCONNECTED;
    if (ret==-1) { return false; } //util::USocket::SOCKET_RC::NO_DATA;
    return true;
  }
  
private:
  util::ServerSocket _netListener;
  util::ClientSocket _netClient; // TODO: multiple client
  //uint16_t _randomlistenPort = 0;
  
  bool _isClient = false;
};

}

#endif /* Channel_h */
