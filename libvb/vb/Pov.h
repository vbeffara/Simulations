#pragma once /// \file
#include <vb/common.h>

namespace vb {
    class Pov_Object { public: std::string type; std::vector<tri> pts; std::vector<double> coefs;
        Pov_Object (std::string s) : type(s) {};
		virtual ~Pov_Object () {};
        virtual std::ostream & output_pov (std::ostream & os);
        virtual std::ostream & pov_contents (std::ostream & os) { return os; };
	};
    std::ostream & operator<< (std::ostream & os, Pov_Object * o) { return o->output_pov(os) << std::endl; }

    class Pov_Camera : public Pov_Object { public: tri a,b; double d;
		Pov_Camera (tri aa, tri bb, double dd) : Pov_Object("camera"), a(aa), b(bb), d(dd) {}
        std::ostream & pov_contents (std::ostream & os);
    };

    class Pov_Light_Source : public Pov_Object { public:
		Pov_Light_Source (tri a) : Pov_Object("light_source") { pts.push_back(a); }
        std::ostream & pov_contents (std::ostream & os);
    };

    class Pov_Sphere : public Pov_Object { public:
		Pov_Sphere (tri a, double r) : Pov_Object("sphere") { pts.push_back(a); coefs.push_back(r); }
        std::ostream & pov_contents (std::ostream & os);
    };

    class Pov_Cylinder : public Pov_Object { public:
        Pov_Cylinder (tri a, tri b, double r) : Pov_Object("cylinder") { pts.push_back(a); pts.push_back(b); coefs.push_back(r); }
        std::ostream & pov_contents (std::ostream & os);
    };

    class Pov_Box : public Pov_Object { public:
		Pov_Box (tri a, tri b) : Pov_Object("box") { pts.push_back(a); pts.push_back(b); }
        std::ostream & pov_contents (std::ostream & os);
    };

    class Pov_Plane : public Pov_Object { public:
		Pov_Plane (tri a, double d) : Pov_Object("plane") { pts.push_back(a); coefs.push_back(d); }
        std::ostream & pov_contents (std::ostream & os);
    };

    class Pov_Frame : public Pov_Object { public: tri a,b;
		Pov_Frame (tri aa, tri bb) : Pov_Object("union"), a(aa), b(bb) {}
        std::ostream & pov_contents (std::ostream & os);
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
