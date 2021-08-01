//
//  Evaluation.h
//  green
//
//  Created by Steve Cheng on 16/6/2021.
//

#ifndef Evaluation_h
#define Evaluation_h

// boost
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/lockfree/queue.hpp>

namespace core {
  using QueueElement=core::EventPtr;
  using EventQueue=boost::lockfree::spsc_queue
  <
    QueueElement,
    boost::lockfree::capacity<1000>  // TODO:
  >;
  using DropcopyQueue=boost::lockfree::queue
  <
    QueueElement,
    boost::lockfree::capacity<1000>  // TODO:
  >;
/*
class Evaluation {

public:
  auto&     evalEvents() { return _evalEvents;     }
  auto&       uiEvents() { return _uiEvents;       }
  auto&  contextEvents() { return _contextEvents;  }
  auto& dropcopyEvents() { return _dropcopyEvents; }

private:
  EventQueue _evalEvents;
  EventQueue _contextEvents;
  EventQueue _uiEvents;
  //EventQueue _dropcopyEvents; // Resolved TODO: this is not spsc
  DropcopyQueue _dropcopyEvents;

};
 */
}

#endif /* Evaluation_h */
