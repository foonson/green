//
//  TeUI.hpp
//  green
//
//  Created by Steve Cheng on 18/6/2021.
//

#ifndef TeUI_hpp
#define TeUI_hpp

#include "core/includes.h"
#include "core/SDLUI.h"
#include "core/Event.h"

namespace tetris {

class TeUI : public core::SDLUI<TeUI> {
public:
  std::string_view windowTitle() ;
  void renderFrame(SDL_Renderer* pRenderer_, core::Event* pEvent_);
};

}

#endif /* TeUI_hpp */
