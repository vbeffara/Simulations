#pragma once /// \file
#include <vb/Picture.h>
#include <vb/coo.h>

namespace vb {
	class Coloring : public Picture { public:
		Coloring (cpx z1_, cpx z2_, int n, std::function <Color(cpx)> f_);

		virtual void show();

		void scale (double s);
		void shift (cpx z);

		double eps;
		bool aa = false;
		cpx z1, z2;
		std::function <Color(cpx)> f;

	private:
		cpx c_to_z (coo c) const;

		Color & at (coo z) const;
		Color color (coo c) const;

        void line (coo s, coo d, int l);
        void go (coo ul, coo lr);
	    void tessel (coo ul, coo lr);

		Color * stage = nullptr;
	};
}
