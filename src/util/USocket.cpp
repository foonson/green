//
//  USocket.cpp
//  green
//
//  Created by Steve Cheng on 29/6/2021.
//

// https://www.jianshu.com/p/3b233facd6bb

#include <iostream>
#include <string>
#include <string_view>

//#include <stdio.h>
//#include <stdlib.h>
#include <sys/socket.h>
//#include <sys/types.h>
#include <arpa/inet.h> // inet_addr
#include <netinet/in.h> //IPPROTO_TCP
#include <string.h> // memset
#include <unistd.h> // close
#include <fcntl.h>
#include <errno.h>

#include "USocket.h"

namespace util {

USocket::USocket() {
}

USocket::~USocket() {
}

void getSocketAddress(
  sockaddr_in& addr_,
  const std::string_view server_,
  uint16_t port_)
{
  memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = inet_addr(server_.data());
  addr_.sin_port = htons(port_);
}

bool ServerSocket::listen(const std::string_view host_, uint16_t port_) {
  _servSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (_servSocket==-1) {
    std::cerr << "Server socket failure. " << strerror(errno) << "\n" ;
    return false;
  }
  
  if (_nonBlock) {
    std::cout << "nonblock socket\n";
    int res = fcntl(_servSocket, F_SETFL, O_NONBLOCK);
    if (res<0) {
      std::cerr << "Server fcntl O_NONBLOCK failure. " << strerror(errno) << "\n" ;
      return false;
    }
  }
  
  sockaddr_in servAddr;
  getSocketAddress(servAddr, host_, port_);
  if(bind(_servSocket, (sockaddr*)&servAddr, sizeof(servAddr))==-1)
  {
    std::cerr << "bind failure. " << strerror(errno)  << "\n" ;
    close(_servSocket);
    return false;
  }

  if(::listen(_servSocket, 1)==-1) {
    std::cerr << "listen failure. "  << strerror(errno) << "\n" ;
    return false;
  }
  std::cout << "Listen " << host_ << ":" << port_ << "\n" ;
  
  return true;
}
  
bool ServerSocket::acceptClient() {

  if (_clientConnection!=-1) {
    return false;
  }
  
  sockaddr_in clntAddr;
  socklen_t iSize = 0;
  
  _clientConnection = accept(_servSocket, (sockaddr*)&clntAddr, &iSize);
  if (_clientConnection==-1) {
    return false;
  }

  char clientName[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &clntAddr.sin_addr, clientName, INET_ADDRSTRLEN);
  
  std::cout << "Accept client " << clientName << ":" << ntohs(clntAddr.sin_port) << "\n" ;
  
  return true;
}

ServerSocket::~ServerSocket() {
  close(_servSocket);
  close(_clientConnection);
  _servSocket = -1;
  _clientConnection = -1;
}

ClientSocket::~ClientSocket() {
  close(_clntSocket);
  _clntSocket = -1;
}


bool ClientSocket::connect(const std::string_view host_, uint16_t port_) {
  
  if (_clntSocket==-1)
  {
    _clntSocket = socket(AF_INET, SOCK_STREAM, 0);
  }
  
  if (_clntSocket==-1) {
    std::cerr << "Client socket failure. " << strerror(errno) << "\n";
    return false;
  }

  sockaddr_in servAddr;
  getSocketAddress(servAddr, host_, port_);
  std::cout << "Client connect " << host_ << ":" << port_ << "\n" ;
  if (::connect(_clntSocket, (sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
    std::cerr << "connect failure. " << strerror(errno) << "\n";
    close(_clntSocket);
    _clntSocket = -1;
    return false;
  }
  std::cout << "Client connected " << host_ << ":" << port_ << "\n" ;

  return true;
}

/*
USocket::SOCKET_RC ServerSocket::receiveBuffer(char** ppBuffer, BufferSize& outSize_) {

  BufferSize bufferSize = 0;
  long ret = ::recv(_clientConnection, &bufferSize, 2, 0);
  
  if (ret==0) return SOCKET_RC::DISCONNECTED;
  if (ret==-1) return SOCKET_RC::NO_DATA;

  uint8_t offset = sizeof(BufferSize);
  if (bufferSize <= offset) {
    assert(false && "invalid message");
  }

  char* buffer = new char[bufferSize];
  // *(reinterpret_cast<uint16_t*>(buffer)) = bufferSize;
  ret = ::recv(_clientConnection, buffer+offset, bufferSize-offset, 0);

  if (ret==0) {
    delete [] buffer;
    return SOCKET_RC::DISCONNECTED;
  }
  if (ret==-1) {
    delete [] buffer;
    return SOCKET_RC::NO_DATA;
  }

  outSize_ = bufferSize;
  *ppBuffer = buffer;
  return SOCKET_RC::SUCCESS;
}
*/

long ServerSocket::recv(void* buffer_, int size_, int flag_) {
  return ::recv(_clientConnection, buffer_, size_, flag_);
}

long ServerSocket::send(void* buffer_, int size_, int flag_) {
  return ::send(_clientConnection, buffer_, size_, flag_);
}

long ClientSocket::recv(void* buffer_, int size_, int flag_) {
  return ::recv(_clntSocket, buffer_, size_, flag_);
}

long ClientSocket::send(void* buffer_, int size_, int flag_) {
  return ::send(_clntSocket, buffer_, size_, flag_);
}


}
