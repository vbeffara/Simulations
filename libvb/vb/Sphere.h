#pragma once
#include <vb/Coloring.h>

namespace vb {
    class Sphere : public Coloring {
    public:
        Sphere (int w, const std::function <Color(double,double,double)> &f); // Cartesian coordinates
        Sphere (int w, const std::function <Color(double,double)> &f);        // Polar coordinates

        void show () override;

    private:
        Sphere (int w, std::function <Color(cpx)> f);
    };
}
