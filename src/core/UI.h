//
//  UI.h
//  green
//
//  Created by Steve Cheng on 18/6/2021.
//

#ifndef UI_h
#define UI_h
//#include <boost/lockfree/spsc_queue.hpp>

namespace core {

template<typename TUI>
class UI {
public:
  
  // TODO: how to set capacity of queue
  //typedef boost::lockfree::spsc_queue<
  //  TUIEvent,
  //  boost::lockfree::capacity<1000>> UIEvents;

  //auto& uiEvents() { return _uiEvents; }

  bool initialize() {
    return true;
  }
  
  void shutdown()
  {
  }
  /*
  void renderFramex() {
    TUIEvent uiEvent;
    while (_uiEvents.pop(uiEvent)) {
      static_cast<TUI*>(this)->renderFrame(uiEvent);
      //std::cout << "UIQueue:" << _uiQueue.read_available() << "\n";
    }
    //std::cout << "UIQueue:" << _uiQueue.read_available() << "\n";
  }
  */
  
private:

  //UIEvents _uiEvents;

};

}

#endif /* UI_h */
