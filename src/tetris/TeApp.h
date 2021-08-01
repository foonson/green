//
//  tApp.hpp
//  tetris
//
//  Created by Steve Cheng on 14/6/2021.
//

#ifndef tApp_hpp
#define tApp_hpp

#include "core/App.h"
#include "TeUI.h"
#include "TeContext.h"
#include "TeConfig.h"
#include "TeAppTraits.h"

#include <array>
#include <memory>

namespace tetris {

class TeApp : public core::App<TeApp, TeAppTraits > {

typedef core::App<TeApp, TeAppTraits> Base;
  
public:
  
  static const uint8_t PLAYER0 = 0;
  static const uint8_t PLAYER1 = 1;

  static const bool REQUIRE_DROPCOPY_THREAD=true;

  bool initialize(int argc_, const char * argv_[]) {
    if (!Base::initialize(argc_, argv_)) {
      return false;
    }
    if (config().asMaster()) {
      _hPlayer = PLAYER0;
    } else {
      _hPlayer = PLAYER1;
    }
    _moveTick.intervalMilli(1);
    _moveTick.tickPerMilli(tickPerMilli());
    return true;
  }

  auto  hPlayer()    const { return _hPlayer; }
  auto& allPlayers() const { return _allPlayers; }
  auto& moveTick()   const { return _moveTick; }
  bool isConcernKey(SDL_Keycode keyCode_) const;
  bool isArrowKey  (SDL_Keycode keyCode_) const;

  void pollTimer();
  void pollInput();
  void evaluate(core::EventPtr pEvent);
  void evaluateKey(core::EventPtr pEvent);
  void evaluateSystem(core::EventPtr pEvent);
  void updateContext(core::Event* pEvent);

private:
  mutable util::Tick _moveTick;
  core::Handle _hPlayer = -1; // TODO: move to Context
  std::array<core::Handle, TeAppTraits::PlayerCount> _allPlayers = {PLAYER0, PLAYER1}; // TODO: Auto initialize with PlayerCount
  
};

}

#endif /* tApp_hpp */
