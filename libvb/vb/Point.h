/// @file Point.h
/// Definitions for the vb::Point and vb::PointQueue classes

#ifndef __VB_POINT_H
#define __VB_POINT_H

#include <vb/CoarseImage.h>

namespace vb {
  class PointQueue;

  /** A stupid class for a point with a clock. 
   *
   * The class is completely inlined.
   */

  class Point {
    public:
      int x;      ///< The first coordinate.
      int y;      ///< The second coordinate.
      double t;   ///< The time at which the clock rings.
      char c;     ///< The color of the point.

      /** The standard constructor of the vb::Point class.
       *
       * @param xx The first coordinate.
       * @param yy The second coordinate.
       * @param tt The time at which the clock rings.
       * @param cc The color of the point.
       */

      Point (int xx, int yy, double tt, char cc=1) : 
        x(xx), y(yy), t(tt), c(cc) {};

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
       */

      bool operator< (const Point &pt) const { return t > pt.t; };
  };

  /** A wrapper around a priority_queue of Point elements. */

  class PointQueue {
    protected:
      std::priority_queue<Point> q;   ///< The std::priority_queue itself.

      friend inline PointQueue &operator>> (PointQueue &, Point &);
      friend inline PointQueue &operator<< (PointQueue &pq, const Point &p);
  };

  /** Put a Point in a PointQueue.
   * 
   * @param pq The vb::PointQueue in which to put the point.
   * @param p  The vb::Point to insert.
   */

  inline PointQueue &operator<< (PointQueue &pq, const Point &p) {
    pq.q.push(p);
    return pq;
  }

  /** Take a Point from a PointQueue.
   *
   * @param pq The vb::PointQueue from which to extract the point.
   * @param p  The vb::Point to fill with the extractd point.
   */

  inline PointQueue &operator>> (PointQueue &pq, Point &p) {
    p = pq.q.top();
    pq.q.pop();
    return pq;
  }

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
   */

  inline Image &operator<< (Image &img, const Point &p) {
    img.putpoint (p.x,p.y,p.c);
    return img;
  }

  /** Put a Point in a CoarseImage.
   *
   * This sets the point at coordinates (p.x,p.y) in img to color p.c,
   * and activates the auto-timing feature of the Image.
   *
   * @param img The vb::CoarseImage in which to put the point.
   * @param p   The point to inset.
   */

  inline CoarseImage &operator<< (CoarseImage &img, const Point &p) {
    img.putpoint (p.x,p.y,p.c);
    return img;
  }
}

#endif
