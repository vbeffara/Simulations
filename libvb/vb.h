
/// @file vb.h
/// Include everything from the libvb library.

#include <vb/config.h>

#include <vb/Image.h>
#include <vb/CoarseImage.h>
#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/Point.h>

#include <string>

/** The namespace where libvb puts everything it defines. */

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

  int printout_path (char *p, long l, std::string title);
}
