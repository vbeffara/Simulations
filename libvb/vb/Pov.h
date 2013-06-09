#pragma once /// \file
#include <sstream>
#include <vector>
#include <vb/common.h> // for foreach

namespace vb {
	class tri { public: double x,y,z; tri (double xx, double yy, double zz) : x(xx), y(yy), z(zz) {} };
	inline std::ostream & operator<< (std::ostream &os, const tri &c) { return os << "<" << c.x << "," << c.y << "," << c.z << ">"; }

	class Pov_Object { public: std::string type, pov; bool braces;
		std::vector<tri> pts; std::vector<double> coefs; std::vector<Pov_Object*> subs;

		Pov_Object (std::string s, std::string t = "", std::string p = "", bool b = true) : type(s), pov(p), braces(b) {
			if (t != "") (*this) << new Pov_Object ("texture", t);
		};
		virtual ~Pov_Object () { foreach (Pov_Object *o, subs) delete o; };
		virtual std::ostream & output_pov (std::ostream & os);
		Pov_Object & operator<< (tri a)        	{ pts.push_back(a); return *this; }
		Pov_Object & operator<< (double x)     	{ coefs.push_back(x); return *this; }
		Pov_Object & operator<< (Pov_Object *o)	{ subs.push_back(o); return *this; }
	};
	std::ostream & operator<< (std::ostream & os, Pov_Object * o) { return o->output_pov(os) << std::endl; }

	class Pov_Scene : public Pov_Object { public: std::string title;
		Pov_Scene (std::string s);
		void output_pov (const std::string &s = "");
	};

	class Pov_Camera : public Pov_Object { public:
		Pov_Camera (tri a, tri b, double d) : Pov_Object("camera") {
			std::ostringstream os; os << " location " << a << " look_at " << b << " angle " << d; pov = os.str();
		}
	};

	class Pov_Light_Source : public Pov_Object { public:
		Pov_Light_Source (tri a) : Pov_Object("light_source") {
			(*this) << a << new Pov_Object ("color", "", "White*2", false);
		}
	};

	class Pov_Sphere : public Pov_Object { public:
		Pov_Sphere (tri a, double r, std::string t = "") : Pov_Object("sphere",t) { (*this) << a << r; }
	};

	class Pov_Cylinder : public Pov_Object { public:
		Pov_Cylinder (tri a, tri b, double r, std::string t = "") : Pov_Object("cylinder",t) { (*this) << a << b << r; }
	};

	class Pov_Box : public Pov_Object { public:
		Pov_Box (tri a, tri b, std::string t = "") : Pov_Object("box",t) { (*this) << a << b; }
	};

	class Pov_Plane : public Pov_Object { public:
		Pov_Plane (tri a, double d, std::string t = "") : Pov_Object("plane",t) { (*this) << a << d; }
	};

	class Pov_Union : public Pov_Object { public: Pov_Union (std::string t = "") : Pov_Object ("union",t) {} };
	class Pov_Frame : public Pov_Union { public: Pov_Frame (tri a, tri b, std::string t = ""); };
}
