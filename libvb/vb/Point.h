/// @file
/// Definitions for the vb::Point and vb::PointQueue classes

#pragma once
#include <vb/Image.h>
#include <vb/CoarseImage.h>

namespace vb {
  class PointQueue;

  /** A stupid class for a point with a clock.
   *
   * The class is completely inlined.
   */

  class Point {
    public:
      coo z;      ///< The location of the point.
      double t;   ///< The time at which the clock rings.
      char c;     ///< The color of the point.

      /** The standard constructor of the vb::Point class.
       *
       * @param xx The first coordinate.
       * @param yy The second coordinate.
       * @param tt The time at which the clock rings.
       * @param cc The color of the point.
       */

      Point (coo zz=0, double tt=0, char cc=1) :
        z(zz), t(tt), c(cc) {};

      /** Another constructor that takes the first point out of a queue.
       *
       * @param pq The vb::PointQueue to use as a source.
       */

      Point (PointQueue &pq);

      /** Ordering of the points for the queue.
       *
       * CAUTION : the priority is the opposite of time, so pt1<pt2 means
       * pt1.t>pt2.t ...
       *
       * @param pt The vb::Point to compare to.
       * @return   Whether inequality holds.
       */

      bool operator< (const Point &pt) const { return t > pt.t; };
  };

  /** A wrapper around a priority_queue of Point elements. */

  class PointQueue : public std::priority_queue<Point> {
  public:
    /// Pick a Point from a PointQueue.
    Point &operator>> (Point &p) {
      p = top();
      pop();
      return p;
    }

    /// Add a Point to a PointQueue.
    PointQueue &operator<< (const Point &p) {
      push(p);
      return *this;
    }
  };

  Point::Point (PointQueue &pq) {
    pq >> *this;
  };

  /** Put a Point in an Image.
   *
   * This sets the point at coordinates (p.x,p.y) in img to color p.c,
   * and activates the auto-timing feature of the Image.
   *
   * @param img The vb::Image in which to put the point.
   * @param p   The point to inset.
   * @return    A reference to the modified Image (for chaining).
   */

  Image &operator<< (Image &img, const Point &p) {
    img.put (p.z,p.c);
    return img;
  }

  /** Put a Point in a CoarseImage.
   *
   * This sets the point at coordinates (p.x,p.y) in img to color p.c,
   * and activates the auto-timing feature of the Image.
   *
   * @param img The vb::CoarseImage in which to put the point.
   * @param p   The point to inset.
   * @return    A reference to the modified CoarseImage (for chaining).
   */

  CoarseImage &operator<< (CoarseImage &img, const Point &p) {
    img.put (p.z,p.c);
    return img;
  }
}
