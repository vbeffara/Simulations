/// @file
/// Implementation of the vb::ProgressBar class

#include <vb/ProgressBar.h>

namespace vb {
  ProgressBar::ProgressBar (int length, double pow) :
    AutoWindow(0,0,"Progress Bar"), final(length), current(0), nchar(0), power(pow) { }

  ProgressBar::~ProgressBar () {
    set(final);
    update();
    std::cerr << "\n";
  }

  void ProgressBar::set (int pos) {
    if (pos<0) pos=0;
    if (pos>final) pos=final;
    current = pos;
    global_clock.step();
  }

  void ProgressBar::update () {
    std::cerr << "\r[";
    nchar = (current*50.0)/final;
    for (int i=0; i<nchar; ++i)  std::cerr << "=";
    for (int i=nchar; i<50; ++i) std::cerr << " ";
    std::cerr << "]";

    if (global_clock.time() > 2) {
      double done = pow(current,power), todo = pow(final,power);
      int    eta  = global_clock.time() * (todo/done - 1.0);
      bool   big  = false;

      std::cerr << " (";
      if (eta >= 3600*24) {
        std::cerr << eta/3600/24 << "d ";
        eta = eta % 3600*24;
        big = true;
      }
      if (big || (eta >= 3600)) {
        std::cerr << eta/3600 << ":" << std::setw(2) << std::setfill('0');
        eta = eta % 3600;
      }
      std::cerr << eta/60 << ":" << std::setw(2) << std::setfill('0') << eta%60 << ")   ";
    }
  }
}
