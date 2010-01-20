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
       * This uses the default format (i.e., PDF), and the file name 
       * defaults to the title of the vb::Path opbject.
       *
       * @param s The filename of the output.
       */

      void output (const std::string &s = "") const;

      /** Output the path as a PDF file.
       *
       * @param s The filename of the output.
       */

      void output_pdf (const std::string &s = "") const;

      /** Check whether the path is self-avoiding. */

      bool self_avoiding ();

    private:
      std::string       title;
  };
}
