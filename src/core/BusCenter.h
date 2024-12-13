//
//  Evaluation.h
//  green
//
//  Created by Steve Cheng on 16/6/2021.
//

#ifndef Evaluation_h
#define Evaluation_h

#include "core/Event.h"
#include "core/Queue.h"
#include "core/Channel.h"
#include "core/Config.h"
#include "util/Tick.h"

namespace core {

template <typename TBusCenter, typename TEventFactory, typename TUI>
class BusCenter {

public:

  bool initialize(
                  const util::Tick& evalThrottle_,
                  TEventFactory&    eventFactory_,
                  core::Config&     config_,
                  core::Channel&    masterClientsChannel_,
                  TUI&              ui_)
  {
    _evalThrottle  = evalThrottle_;
    _pEventFactory = &eventFactory_;
    _pConfig       = &config_;
    _pMasterClientsChannel = &masterClientsChannel_;
    _pUI           = &ui_;
    return true;
  }
  
  bool shutdown() {
    return true;
  }
  
  TBusCenter& appBusCenter() { return *(static_cast<TBusCenter*>(this)); }
  
  bool forwardEvent(core::EventPtr pEvent) {
    // Forward the context/UI event from dropcopy to queue
    if (!pEvent->isFromPartner()) { return false ;}
    
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

      // forward event if event is from dropcopy
      if (pEvent->isFromPartner()) {
        if (forwardEvent(pEvent)) {
          continue;
        }
      }

      // application specific evaluation
      appBusCenter().evaluate(pEvent);

      auto rc = core::push(dropcopyEvents(), pEvent);
      if (!rc) {
        std::cerr << "Drop event - dropcopyEvents evalOnce\n";
        core::EventFactory::releaseEvent(pEvent);
      }
    }
    
    // Update context
    while (true) {
      EventPtr pEvent;
      if(!contextEvents().pop(pEvent)) { break; }
      appBusCenter().updateContext(pEvent); //TODO
      
      auto rc = core::push(dropcopyEvents(), pEvent);
      if (!rc) {
        //pEvent->humanReader();
        std::cerr << "Drop event - dropcopyEvents evalOnce 2\n";
        core::EventFactory::releaseEvent(pEvent);
      }
    }
  }
  
  void evalLoop() {
    while (!exitFlag()) {

      if (!_evalThrottle.pass()) { continue; }

    // ui input
      appBusCenter().pollInput();
      
    // IO
      if (config().asMaster()) {
        masterClientsChannel().acceptClient();
      }

      // Fetch and process event from MasterClient channel
      core::Event* pEvent = eventFactory().createEventFromStream(
        [this](char* buffer_, EventSize size_) -> bool { return masterClientsChannel().recv(buffer_, size_); }
      );
        
      if (pEvent!=nullptr) {
        pEvent->isFromPartner(true);
        
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
      appBusCenter().pollTimer();

      evalOnce();

      //healthCheck(false);  TODO

    }
  }

  auto        exitFlag() const { return _exitFlag; }
  void     setExitFlag(bool u_=true) { _exitFlag = u_; }

  auto&     evalEvents() { return _evalEvents;     }
  auto&       uiEvents() { return _uiEvents;       }
  auto&  contextEvents() { return _contextEvents;  }
  auto& dropcopyEvents() { return _dropcopyEvents; }

  auto&       eventFactory() { return *_pEventFactory; }
  const auto& eventFactory() const { return *_pEventFactory; }
  auto&       config()             { return *_pConfig; }
  const auto& config()       const { return *_pConfig; }
  auto&       ui()                 { return *_pUI; }
  const auto& ui()           const { return *_pUI; }
  auto&       evalThrottle()       { return _evalThrottle; }
  const auto& evalThrottle() const { return _evalThrottle; }

  auto& masterClientsChannel() { return *_pMasterClientsChannel; }


private:
  bool       _exitFlag = false;
  EventQueue _evalEvents;
  EventQueue _contextEvents;
  EventQueue _uiEvents;
  DropcopyQueue _dropcopyEvents;

  util::Tick _evalThrottle;

  // non-own pointers
  TEventFactory* _pEventFactory;
  core::Config*  _pConfig;
  core::Channel* _pMasterClientsChannel;
  TUI*           _pUI;
  

};

}

#endif /* Evaluation_h */
