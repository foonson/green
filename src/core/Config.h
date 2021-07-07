//
//  Config.h
//  green
//
//  Created by Steve Cheng on 3/7/2021.
//

#ifndef Config_h
#define Config_h

#include "core/includes.h"

namespace core {
class Config {
public:
  std::string_view connectHost() { return _connectHost; }
  std::string_view listenHost() { return _listenHost; }
  auto connectPort() { return _connectPort; }
  auto listenPort() { return _listenPort; }

  std::string _connectHost = "127.0.0.1";
  std::string _listenHost = "127.0.0.1";
  uint16_t _connectPort;
  uint16_t _listenPort;
};
}
#endif /* Config_h */
