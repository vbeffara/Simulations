/// @file Path.h
/// Description of the vb::Path class

#include <vb/common.h>

namespace vb {
  /// A class to describe a path in Z^2.

  class Path : public std::vector<char> {
    public:
      Path (int l, const std::string & title_);
      void printout ();
    private:
      std::string       title;
  };
}
