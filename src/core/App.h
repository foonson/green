//
//  App.h
//  green
//
//  Created by Steve Cheng on 14/6/2021.
//

#ifndef App_h
#define App_h

// std
#include "core/includes.h"
#include <thread>
#include <csignal>

// green
#include "util/UCPU.h"
#include "util/Tick.h"
#include "util/USocket.h"
#include "core/Event.h"
#include "core/Monitor.h"
#include "core/Dropcopy.h"
#include "core/Journal.h"
#include "core/BusCenter.h"
#include "core/Channel.h"

namespace core {

template<typename TApp, typename TAppTraits>
class App {
  
public:
  
  using       TContext=typename TAppTraits::TContext;
  using        TConfig=typename TAppTraits::TConfig;
  using            TUI=typename TAppTraits::TUI;
  using  TEventFactory=typename TAppTraits::TEventFactory;
  using     TBusCenter=typename TAppTraits::TBusCenter;

  static TApp& app() {
    static TApp _gapp;
    return _gapp;
  }
  
  bool recover() {
    journal().recoverFromJournal(busCenter().evalEvents(),
      [this](){
        busCenter().evalOnce();
      }
    );
    return true;
  }
  
  bool initialize(int argc_, const char * argv_[]) {
    // Start tick
    _startTick = util::cpuTick();

    // Register Signal Handler
    registerSignalHandler();

    if (!config().initialize(argc_, argv_)) {
      std::cerr << "config initialize failure!\n";
      return false;
    }

    if (!journal().initialize(
      config().journalPathName(),
      eventFactory())
    ) {
      std::cerr << "Journal initialize failure!\n";
      return false;
    }
    
    if (config().asReader()) {
      return true;
    }
    
    if (config().needDropcopy()) {
      if (!dropcopy().initialize(config().journalPathName())) {
        std::cerr << "dropcopy initialize failure!\n";
        return false;
      }
    }

    // Initialize ticker
    std::cout << "Start CPU tick:" << _startTick << "\n";
    std::cout << "coreCount:" << util::coreCount() << "\n";
    _tickPerMilli = util::calcTickPerMilli();
    _uiThrottle.intervalMicro(100000);
    _uiThrottle.tickPerMilli(_tickPerMilli);
    
    util::Tick evalThrottle;
    evalThrottle.intervalMicro(1000);
    evalThrottle.tickPerMilli(_tickPerMilli);
    std::cout << "TickPerMilli _uiTick:"  << _uiThrottle.tickPerMilli() << "\n";
    std::cout << "TickPerMilli evalTick:" << evalThrottle.tickPerMilli() << "\n";
        
    if (!context().initialize()) {
      std::cerr << "context initialize failure!\n";
      return false;
    }
    
    if (!ui().initialize()) {
      std::cerr << "ui initialize failure!\n";
      return false;
    }

    busCenter().initialize(evalThrottle, eventFactory(), config(), clientsChannel(), ui());

    auto& c = config();
    if (c.asMaster()) {
      if (!clientsChannel().initializeAsListener(c.masterHost(), c.masterPort())) {
        std::cerr << "initializeAsListener failure!\n";
        return false;
      }
    } else if (c.asClient()) {
      if(!clientsChannel().connect(config().masterHost(), config().masterPort())) {
        std::cerr << "connect failure!\n";
        return false;
      }
    }

    return true;
  }
  
  void readJournal() {
    journal().readJournal();
  }
  
  void run() {
    
    // reader
    if (config().mode()==TConfig::ConfigAsReader) {
      return readJournal();
    }

    if (!recover()) {
      std::cerr << "recover failure!\n";
      return;
    }
    busCenter().setExitFlag(false);
    
    std::thread dropcopyThread;
    std::thread evalThread;

    //if constexpr (TApp::TUI::REQUIRE_UI_THREAD) {
    //  uiThread = std::thread(&App::uiLoop, this);
    //}

    if constexpr (TApp::REQUIRE_DROPCOPY_THREAD) {
      if (config().needDropcopy()) {
        dropcopyThread = std::thread(&App::dropcopyLoop, this);
      }
    }

    evalThread = std::thread(&TBusCenter::evalLoop, &busCenter());

    this->uiLoop();

    std::cout << "Wait UILoop finish\n";
    healthCheck(true);
    //if constexpr (TApp::TUI::REQUIRE_UI_THREAD) {
      evalThread.join();
    //}
    std::cout << "Wait Dropcopy thread finish\n";
    healthCheck(true);
    if constexpr (TApp::REQUIRE_DROPCOPY_THREAD) {
      if (config().needDropcopy()) {
        dropcopyThread.join();
      }
    }
  }
  
  void healthCheck(bool printSnapShot_) {
    monitor().monitorQueueLength( printSnapShot_,
                                 busCenter().evalEvents().read_available(),
                                 busCenter().uiEvents().read_available(),
                             /*dropcopyEvents().read_available()*/ -1,
                                 busCenter().contextEvents().read_available());
  }
  
  void uiLoop() {
    while (!busCenter().exitFlag()) {
      if (!_uiThrottle.pass()) { continue; }
      
      do {
        EventPtr pEvent;
        if (!busCenter().uiEvents().pop(pEvent)) { break; }
        
        if (!busCenter().exitFlag()) {
          ui().render(pEvent); // TODO
        }

        // Resolved TODO: break SPSC
        auto rc = core::push(busCenter().dropcopyEvents(), pEvent);
        if (!rc) {
          std::cerr << "Drop event - dropcopyEvents 3\n";
          core::EventFactory::releaseEvent(pEvent);
        }

      } while(true);
    }
  }
  
  void dropcopyLoop() {
    
    // Server connection
    //int i=0;
    /*
    while (!exitFlag()) {
      if (clientsChannel().connect(config().connectHost(), config().connectPort()))
      {
        break;
      }
      std::cerr << "retry after 1sec [" << i << "]\n";
      util::sleep(1000);
      i++;
    }
     */
    
    // dropcopy Loop
    while (!busCenter().exitFlag()) {
      if (!clientsChannel().isReady()) {
        std::cerr << "Dropcopy retry after 1sec\n";
        util::sleep(1000);
        continue;
      }
      do {
        EventPtr pEvent;
        if (!busCenter().dropcopyEvents().pop(pEvent)) { break; }
        
        bool bDropcopy = dropcopy().dropcopy(pEvent, clientsChannel());
        
        // release event
        core::EventFactory::releaseEvent(pEvent);
        
        if (!bDropcopy) {
          std::cerr << "dropcopy failure\n";
          busCenter().setExitFlag(true);
          break;
        }
      } while(true);
    }
  }
  
  bool shutdown() {
    std::cout << "App::shutdown\n";
    busCenter().shutdown();
    monitor().printSummary();
    ui().shutdown();
    context().shutdown();
    if (config().needDropcopy()) { dropcopy().shutdown(); }
    
    uint64_t endTick = util::cpuTick();
    std::cout << (endTick - _startTick) / _tickPerMilli << "ms\n";
    return true;
  }
  
  static void signalHandler(int signNum_) {
    std::cout << "signalHandler signNum:" << signNum_ << "\n";
    
    switch(signNum_) {
      case SIGINT:  std::cout << "SIGINT\n";  break;
      case SIGTERM: std::cout << "SIGTERM\n"; break;
      case SIGSEGV: std::cout << "SIGSEGV\n"; break;
      case SIGILL:  std::cout << "SIGILL\n";  break;
      case SIGABRT: std::cout << "SIGABRT\n"; break;
      case SIGFPE:  std::cout << "SIGFPE\n";  break;
      case SIGPIPE: std::cout << "SIGPIPE\n"; break;
      default:      std::cout << "Signum:" << signNum_ << "\n";
    }
    
    app().healthCheck(true);
    app().busCenter().setExitFlag(true);
  }

  void registerSignalHandler() {
    std::signal(SIGINT,  &App::signalHandler);
    std::signal(SIGTERM, &App::signalHandler);
    std::signal(SIGSEGV, &App::signalHandler);
    std::signal(SIGILL,  &App::signalHandler);
    std::signal(SIGABRT, &App::signalHandler);
    std::signal(SIGFPE,  &App::signalHandler);
    
    std::signal(SIGPIPE, SIG_IGN); // TODO
  }

  auto&       config()       { return _config; }
  const auto& config() const { return _config; }

  auto    tickPerMilli() const { return _tickPerMilli; }

  auto&        context() { return _context;        }
  auto&             ui() { return _ui;             }
  auto&           busCenter()       { return _busCenter;      }
  const auto&     busCenter() const { return _busCenter;      }
  auto& clientsChannel() { return _clientsChannel;  }
  auto&        monitor() { return _monitor;        }
  auto&       dropcopy() { return _dropcopy;       }
  auto&   eventFactory() { return _eventFactory;   }
  auto&        journal() { return _journal;        }

private:
  uint64_t   _startTick;
  uint64_t   _tickPerMilli;
  util::Tick _uiThrottle;

  TContext      _context;
  TUI           _ui;
  TConfig       _config;
  Dropcopy      _dropcopy;
  Monitor       _monitor;
  Channel       _clientsChannel;
  TBusCenter    _busCenter;
  TEventFactory _eventFactory;
  Journal<TAppTraits> _journal;

};

}

#endif /* App_h */
