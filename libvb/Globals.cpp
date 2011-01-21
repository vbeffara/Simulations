/// @file
/// Implementation of the global state variables.

#include <vb/Globals.h>
#include <vb/PRNG.h>

namespace vb {
  void Globals::init (int argc_, char **argv_) {
    argc = argc_;
    for (int i=0; i<argc_; ++i) argv.push_back(argv_[i]);
    initialized = true;
  }

  Globals globals;

  /****** Additional global variables declared elsewhere ... ******/

  PRNG prng;
}
