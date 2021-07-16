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
#include "core/Reader.h"

// boost
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/lockfree/queue.hpp>

namespace core {

template<typename TApp, typename TAppTraits>
class App {
  
public:
  
  using       TContext=typename TAppTraits::TContext;
  using        TConfig=typename TAppTraits::TConfig;
  using            TUI=typename TAppTraits::TUI;
  using  TEventFactory=typename TAppTraits::TEventFactory;

  using QueueElement=core::EventPtr;
  using EventQueue=boost::lockfree::spsc_queue
  <
    QueueElement,
    boost::lockfree::capacity<1000>  // TODO:
  >;
  using DropcopyQueue=boost::lockfree::queue
  <
    QueueElement,
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

    if (!config().initialize(argc_, argv_)) {
      std::cerr << "config initialize failure!\n";
      return false;
    }

    if (config().needDropcopy()) {
      if (!dropcopy().initialize(config().journalPathName())) {
        std::cerr << "dropcopy initialize failure!\n";
        return false;
      }
    }

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

    
    if (!context().initialize()) {
      std::cerr << "context initialize failure!\n";
      return false;
    }
    
    if (!ui().initialize()) {
      std::cerr << "ui initialize failure!\n";
      return false;
    }

    netListener().listen(config().listenHost(), config().listenPort()); // TODO:

    return true;
  }
  
  void readJournal() {
    Reader<TAppTraits> reader;
    reader.readJournal(config().journalPathName());
  }
  
  void run() {
    
    // reader
    if (config().mode()==TConfig::ConfigAsReader) {
      return readJournal();
    }
    
    std::thread uiThread;
    std::thread dropcopyThread;

    if constexpr (TApp::TUI::REQUIRE_UI_THREAD) {
      uiThread = std::thread(&App::uiLoop, this);
    }

    if constexpr (TApp::REQUIRE_DROPCOPY_THREAD) {
      if (config().needDropcopy()) {
        dropcopyThread = std::thread(&App::dropcopyLoop, this);
      }
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
      if (config().needDropcopy()) {
        dropcopyThread.join();
      }
    }

  }
  
  void healthCheck(bool printSnapShot_) {
    monitor().monitorQueueLength( printSnapShot_,
                                 evalEvents().read_available(),
                                   uiEvents().read_available(),
                             /*dropcopyEvents().read_available()*/ -1,
                              contextEvents().read_available());
  }
  
  void evaluateTimer() {
    app().evaluateTimer();
  }
  
  bool forwardEvent(core::EventPtr pEvent) {
    // Forward the context/UI event from dropcopy to queue
    if (!pEvent->isFromDropcopy()) { return false ;}
    
    if (pEvent->isContextEvent()) {
      auto rc = core::push(contextEvents(), pEvent);
      if (!rc) {
        std::cerr << "Drop event - contextEvents";
        core::EventFactory::releaseEvent(pEvent);
      }
      return true;
    }

    if (pEvent->isUIEvent()) {
      auto rc = core::push(uiEvents(), pEvent);
      if (!rc) {
        std::cerr << "Drop event - uiEvent";
        core::EventFactory::releaseEvent(pEvent);
      }
      return true;
    }
    
    return false;
  }
  
  void evalLoop() {
    while (!exitFlag()) {

      if (!_evalThrottle.pass()) { continue; }

    // ui input
      ui().pollInput();
      
    // IO
      netListener().acceptClient();
      char* pBuffer = nullptr;
      EventSize eventSize = 0;
      util::USocket::SOCKET_RC rc = netListener().receiveBuffer(&pBuffer, eventSize);
      if (rc==util::USocket::SOCKET_RC::SUCCESS && pBuffer!=nullptr) {
        core::Event* pEvent = eventFactory().createEvent(pBuffer, eventSize);
        
        pEvent->isFromDropcopy(true);
        
        //std::cout << util::UCPU::cpuTick() << " ";//<< "[" << pEvent->dcSeqno() << "]: Receive " << pEvent->eventName() << " " << pEvent->size() << " bytes\n";
        //pEvent->humanReader();

        if (pEvent->isContextEvent() ||
            pEvent->isUIEvent())
        {
          auto rc = core::push(evalEvents(), pEvent);
          if (!rc) {
            std::cerr << "Drop event - evalEvents\n";
            core::EventFactory::releaseEvent(pEvent);
          }

        } else {
          std::cout << "Discard " << pEvent->eventName() << "\n";
          core::EventFactory::releaseEvent(pEvent);
        }
      }

      evaluateTimer();

      // Evaluate
      do {
        EventPtr pEvent;
        if(!evalEvents().pop(pEvent)) { break; }

        // forward event
        if (forwardEvent(pEvent)) {
          continue;
        }

        // application specific evaluation
        app().evaluate(pEvent);

        if (!pEvent->isFromDropcopy()) {
          auto rc = core::push(dropcopyEvents(), pEvent);
          if (!rc) {
            std::cerr << "Drop event - dropcopyEvents 1\n";
            core::EventFactory::releaseEvent(pEvent);
          }
        } else {
          core::EventFactory::releaseEvent(pEvent);
        }
      } while (true);
      
      // Update context
      do {
        EventPtr pEvent;
        if(!contextEvents().pop(pEvent)) { break; }
        app().updateContext(pEvent); //TODO
        
        if (!pEvent->isFromDropcopy()) {
          auto rc = core::push(dropcopyEvents(), pEvent);
          if (!rc) {
            std::cerr << "Drop event - dropcopyEvents 2\n";
            core::EventFactory::releaseEvent(pEvent);
          }

        } else {
          core::EventFactory::releaseEvent(pEvent);
        }
      } while (true);
      
      healthCheck(false);

    }
  }
  
  void uiLoop() {
    while (!exitFlag()) {
      if (!_uiThrottle.pass()) { continue; }
      
      do {
        EventPtr pEvent;
        if (!uiEvents().pop(pEvent)) { break; }
        
        if (!exitFlag()) {
          ui().render(pEvent); // TODO
        }

        // Resolved TODO: break SPSC
        if (!pEvent->isFromDropcopy()) {
          auto rc = core::push(dropcopyEvents(), pEvent);
          if (!rc) {
            std::cerr << "Drop event - dropcopyEvents 3\n";
            core::EventFactory::releaseEvent(pEvent);
          }
        } else {
          core::EventFactory::releaseEvent(pEvent);
        }

      } while(true);
        
    }
  }
  
  void dropcopyLoop() {
    
    // Server connection
    int i=0;
    while (!exitFlag()) {
      if (netSender().connect(config().connectHost(), config().connectPort()))
      {
        break;
      }
      std::cerr << "retry after 1sec [" << i << "]\n";
      util::sleep(1000);
      i++;
    }
    
    // dropcopy Loop
    while (!_exitFlag) {
      EventPtr pEvent;
      if (dropcopyEvents().pop(pEvent)) {
        dropcopy().dropcopy(pEvent, netSender());
        
        // release event
        core::EventFactory::releaseEvent(pEvent);
      }
    }
  }
  
  bool shutdown() {
    std::cout << "App::shutdown\n";
    monitor().printSummary();
    ui().shutdown();
    context().shutdown();
    if (config().needDropcopy()) { dropcopy().shutdown(); }
    
    uint64_t endTick = util::UCPU::cpuTick();
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
    app()._exitFlag = true;
  }

  void registerSignalHandler() {
    std::signal(SIGINT,  &App::signalHandler);
    std::signal(SIGTERM, &App::signalHandler);
    std::signal(SIGSEGV, &App::signalHandler);
    std::signal(SIGILL,  &App::signalHandler);
    std::signal(SIGABRT, &App::signalHandler);
    std::signal(SIGFPE,  &App::signalHandler);
    std::signal(SIGPIPE,  &App::signalHandler); // TODO
  }

  auto        exitFlag() { return _exitFlag;       }
  auto&        context() { return _context;        }
  auto&             ui() { return _ui;             }
  auto&         config() { return _config;         }
  auto&     evalEvents() { return _evalEvents;     }
  auto&       uiEvents() { return _uiEvents;       }
  auto&  contextEvents() { return _contextEvents;  }
  auto& dropcopyEvents() { return _dropcopyEvents; }
  auto&      netSender() { return _netSender;      }
  auto&    netListener() { return _netListener;    }
  auto    tickPerMilli() { return _tickPerMilli;   }
  auto&        monitor() { return _monitor;        }
  auto&       dropcopy() { return _dropcopy;       }
  auto&   eventFactory() { return _eventFactory;   }
  
private:
  bool       _exitFlag = false;
  uint64_t   _startTick;
  uint64_t   _tickPerMilli;
  util::Tick _uiThrottle;
  util::Tick _evalThrottle;

  TContext   _context;
  TUI        _ui;
  TConfig    _config;
  Dropcopy   _dropcopy;
  Monitor    _monitor;
  TEventFactory _eventFactory;

  util::ServerSocket _netListener;
  util::ClientSocket _netSender; // TODO: multiple client
  
  EventQueue _evalEvents;
  EventQueue _contextEvents;
  EventQueue _uiEvents;
  //EventQueue _dropcopyEvents; // Resolved TODO: this is not spsc
  DropcopyQueue _dropcopyEvents;


};

}

#endif /* App_h */
