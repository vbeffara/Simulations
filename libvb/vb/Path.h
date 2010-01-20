/// @file Path.h
/// Description of the vb::Path class

#include <vb/common.h>

namespace vb {
  /** A class to describe a path in Z^2.
   *
   * The path is implemented in terms of turns, each location in the 
   * vector being 0 for going straight, 1 for turning left, -1 for 
   * turning right and 2 for doing a half-turn (all of this understood 
   * as mod 4). This is to facilitate things like the pivot algorithm 
   * for random walks.
   */

  class Path : public std::vector<char> {
    public:
      /** Create a vb::Path of a given length.
       *
       * @param l      The length of the path.
       * @param title_ The title of the generated image.
       */

      Path (int l, const std::string & title_);

      /** Output the path as an image.
       *
       * This outputs a representation of the path as an EPS file to the 
       * standard output. This is a remainder of previous times and 
       * should be updated to PDF.
       *
       * @todo Change that to a PDF generated through vb::Figure.
       */

      void printout      () const;

      /** Check whether the path is self-avoiding. */

      bool self_avoiding ();

    private:
      std::string       title;
  };
}
