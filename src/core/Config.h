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
  
  enum ConfigMode {
    ConfigAsReader = 0,
    ConfigAsStore,
    ConfigAsApp
  };
  
  bool initialize(int argc_, const char * argv_[]) {
    _mode = ConfigAsApp;
    if (argc_>=2) {
      if (std::string_view("--reader")==argv_[1]) {
        _mode = ConfigAsReader;
        if (argc_>=3) {
          _journalPathName = argv_[2];
        }
      }
    }
    
    switch(_mode) {
      case ConfigAsReader: { std::cout << "ConfigAsReader\n"; break; }
      case ConfigAsStore:  { std::cout << "ConfigAsStore\n";  break; }
      case ConfigAsApp:    { std::cout << "ConfigAsApp\n";    break; }
      default:             { std::cout << "ConfigAs" << _mode << "\n"; break; }
    }
    return true;
  }
  
  bool mode() { return _mode; }
  
  bool needDropcopy() { return mode()!=ConfigAsReader; }
  
  std::string_view connectHost()     { return _connectHost; }
  std::string_view listenHost()      { return _listenHost; }
  std::string_view journalPathName() { return _journalPathName; }
  auto connectPort() { return _connectPort; }
  auto listenPort()  { return _listenPort; }

  std::string _connectHost = "127.0.0.1";
  std::string _listenHost = "127.0.0.1";
  std::string _journalPathName = "/Users/steve/green/tetris.journal";
  uint16_t    _connectPort;
  uint16_t    _listenPort;
  ConfigMode  _mode;
  
  
};
}
#endif /* Config_h */
