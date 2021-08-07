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
  const auto& busCenter = TeApp::app().busCenter();
  auto& ctx = TeApp::app().context();
      
  // TODO: all players
  SDL_SetRenderDrawColor(pRenderer_, 100, 100, 100, 0);
  SDL_Rect rect;
  for (core::Handle hPlayer: busCenter.allPlayers()) {
    auto xy = ctx.getXYPtr(hPlayer);
    rect.x = *std::get<0>(xy);
    rect.y = *std::get<1>(xy);
    rect.w = 100;
    rect.h = 100;
    //printf("%d %d %d %d\n", rect.x, rect.y, rect.w, rect.h);
    SDL_RenderFillRect(pRenderer_, &rect);
  }

}

std::string_view TeUI::windowTitle() {
  auto& app = tetris::TeApp::app();
  if (app.config().asMaster()) {
    return "Tetris master";
  } else {
    return "Tetris";
  }
}


}
