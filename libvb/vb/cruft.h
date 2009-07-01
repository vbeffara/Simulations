/// @file cruft.h
/// Some remainder from a previous iteration of the package.

#include <vb/common.h>

namespace vb {
  /** Makes an EPS file out of a path in Z^2.
   *
   * The path is stored as an array of chars, taking values between 0
   * and 3. The path is clipped automatically.
   *
   * @param p The path data.
   * @param l The length of the path.
   * @param title The title of the created image.
   */

  int printout_path (char *p, long l, const std::string &title);
}
