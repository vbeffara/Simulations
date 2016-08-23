#pragma once /// \file
#include <vb/Picture.h>
#include <vb/coo.h>

namespace vb {
	class Coloring : public Picture { public:
		Coloring (cpx z1_, cpx z2_, int n, std::function <Color(cpx)> f_);

		void show() override;
		void show2();

		void scale (double s);
		void shift (cpx z);

		double eps;
		bool aa = true;
		cpx z1, z2;
		std::function <Color(cpx)> f;

	private:
		cpx c_to_z (coo c) const;

		Color & at (coo z) const;
		Color aa_color (coo c, bool pre = false) const;

        void line (coo s, coo d, int l);
	    void tessel (coo ul, coo lr);
        void tessel_go (coo ul, coo lr);
	    void do_aa ();

		Color * stage = nullptr;

		int handle (int event) override;
	};
}
