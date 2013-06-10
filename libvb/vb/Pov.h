#pragma once /// \file
#include <vector>

namespace vb {
	class tri { public: double x,y,z; tri (double xx, double yy, double zz) : x(xx), y(yy), z(zz) {} };
	inline std::ostream & operator<< (std::ostream &os, const tri &c) { return os << "<" << c.x << "," << c.y << "," << c.z << ">"; }

	class Pov_Object {
	public:
		Pov_Object (std::string s, std::string t = "", std::string p = "", bool b = true);
		virtual ~Pov_Object ();
		virtual std::ostream & output_pov (std::ostream & os);

		Pov_Object & operator<< (tri a)        	{ pts.push_back(a);  	return *this; }
		Pov_Object & operator<< (double x)     	{ coefs.push_back(x);	return *this; }
		Pov_Object & operator<< (Pov_Object *o)	{ subs.push_back(o); 	return *this; }

		std::string type, pov; bool braces;
		std::vector<tri> pts; std::vector<double> coefs; std::vector<Pov_Object*> subs;
	};
	std::ostream & operator<< (std::ostream & os, Pov_Object * o) { return o->output_pov(os) << std::endl; }

	class Pov_Scene : public Pov_Object {
	public:
		Pov_Scene ();
		void output_pov (const std::string &s);
	};

	class Pov_Camera      	: public Pov_Object { public: Pov_Camera      	(tri a, tri b, double d);	                    	};
	class Pov_Light_Source	: public Pov_Object { public: Pov_Light_Source	(tri a);                 	                    	};
	class Pov_Union       	: public Pov_Object { public: Pov_Union       	(                        	std::string t = "");	};
	class Pov_Sphere      	: public Pov_Object { public: Pov_Sphere      	(tri a, double r,        	std::string t = "");	};
	class Pov_Cylinder    	: public Pov_Object { public: Pov_Cylinder    	(tri a, tri b, double r, 	std::string t = "");	};
	class Pov_Box         	: public Pov_Object { public: Pov_Box         	(tri a, tri b,           	std::string t = "");	};
	class Pov_Plane       	: public Pov_Object { public: Pov_Plane       	(tri a, double d,        	std::string t = "");	};
	class Pov_Frame       	: public Pov_Union  { public: Pov_Frame       	(tri a, tri b,           	std::string t = "");	};
}
