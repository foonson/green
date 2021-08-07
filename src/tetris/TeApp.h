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
  
  static const bool REQUIRE_DROPCOPY_THREAD=true;

  bool initialize(int argc_, const char * argv_[]) {
    if (!Base::initialize(argc_, argv_)) {
      return false;
    }
    
    busCenter().initializeEval(); // TODO
    return true;
  }

};

}

#endif /* tApp_hpp */
