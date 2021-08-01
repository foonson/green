//
//  main.cpp
//  tetris
//
//  Created by Steve Cheng on 14/6/2021.
//

#include "TeApp.h"

int main(int argc_, const char * argv_[]) {    
  auto& app = tetris::TeApp::app();
  if (!app.initialize(argc_, argv_)) {
    std::cout << "app initialize failure!";
    return 0;
  }
  app.run();
  app.shutdown();
  return 0;
}

