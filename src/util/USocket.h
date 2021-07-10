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
  };

protected:
  bool _nonBlock = true;
  USocket();
  ~USocket();

};

class ServerSocket : public USocket {
public:
  ~ServerSocket();
  bool listen(const std::string_view host_, uint16_t port_);
  bool acceptClient();
  SOCKET_RC receiveBuffer(char** ppBuffer);
  
private:
  int _servSocket = -1;
  int _clientConnection = -1; // TODO: multiple client connection

};

class ClientSocket : public USocket {
public:
  ~ClientSocket();
  bool connect(const std::string_view host_, uint16_t port_);
  bool sendBuffer(void* buffer_, uint16_t size_);

private:
  int _clntSocket = -1;

};

}

#endif /* USocket_hpp */
