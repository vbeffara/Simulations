#pragma once /// \file
#include <vb/common.h>

namespace vb {
    class Pov_Object { public:
		virtual std::ostream & output_pov (std::ostream & os) =0;
		virtual ~Pov_Object () {};
	};
    std::ostream & operator<< (std::ostream & os, Pov_Object * o) { return o->output_pov(os) << std::endl; }

    class Pov_Camera : public Pov_Object { public: tri a,b; double d;
		Pov_Camera (tri aa, tri bb, double dd) : a(aa), b(bb), d(dd) {}
        std::ostream & output_pov (std::ostream & os);
    };

    class Pov_Light_Source : public Pov_Object { public: tri a;
		Pov_Light_Source (tri aa) : a(aa) {}
        std::ostream & output_pov (std::ostream & os);
    };

    class Pov_Sphere : public Pov_Object { public: tri a; double r;
		Pov_Sphere (tri aa, double rr) : a(aa), r(rr) {}
        std::ostream & output_pov (std::ostream & os);
    };

    class Pov_Cylinder : public Pov_Object { public: tri a,b; double r; bool c;
        Pov_Cylinder (tri aa, tri bb, double rr, bool cc = false) : a(aa), b(bb), r(rr), c(cc) {}
        std::ostream & output_pov (std::ostream & os);
    };

    class Pov_Box : public Pov_Object { public: tri a,b;
		Pov_Box (tri aa, tri bb) : a(aa), b(bb) {}
        std::ostream & output_pov (std::ostream & os);
    };

    class Pov_Plane : public Pov_Object { public: tri a; double d;
		Pov_Plane (tri aa, double dd) : a(aa), d(dd) {}
        std::ostream & output_pov (std::ostream & os);
    };

    class Pov_Frame : public Pov_Object { public: tri a,b;
		Pov_Frame (tri aa, tri bb) : a(aa), b(bb) {}
        std::ostream & output_pov (std::ostream & os);
    };

    class Pov_Scene { public: std::string title; std::vector <Pov_Object*> objs;
		Pov_Scene (std::string s) : title(s) { }
		~Pov_Scene () { foreach (Pov_Object *o,objs) delete o; }

		std::ostream & output_pov (std::ostream & os);
		void output_pov (const std::string &s = "");

		Pov_Scene & operator<< (Pov_Object *o) { objs.push_back(o); return *this; }
    };

    std::ostream & operator<< (std::ostream & os, Pov_Scene &S) { return S.output_pov (os); }
}
