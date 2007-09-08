/** Classes to build nice figures.
 *
 * The main output is a .asy file (Asymptote format), later there might
 * be others, especially EPS would be nice.
 */

#ifndef __VB_FIGURE_H
#define __VB_FIGURE_H

#include <iostream>
#include <complex>
#include <list>

namespace vb {

  /** Base class for the elements of a figure.
   */

  class Shape {
    public:

      /** Empty destructor to make the compiler happy.
       */

      virtual ~Shape() {}

      /** This writes Asymptote code for the shape.
       */

      virtual std::ostream &printASY (std::ostream &os) = 0;
  };

  /** Line segment.
   */

  class Segment : public Shape {
    public:
      std::complex<double> z1, z2;

      /** Constructor from two complex numbers.
       */

      Segment (std::complex<double> zz1, std::complex<double> zz2) : z1(zz1), z2(zz2) {}

      /** Implementation of ASY output.
       */

      virtual std::ostream &printASY (std::ostream &os) {
        return os << "draw (" << z1 << "--" << z2 << ");" << std::endl;
      }
  };

  /// Circle.

  class Circle : public Shape {
    public:
      std::complex<double> z;
      double r;

      /// Constructor from center and radius.
      
      Circle (std::complex<double> zz, double rr) : z(zz), r(rr) {}

      /// Implementation of ASY output.
      
      virtual std::ostream &printASY (std::ostream &os) {
        return os << "draw (circle(" << z << "," << r << "));" << std::endl;
      }
  };

  /** The main Figure class.
   *
   * Right now it's a list of shapes with an output routine.
   */

  class Figure {
    public:

      /// The contents as a list of vb::Sahpe objects.

      std::list<Shape*> contents;

      /** Add an element to the figure.
       *
       * It transfers ownership to the Figure object.
       */
      
      Figure &add (Shape *S) {
        contents.push_back(S);
        return (*this);
      }

      /// Output as an ASY file.

      std::ostream &printASY (std::ostream &os) {
        os << "unitsize(100);" << std::endl;

        std::list<Shape*>::iterator i;
        for (i = contents.begin(); i != contents.end(); ++i) {
          (*i)->printASY(os);
        }

        return os;
      }
  };
}

#endif
