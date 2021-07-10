//
//  TeUI.cpp
//  green
//
//  Created by Steve Cheng on 18/6/2021.
//

#include "TeUI.h"
#include "TeApp.h"

namespace tetris {

void TeUI::renderFrame(SDL_Renderer* pRenderer_, core::Event* pEvent_) {

  //SDL_RenderCopy(pRenderer, _pTxBackground, NULL, NULL);
  auto& rctx = TeApp::app().context();
      
  SDL_SetRenderDrawColor(pRenderer_, 100, 100, 100, 0);
  SDL_Rect rect;
  rect.x = rctx._x;
  rect.y = rctx._y;
  rect.w = 100;
  rect.h = 100;
  //printf("%d %d %d %d\n", rect.x, rect.y, rect.w, rect.h);
  SDL_RenderFillRect(pRenderer_, &rect);

}

std::string_view TeUI::windowTitle() {
  auto& app = tetris::TeApp::app();
  if (app.config().isServer()) {
    return "Tetris master";
  } else {
    return "Tetris";
  }
}


}
