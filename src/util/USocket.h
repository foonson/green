//
//  USocket.hpp
//  green
//
//  Created by Steve Cheng on 29/6/2021.
//

#ifndef USocket_hpp
#define USocket_hpp

namespace util {

class USocket {
public:
  enum SOCKET_RC : uint8_t {
    SUCCESS = 0,
    NO_DATA,
    DISCONNECTED,
  }; // TODO: deco

protected:
  bool _nonBlock = true;
  USocket() = default;
  ~USocket() = default;

};

class ServerSocket : public USocket {
public:
  ~ServerSocket();
  bool listen(const std::string_view host_, uint16_t port_);
  bool acceptClient();
  long recv(void* buffer_, int size_, int flag_=0);
  long send(void* buffer_, int size_, int flag_=0);

private:
  int _servSocket = -1;
  int _clientConnection = -1; // TODO: multiple client connection

};

class ClientSocket : public USocket {
public:
  ~ClientSocket();
  bool connect(const std::string_view host_, uint16_t port_);
  long recv(void* buffer_, int size_, int flag_=0);
  long send(void* buffer_, int size_, int flag_=0);

private:
  int _clntSocket = -1;

};

}

#endif /* USocket_hpp */
