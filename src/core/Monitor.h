//
//  Monitor.h
//  green
//
//  Created by Steve Cheng on 7/7/2021.
//

#ifndef Monitor_h
#define Monitor_h


namespace core {
class Monitor {
public:
  void monitorQueueLength(bool printSnapShot_,
                     uint16_t qlEval_,
                     uint16_t qlUI_,
                     uint16_t qlDropcopy_,
                     uint16_t qlContext_)
  {
    if (printSnapShot_) {
      std::cout
      << "qlEval="     << qlEval_     << "\n"
      << "qlUI="       << qlUI_       << "\n"
      << "qlDropcopy=" << qlDropcopy_ << "\n"
      << "qlContext="  << qlContext_  << "\n";
    }
    _maxQLEval     = std::max(_maxQLEval,     qlEval_);
    _maxQLUI       = std::max(_maxQLUI,       qlUI_);
    _maxQLDropcopy = std::max(_maxQLDropcopy, qlDropcopy_);
    _maxQLContext  = std::max(_maxQLContext,  qlContext_);
  }
  
  void printSummary() {
    std::cout
      << "maxQLEval="     << _maxQLEval     << "\n"
      << "maxQLUI="       << _maxQLUI       << "\n"
      << "maxQLDropcopy=" << _maxQLDropcopy << "\n"
      << "maxQLContext="  << _maxQLContext  << "\n";
  }
  
private:
  // Queue length
  uint16_t _maxQLEval     = 0;
  uint16_t _maxQLUI       = 0;
  uint16_t _maxQLDropcopy = 0;
  uint16_t _maxQLContext  = 0;
};
}
#endif /* Monitor_h */
