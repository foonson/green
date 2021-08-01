//
//  Config.h
//  green
//
//  Created by Steve Cheng on 3/7/2021.
//

#ifndef Config_h
#define Config_h

#include "core/includes.h"
#include <filesystem>

namespace core {
class Config {
public:
  
  enum ConfigMode {
    ConfigAsReader = 0,
    ConfigAsStore,
    ConfigAsMaster,
    ConfigAsClient
  };
  
  virtual std::string_view appName() = 0;
  
  void setDefaultJorunalPath() {

    namespace fs = std::filesystem;
    fs::path journalPath = fs::path(_rootPath) / appName();
    fs::create_directory(journalPath);

    switch (_mode) {
      case ConfigAsReader: {
        journalPath = journalPath / "master.journal" ;
        break ;
      }
      case ConfigAsStore:  {
        journalPath = journalPath / "store.journal" ;
        break ;
      }
      case ConfigAsMaster:    {
        journalPath = journalPath / "master.journal" ;
        break ;
      }
      case ConfigAsClient:    {
        journalPath = journalPath / "client.journal" ;
        break ;
      }
    }
    _journalPathName = journalPath.string();
  }
  
  bool initialize(int argc_, const char * argv_[]) {
    
    _mode = ConfigAsMaster;
    
    if (argc_>=2) {
      if (std::string_view("--reader")==argv_[1]) {
        _mode = ConfigAsReader;
      }
      
      else if (std::string_view("--store")==argv_[1]) {
        _mode = ConfigAsStore;
      }

      else if (std::string_view("--client")==argv_[1]) {
        _mode = ConfigAsClient;
      }
    }

    // Set journal pathname
    if (argc_>=3) {
      namespace fs = std::filesystem;
      auto f = fs::absolute(fs::path(argv_[2]));
      _journalPathName = f.string();
    } else {
      setDefaultJorunalPath();
    }

    if (asReader() && !std::filesystem::exists(journalPathName())) {
      std::cout << journalPathName() << " not exist\n";
      return false;
    }
    
    switch(_mode) {
      case ConfigAsReader: { std::cout << "ConfigAsReader\n"; break; }
      case ConfigAsStore:  { std::cout << "ConfigAsStore\n";  break; }
      case ConfigAsMaster: { std::cout << "ConfigAsMaster\n";    break; }
      case ConfigAsClient: { std::cout << "ConfigAsClient\n";    break; }
    }
    
    std::cout << journalPathName() << "\n";
    return true;
  }
  
  auto mode() const { return _mode; }
  bool asReader() const { return mode()==ConfigAsReader; }
  bool asStore()  const { return mode()==ConfigAsStore; }
  bool asMaster() const { return mode()==ConfigAsMaster; }
  bool asClient() const { return mode()==ConfigAsClient; }

  bool needDropcopy() const { return mode()!=ConfigAsReader; }
  
  std::string_view masterHost()      { return _masterHost; }
  std::string_view journalPathName() { return _journalPathName; }
  auto masterPort()                  { return _masterPort; }

  std::string _masterHost = "127.0.0.1";
  std::string _rootPath = "/Users/steve/green/";
  std::string _journalPathName;
  uint16_t    _masterPort;
  ConfigMode  _mode;
  
};
}
#endif /* Config_h */
