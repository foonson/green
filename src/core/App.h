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
#include "core/Evaluation.h"
#include "core/Channel.h"

namespace core {

template<typename TApp, typename TAppTraits>
class App {
  
public:
  
  using       TContext=typename TAppTraits::TContext;
  using        TConfig=typename TAppTraits::TConfig;
  using            TUI=typename TAppTraits::TUI;
  using  TEventFactory=typename TAppTraits::TEventFactory;

  static TApp& app() {
    static TApp _gapp;
    return _gapp;
  }
  
  bool recover() {
    journal().recoverFromJournal(evalEvents(),
      [this](){
        evalOnce();
      }
    );
    return true;
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
    setExitFlag(false);
    
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
  
  void pollTimer() {
    app().pollTimer();
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
  
  void evalOnce() {
    // Evaluate
    while(true) {
      EventPtr pEvent;
      if(!evalEvents().pop(pEvent)) { break; }

      // forward event
      if (forwardEvent(pEvent)) {
        continue;
      }

      // application specific evaluation
      app().evaluate(pEvent);

      auto rc = core::push(dropcopyEvents(), pEvent);
      if (!rc) {
        std::cerr << "Drop event - dropcopyEvents 1\n";
        core::EventFactory::releaseEvent(pEvent);
      }
    }
    
    // Update context
    while (true) {
      EventPtr pEvent;
      if(!contextEvents().pop(pEvent)) { break; }
      app().updateContext(pEvent); //TODO
      
      auto rc = core::push(dropcopyEvents(), pEvent);
      if (!rc) {
        std::cerr << "Drop event - dropcopyEvents 2\n";
        core::EventFactory::releaseEvent(pEvent);
      }
    }
  }
  
  void evalLoop() {
    while (!exitFlag()) {

      if (!_evalThrottle.pass()) { continue; }

    // ui input
      app().pollInput();
      
    // IO
      if (config().asMaster()) {
        clientsChannel().acceptClient();
      }

      core::Event* pEvent = eventFactory().createEventFromStream(
        [this](char* buffer_, EventSize size_) -> bool { return clientsChannel().recv(buffer_, size_); }
      );
        
      if (pEvent!=nullptr) {
        pEvent->isFromDropcopy(true);
        
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

      // Timer
      pollTimer();

      evalOnce();

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
        auto rc = core::push(dropcopyEvents(), pEvent);
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
    while (!_exitFlag) {
      do {
        EventPtr pEvent;
        if (!dropcopyEvents().pop(pEvent)) { break; }
        
        dropcopy().dropcopy(pEvent, clientsChannel());
        
        // release event
        core::EventFactory::releaseEvent(pEvent);
      } while(true);
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

  const auto& config() const { return _config; }
  auto&       config()       { return _config; }

  auto        exitFlag() const { return _exitFlag; }
  void     setExitFlag(bool u_=true) { _exitFlag = u_; }
  auto    tickPerMilli() const { return _tickPerMilli; }

  auto&        context() { return _context;        }
  auto&             ui() { return _ui;             }
  auto&     evalEvents() { return _evalEvents;     }
  auto&       uiEvents() { return _uiEvents;       }
  auto&  contextEvents() { return _contextEvents;  }
  auto& dropcopyEvents() { return _dropcopyEvents; }
  auto& clientsChannel() { return _clientsChannel;  }
  auto&        monitor() { return _monitor;        }
  auto&       dropcopy() { return _dropcopy;       }
  auto&   eventFactory() { return _eventFactory;   }
  auto&        journal() { return _journal;        }

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
  Journal<TAppTraits> _journal;
  Channel _clientsChannel;
  //Evaluation _evaluation;

  EventQueue _evalEvents;
  EventQueue _contextEvents;
  EventQueue _uiEvents;
  //EventQueue _dropcopyEvents; // Resolved TODO: this is not spsc
  DropcopyQueue _dropcopyEvents;

};

}

#endif /* App_h */
