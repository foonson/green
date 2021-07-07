//
//  main.cpp
//  tetris
//
//  Created by Steve Cheng on 14/6/2021.
//

#include "TeApp.h"

int main(int argc_, const char * argv_[]) {    
  auto& app = tetris::TeApp::app();
  app.initialize(argc_, argv_);
  app.run();
  app.shutdown();
  return 0;
}

