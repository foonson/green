//
//  TeBusCenter.h
//  green
//
//  Created by Steve Cheng on 6/8/2021.
//

#ifndef TeBusCenter_h
#define TeBusCenter_h

#include "core/BusCenter.h"
#include "TeEvent.h"
#include "TeUI.h"
#include <array>

namespace tetris {

class TeBusCenter : public core::BusCenter<TeBusCenter, TeEventFactory, TeUI>  {

public:
  static const uint8_t PLAYER0 = 0;
  static const uint8_t PLAYER1 = 1;
  // Const
  static const uint8_t PlayerCount = 2;

  //TeBusCenter() {};
  
  bool initializeEval();
  void pollInput() ;
  void pollTimer() ;

  void evaluate(core::EventPtr pEvent);
  void updateContext(core::EventPtr pEvent) ;
  void evaluateKey(core::EventPtr pBaseEvent_) ;
  void evaluateSystem(core::EventPtr pBaseEvent_) ;

  auto  hPlayer()    const { return _hPlayer; }
  auto& allPlayers() const { return _allPlayers; }
  auto& moveTick()   const { return _moveTick; }
  bool isConcernKey(SDL_Keycode keyCode_) const;
  bool isArrowKey  (SDL_Keycode keyCode_) const;

private:
  mutable util::Tick _moveTick;
  core::Handle _hPlayer = -1; // TODO: move to Context
  std::array<core::Handle, PlayerCount> _allPlayers = {PLAYER0, PLAYER1}; // TODO: Auto initialize with PlayerCount

};
}


#endif /* TeBusCenter_h */
