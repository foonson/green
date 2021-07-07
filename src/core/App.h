//
//  App.h
//  green
//
//  Created by Steve Cheng on 14/6/2021.
//

#ifndef App_h
#define App_h

#include "core/includes.h"
#include <thread>
#include <csignal>

#include "util/UCPU.h"
#include "util/Tick.h"
#include "util/USocket.h"
#include "core/Event.h"
#include "core/Monitor.h"

// boost
#include <boost/lockfree/spsc_queue.hpp>

namespace core {

template<typename TApp, typename TAppTraits>
class App {
    
public:
  
  using TContext=typename TAppTraits::TContext;
  using  TConfig=typename TAppTraits::TConfig;
  using      TUI=typename TAppTraits::TUI;

  using EventQueue=boost::lockfree::spsc_queue
  <
    core::EventSPtr,
    boost::lockfree::capacity<1000>  // TODO:
  >;
  
  static TApp& app() {
    static TApp _gapp;
    return _gapp;
  }
  
  bool initialize(int argc_, const char * argv_[]) {
    // Start tick
    _startTick = util::UCPU::cpuTick();

    // Register Signal Handler
    registerSignalHandler();

    // Initialize ticker
    std::cout << "Start CPU tick:" << _startTick << "\n";
    std::cout << "coreCount:" << util::UCPU::coreCount() << "\n";
    _tickPerMilli = util::UCPU::calcTickPerMilli();
    _uiThrottle.intervalMicro(100);
    _uiThrottle.tickPerMilli(_tickPerMilli);
    _evalThrottle.intervalMicro(100);
    _evalThrottle.tickPerMilli(_tickPerMilli);
    std::cout << "TickPerMilli _uiTick:"   << _uiThrottle.tickPerMilli() << "\n";
    std::cout << "TickPerMilli _evalTick:" << _evalThrottle.tickPerMilli() << "\n";

    if (!config().initialize(argc_, argv_)) {
      return false;
    }
    
    if (!context().initialize()) {
      return false;
    }
    
    if (!ui().initialize()) {
      return false;
    }

    netServer().listen(config().listenHost(), config().listenPort()); // TODO:

    return true;
  }
  
  void run() {
    
    std::thread uiThread;
    std::thread dropcopyThread;

    if constexpr (TApp::TUI::REQUIRE_UI_THREAD) {
      uiThread = std::thread(&App::uiLoop, this);
    }

    if constexpr (TApp::REQUIRE_DROPCOPY_THREAD) {
      dropcopyThread = std::thread(&App::dropcopyLoop, this);
    }

    evalLoop();

    std::cout << "Wait UI thread finish\n";
    healthCheck(true);
    if constexpr (TApp::TUI::REQUIRE_UI_THREAD) {
      uiThread.join();
    }
    std::cout << "Wait Dropcopy thread finish\n";
    healthCheck(true);
    if constexpr (TApp::REQUIRE_DROPCOPY_THREAD) {
      dropcopyThread.join();
    }

  }
  
  void healthCheck(bool printSnapShot_) {
    monitor().monitorQueueLength( printSnapShot_,
                                 evalEvents().read_available(),
                                   uiEvents().read_available(),
                             dropcopyEvents().read_available(),
                              contextEvents().read_available());
  }
  
  void evaluateTimer() {
    app().evaluateTimer();

  }
  
  void evalLoop() {
    while (!_exitFlag) {

      if (!_evalThrottle.pass()) { continue; }

    // ui input
      ui().pollInput();
      
    // IO
      netServer().acceptClient();
      void* pBuffer = nullptr;
      util::USocket::SOCKET_RC rc = netServer().receiveBuffer(&pBuffer);
      if (rc==util::USocket::SOCKET_RC::SUCCESS && pBuffer!=nullptr) {
        core::Event* pEvent = static_cast<core::Event*>(pBuffer);
        pEvent->isFromDropcopy(true);

        std::cout << util::UCPU::cpuTick() << ": Receive " << std::hex << pEvent->eventType() << " " << std::dec << pEvent->size() << "bytes\n";

        
        if (pEvent->isContextEvent()) {
          evalEvents().push(EventSPtr(pEvent));
        } else {
          std::cout << "Discard " << pEvent->eventType() << "\n";
        }
      }

      evaluateTimer();

      // Evaluate
      do {
        EventSPtr pEvent;
        if(!evalEvents().pop(pEvent)) { break; }
        app().evaluate(pEvent);

        if (!pEvent->isFromDropcopy()) {
          dropcopyEvents().push(pEvent);
        }
      } while (true);
      
      // Update context
      do {
        EventSPtr pEvent;
        if(!contextEvents().pop(pEvent)) { break; }
        context().updateContext(pEvent.get());
        
        if (!pEvent->isFromDropcopy()) {
          dropcopyEvents().push(pEvent);
        }
      } while (true);
      
      healthCheck(false);

    }
  }
  
  void uiLoop() {
    while (!_exitFlag) {
      if (!_uiThrottle.pass()) { continue; }
      
      do {
        EventSPtr pEvent;
        if (!uiEvents().pop(pEvent)) { break; }
        ui().render(pEvent.get());
      } while(true);
        
    }
  }
  
  void dropcopyLoop() {
    
    // Server connection
    int i=0;
    while (!_exitFlag) {
      if (netClient().connect(config().connectHost(), config().connectPort()))
      {
        break;
      }
      std::cerr << "retry after 1sec [" << i << "]\n";
      util::sleep(1000);
      i++;
    }
    
    // dropcopy Loop
    while (!_exitFlag) {
      EventSPtr pEvent;
      if (dropcopyEvents().pop(pEvent)) {
        app().dropcopy(pEvent.get());
      }
    }
  }

  void dropcopy(core::Event* pEvent) {
    std::cout << util::UCPU::cpuTick() << ": Send " << std::hex << pEvent->eventType() << " " << std::dec << pEvent->size() << "bytes\n";
    netClient().sendBuffer(pEvent, pEvent->size());
  }

  
  bool shutdown() {
    std::cout << "App::shutdown\n";
    monitor().printSummary();
    ui().shutdown();
    context().shutdown();
    return true;
  }
  
  static void signalHandler(int signNum_) {
    std::cout << "signalHandler signNum:" << signNum_ << "\n";
    app()._exitFlag = true;
  }

  void registerSignalHandler() {
    std::signal(SIGINT,  &App::signalHandler);
    std::signal(SIGTERM, &App::signalHandler);
    std::signal(SIGSEGV, &App::signalHandler);
    std::signal(SIGILL,  &App::signalHandler);
    std::signal(SIGABRT, &App::signalHandler);
    std::signal(SIGFPE,  &App::signalHandler);

    std::signal(SIGPIPE,  &App::signalHandler);

  }

  auto&        context() { return _context;        }
  auto&             ui() { return _ui;             }
  auto&         config() { return _config;         }
  auto&     evalEvents() { return _evalEvents;     }
  auto&       uiEvents() { return _uiEvents;       }
  auto&  contextEvents() { return _contextEvents;  }
  auto& dropcopyEvents() { return _dropcopyEvents; }
  auto&      netClient() { return _netClient;      }
  auto&      netServer() { return _netServer;      }
  auto    tickPerMilli() { return _tickPerMilli;   }
  auto&        monitor() { return _monitor;        }
  
private:
  bool       _exitFlag = false;
  TContext   _context;
  TUI        _ui;
  TConfig    _config;
  uint64_t   _startTick;
  util::Tick _uiThrottle;
  util::Tick _evalThrottle;
  uint64_t   _tickPerMilli;
  
  util::ServerSocket _netServer;
  util::ClientSocket _netClient; // TODO: multiple client

  EventQueue _evalEvents;
  EventQueue _contextEvents;
  EventQueue _uiEvents;
  EventQueue _dropcopyEvents;
  
  Monitor _monitor;

};

}

#endif /* App_h */
