//
//  TeAppTraits.h
//  green
//
//  Created by Steve Cheng on 17/7/2021.
//

#ifndef TeAppTraits_h
#define TeAppTraits_h

namespace tetris {

class TeConfig;
class TeContext;
class TeBusCenter;

struct TeAppTraits {
  using        TConfig=tetris::TeConfig;
  using       TContext=tetris::TeContext;
  using            TUI=tetris::TeUI;
  using  TEventFactory=tetris::TeEventFactory;
  using     TBusCenter=tetris::TeBusCenter;


};

}

#endif /* TeAppTraits_h */
