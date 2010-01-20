/// @file Path.h
/// Description of the vb::Path class

#include <vb/common.h>

namespace vb {
  /// A class to describe a path in Z^2.

  class Path {
    public:
      Path (long l_, const std::vector<char> & p_, const std::string & title_);
      void printout ();
    private:
      long              l;
      std::vector<char> p;
      std::string       title;
  };
}
