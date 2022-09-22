#pragma once
#include <vb/Hypermap.h>

// Toroidal: Hypermap-based triangulation of genus 1
//
// Mode: 1 -> disks along the bones
//       2 -> disks elsewhere
//       4 -> edges along the bones
//       8 -> edges around the bones
//      16 -> edges elsewhere
//      32 -> big points at vertices (initial[i]&2)
//      64 -> circles at midpoints (initial[i]&4)
//     128 -> red circles at faces (initial[i]&8)
//     256 -> stars at faces (variation on 128)
//
// Good modes are:
//    3 -> circle packing;
//    4 -> just the bones;
//   17 -> chain of empty disks plus small triangles;
//   24 -> whole triangulation;
//  228 -> dessin (bones, vertices in black, midpoints in white, centers of faces)

namespace vb {
    class Toroidal : public Hypermap {
    public:
        explicit Toroidal(Hypermap M);

        void pack();
        void output_pdf(const std::string &s, unsigned mode);

        void flip();

        cpx m;
    };
} // namespace vb
