//
//  Queue.h
//  green
//
//  Created by Steve Cheng on 7/8/2021.
//

#ifndef Queue_h
#define Queue_h

// boost
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/lockfree/queue.hpp>

#include "core/Event.h"

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


}

#endif /* Queue_h */
