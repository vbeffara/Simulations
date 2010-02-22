
/// @file globals.h
/// Definitions for the global state variables.

#ifndef __VB_GLOBALS_H
#define __VB_GLOBALS_H

#include <vb/common.h>

namespace vb {
  class Globals {
    public:
      void init (int argc, char **argv);

      int argc;
      std::vector<std::string> argv;
      bool initialized;
  };

  extern Globals globals; ///< A global Globals instance for storing state.
}

#endif
