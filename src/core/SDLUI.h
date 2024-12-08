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
#include "util/ULog.h"

namespace core {

const static SDL_Keycode KEY_UP   =1073741906;
const static SDL_Keycode KEY_DOWN =1073741905;
const static SDL_Keycode KEY_LEFT =1073741904;
const static SDL_Keycode KEY_RIGHT=1073741903;
const static SDL_Keycode KEY_ESC  =27;

template <typename TUI>
class SDLUI : public core::UI<SDLUI<TUI>> {
  typedef core::UI<SDLUI<TUI>> Base;

public:
  static const bool REQUIRE_UI_THREAD=true;
  
public:
  
  bool pollKeyboard(SDL_Keycode& keyCode_) {
    SDL_Event sdlEvent;
    if (SDL_PollEvent(&sdlEvent)==0) { return false; }
    if (sdlEvent.type!=SDL_KEYDOWN) { return false; }
    keyCode_ = sdlEvent.key.keysym.sym;
    return true;
    
  }
  
  bool initialize() {
    Base::initialize();

    int iRes = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    std::cout << "SDL_Init " << iRes << "\n";

    _pWindow = SDL_CreateWindow(
      static_cast<TUI*>(this)->windowTitle().data(),
      000, //SDL_WINDOWPOS_UNDEFINED,
      000, //SDL_WINDOWPOS_UNDEFINED,
      1000,
      1000,
      SDL_WINDOW_SHOWN);

    //_pRender = SDL_CreateRenderer(_pWindow, -1, SDL_RENDERER_ACCELERATED);
    _pRenderer = SDL_CreateRenderer(_pWindow, -1, 0);

    if (_pRenderer==nullptr) {
      std::cout << "SDL_CreateRenderer error" << "\n";
      return false;
    }

    //SDL_Surface* pSurface = IMG_Load("/Users/steve/green/background.jpg");
    SDL_Surface* pSurface = IMG_Load("/home/parallels/green/background.jpg");
    if (pSurface==nullptr) {
      std::cout << "IMG_Load error\n";
      return false;
    }

    _pTxBackground = SDL_CreateTextureFromSurface(renderer(), pSurface);
    if (_pTxBackground==nullptr) {
      std::cout << "SDL_CreateTextureFromSurface error\n";
      return false;
    }

    // TODO: RAII release
    SDL_FreeSurface(pSurface);

    return true;
  }
  
  void shutdown() {
    std::cout << "SDLUI::shutdown\n";
    SDL_Quit();
    Base::shutdown();
  }
  
  bool render(core::Event* pEvent_) {

    // TODO: handle error
    auto pRenderer = renderer();
    auto res = SDL_RenderCopy(pRenderer, _pTxBackground, NULL, NULL);
    if (res!=0) {
      util::logError("SDL_RenderCopy error");
    }
        
    bool bRet = static_cast<TUI*>(this)->renderFrame(pRenderer, pEvent_);

    SDL_RenderPresent(pRenderer);

    return true;
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
