/// @file
/// Definitions for the global state variables.

#ifndef __VB_GLOBALS_H
#define __VB_GLOBALS_H

#include <vb/common.h>

namespace vb {
  /** A class to put global variables when necessary.
   *
   * The eventual aim is to allow for --geometry flags and 
   * auto-dimensioning of vb::Image. But I'm not quite sure how to do 
   * that right now, so this is public and write-only for the moment.
   */

  class Globals {
    public:
      void init (int argc, char **argv); ///< Store argc and argv.

      int argc;                          ///< A copy of argc.
      std::vector<std::string> argv;     ///< A copy of argv (as a vector of strings).
      bool initialized;                  ///< Whether the state of the object is valid.
  };

  extern Globals globals; ///< A global Globals instance for storing state.
}

#endif
