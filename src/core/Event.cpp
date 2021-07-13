//
//  Event.cpp
//  green
//
//  Created by Steve Cheng on 14/7/2021.
//


#include "Event.h"
#include "util/UCPU.h"

namespace core {

Event::Event() {
 _tick = util::UCPU::cpuTick();
}

}

