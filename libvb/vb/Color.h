/// @file Color.h
/// Adapter class to fit in between Cairo and FLTK.

#ifndef __VB_COLOR_H
#define __VB_COLOR_H

#ifdef BYTE_ORDER
# if BYTE_ORDER == BIG_ENDIAN
#  define VB_BIG_ENDIAN
# else
#  define VB_LITTLE_ENDIAN
# endif
#else
# if defined(__sparc__)
#  define VB_BIG_ENDIAN
# else
Error: unknown endianness.
# endif
#endif

namespace vb {
  /** A rather stupid class holding color info in Cairo format.
   *
   * It is just a 4-char structure with a few convenience methods for 
   * making it out of components, and it adapts to the endianness of the 
   * machine as determined at compilation time.
   */

  class Color {
    public:
#ifdef VB_LITTLE_ENDIAN
      char b; ///< The blue component.
      char g; ///< The green component.
      char r; ///< The red component.
      char a; ///< The alpha channel (for ARGB32).

      /// Constructor from RGBA values.
      Color (char R, char G, char B, char A=255) : b(B), g(G), r(R), a(A) { }

      /// Constructor from a greyscale value.
      Color (char V = 0) : b(V), g(V), r(V), a(255) { }
#else
      char a; ///< The alpha channel (for ARGB32).
      char r; ///< The red component.
      char g; ///< The green component.
      char b; ///< The blue component.

      /// Constructor from RGBA values.
      Color (char R, char G, char B, char A=255) : a(A), r(R), g(G), b(B) { }

      /// Constructor from a greyscale value.
      Color (char V = 0) : a(255), r(V), g(V), b(V) { }
#endif
      /// Compare to another color.
      bool operator== (const Color &o) { return (r==o.r) && (g==o.g) && (b==o.b) && (a==o.a); }
      /// Compare to another color.
      bool operator!= (const Color &o) { return ! operator==(o); }
      /// Convert to greyscale as an integer.
      operator int () { return (r+g+b)/3; }
  };
}

#endif // __VB_COLOR_H
