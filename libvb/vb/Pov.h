#pragma once /// \file
#include <vb/common.h>

namespace vb {
    class tri { public: double x,y,z; tri (double xx, double yy, double zz) : x(xx), y(yy), z(zz) {} };
    std::ostream & operator<< (std::ostream &os, const tri &c) { return os << "<" << c.x << "," << c.y << "," << c.z << ">"; }

    class Pov_Object { public:
		virtual std::ostream & output_pov (std::ostream & os) =0;
		virtual ~Pov_Object () {};
	};
    std::ostream & operator<< (std::ostream & os, Pov_Object * o) { return o->output_pov(os) << std::endl; }

    class Pov_Camera : public Pov_Object { public: tri a,b; double d; Pov_Camera (tri aa, tri bb, double dd) : a(aa), b(bb), d(dd) {}
        std::ostream & output_pov (std::ostream & os) {
            return os << "camera { location " << a << " look_at " << b << " angle " << d << " }";
        }
    };

    class Pov_Light_Source : public Pov_Object { public: tri a; Pov_Light_Source (tri aa) : a(aa) {}
        std::ostream & output_pov (std::ostream & os) {
			return os << "light_source { " << a << " color White }";
		}
    };

    class Pov_Sphere : public Pov_Object { public: tri a; double r; Pov_Sphere (tri aa, double rr) : a(aa), r(rr) {}
        std::ostream & output_pov (std::ostream & os) {
            return os << "sphere { " << a << ", " << r << " texture { pigment { color Red } } }";
        }
    };

    class Pov_Cylinder : public Pov_Object { public: tri a,b; double r; bool c;
        Pov_Cylinder (tri aa, tri bb, double rr, bool cc = false) : a(aa), b(bb), r(rr), c(cc) {}
        std::ostream & output_pov (std::ostream & os) {
            if (c) os << new Pov_Sphere (a,r) << new Pov_Sphere (b,r);
            return os << "cylinder { " << a << ", " << b << ", " << r << " texture { pigment { color Red } } }";
        }
    };

    class Pov_Box : public Pov_Object { public: tri a,b; Pov_Box (tri aa, tri bb) : a(aa), b(bb) {}
        std::ostream & output_pov (std::ostream & os) {
            return os << "box { " << a << ", " << b << " texture { pigment { color rgb <.3,.7,1> } } }";
        }
    };

    class Pov_Plane : public Pov_Object { public: tri a; double d; Pov_Plane (tri aa, double dd) : a(aa), d(dd) {}
        std::ostream & output_pov (std::ostream & os) {
            return os	<< "plane { " << a << ", " << d
                     	<< " texture { pigment { color White } } finish { reflection {.8} ambient 0.1 diffuse 0 } }";
        }
    };

    class Pov_Frame : public Pov_Object { public: tri a,b; Pov_Frame (tri aa, tri bb) : a(aa), b(bb) {}
        std::ostream & output_pov (std::ostream & os) { return os
            << new Pov_Cylinder (tri(a.x,a.y,a.z), tri(b.x,a.y,a.z), .1, 1) << new Pov_Cylinder (tri(a.x,a.y,a.z), tri(a.x,b.y,a.z), .1, 1)
            << new Pov_Cylinder (tri(a.x,a.y,a.z), tri(a.x,a.y,b.z), .1, 1) << new Pov_Cylinder (tri(b.x,b.y,a.z), tri(b.x,a.y,a.z), .1, 1)
            << new Pov_Cylinder (tri(b.x,b.y,a.z), tri(a.x,b.y,a.z), .1, 1) << new Pov_Cylinder (tri(b.x,b.y,a.z), tri(b.x,b.y,b.z), .1, 1)
            << new Pov_Cylinder (tri(a.x,b.y,b.z), tri(a.x,b.y,a.z), .1, 1) << new Pov_Cylinder (tri(a.x,b.y,b.z), tri(a.x,a.y,b.z), .1, 1)
            << new Pov_Cylinder (tri(a.x,b.y,b.z), tri(b.x,b.y,b.z), .1, 1) << new Pov_Cylinder (tri(b.x,a.y,b.z), tri(b.x,a.y,a.z), .1, 1)
            << new Pov_Cylinder (tri(b.x,a.y,b.z), tri(a.x,a.y,b.z), .1, 1) << new Pov_Cylinder (tri(b.x,a.y,b.z), tri(b.x,b.y,b.z), .1, 1);
        }
    };

    class Pov_Scene { public: std::string title;
		Pov_Scene (std::string s) : title(s) { }
		~Pov_Scene () { foreach (Pov_Object *o,objs) delete o; }

		std::ostream & output_pov (std::ostream & os) {
			os	<< "#version 3.7;" << std::endl
			  	<< "#include \"colors.inc\"" << std::endl
			  	<< "#include \"stones.inc\"" << std::endl
			  	<< "background { color White }" << std::endl;
			foreach (Pov_Object *o, objs) os << o;
			return os;
		}

		void output_pov (const std::string &s = "") {
		    std::ostringstream os;
		    if (s == "") os << "output/" << title; else os << s;
			os << ".pov";
			std::cerr << "*** TODO: This should go in " << os.str() << std::endl;
			output_pov (std::cout);
		}

		std::vector <Pov_Object*> objs;
		Pov_Scene & operator<< (Pov_Object *o) { objs.push_back(o); return *this; }
    };

    std::ostream & operator<< (std::ostream & os, Pov_Scene &S) { return S.output_pov (os); }
}
