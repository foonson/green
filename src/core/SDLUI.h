//
//  SDLApp.h
//  green
//
//  Created by Steve Cheng on 14/6/2021.
//

#ifndef SDLApp_h
#define SDLApp_h

#include <SDL.h>
#include <SDL_image.h>
#include "core/UI.h"
#include "core/Event.h"

namespace core {

template <typename TUI>
class SDLUI : public core::UI<SDLUI<TUI>> {

typedef core::UI<SDLUI<TUI>> Base;

public:
  static const bool REQUIRE_UI_THREAD=true;

public:
  
  void pollInput() {
    SDL_Event sdlevent;
    if (SDL_PollEvent(&sdlevent)==0) {
      ;
    }
  }
  
  bool initialize() {
    Base::initialize();

    int iRes = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    std::cout << "SDL_Init " << iRes << "\n";

    _pWindow = SDL_CreateWindow(
      static_cast<TUI*>(this)->windowTitle().data(),
      100, //SDL_WINDOWPOS_UNDEFINED,
      100, //SDL_WINDOWPOS_UNDEFINED,
      800,
      400,
      SDL_WINDOW_SHOWN);

    //_pRender = SDL_CreateRenderer(_pWindow, -1, SDL_RENDERER_ACCELERATED);
    _pRenderer = SDL_CreateRenderer(_pWindow, -1, 0);

    SDL_Surface* pSurface = IMG_Load("/Users/steve/green/background.jpg");
    _pTxBackground = SDL_CreateTextureFromSurface(renderer(), pSurface);
    SDL_FreeSurface(pSurface);

    return true;
  }
  
  void shutdown() {
    std::cout << "SDLUI::shutdown\n";
    SDL_Quit();
    Base::shutdown();
  }
  
  void render(core::Event* pEvent_) {

    auto pRenderer = renderer();
    SDL_RenderCopy(pRenderer, _pTxBackground, NULL, NULL);
        
    static_cast<TUI*>(this)->renderFrame(pRenderer, pEvent_);

    SDL_RenderPresent(pRenderer);
  }
  
  SDL_Texture* txBackground() { return _pTxBackground; }
  SDL_Window*        window() { return _pWindow; }
  SDL_Renderer*    renderer() { return _pRenderer; }
  
private:
  SDL_Texture*  _pTxBackground;
  SDL_Window*   _pWindow;
  SDL_Renderer* _pRenderer;
};

}
#endif /* SDLApp_h */
