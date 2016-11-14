#pragma once /// \file
#include <vector>
#include <iostream>
#include <memory>

namespace vb {
	class tri { public: double x,y,z; tri (double xx, double yy, double zz) : x(xx), y(yy), z(zz) {} };
	inline std::ostream & operator<< (std::ostream &os, const tri &c) { return os << "<" << c.x << "," << c.y << "," << c.z << ">"; }

	class Pov_Object {
	public:
		Pov_Object (std::string s, bool b = false);
		Pov_Object (const Pov_Object &) = default;
		Pov_Object (Pov_Object &&) = default;

		virtual ~Pov_Object () = default;

		Pov_Object & operator= (const Pov_Object &) = default;
		Pov_Object & operator= (Pov_Object &&) = default;

		virtual std::ostream & output_pov (std::ostream & os);
		void output_pov (const std::string &s);

		Pov_Object & operator<< (tri a);
		Pov_Object & operator<< (double x);

		template <typename T> Pov_Object & operator<< (std::unique_ptr<T> && p) {
			subs.push_back (std::move(p)); return *this;
		}

		std::string type; bool braces; unsigned commas;
		std::vector <std::unique_ptr<Pov_Object>> subs;
	};
	inline std::ostream & operator<< (std::ostream & os, Pov_Object * o) { return o->output_pov(os); }

	class Pov_Coordinate : public Pov_Object { public: tri t;
		Pov_Coordinate (tri a) : Pov_Object ("coordinate"), t(a) {}
		std::ostream & output_pov (std::ostream & os) override { return os << t; }
	};

	class Pov_Coefficient : public Pov_Object { public: double v;
		Pov_Coefficient (double x) : Pov_Object ("coefficient"), v(x) {}
		std::ostream & output_pov (std::ostream & os) override { return os << v; }
	};

	class Pov_Scene       	: public Pov_Object { public: Pov_Scene       	();                      	                    	};
	class Pov_Texture     	: public Pov_Object { public: Pov_Texture     	(std::string t);         	                    	};
	class Pov_Camera      	: public Pov_Object { public: Pov_Camera      	(tri a, tri b, double d);	                    	};
	class Pov_Light_Source	: public Pov_Object { public: Pov_Light_Source	(tri a);                 	                    	};
	class Pov_Union       	: public Pov_Object { public: Pov_Union       	(                        	std::string t = "");	};
	class Pov_Sphere      	: public Pov_Object { public: Pov_Sphere      	(tri a, double r,        	std::string t = "");	};
	class Pov_Cylinder    	: public Pov_Object { public: Pov_Cylinder    	(tri a, tri b, double r, 	std::string t = "");	};
	class Pov_Box         	: public Pov_Object { public: Pov_Box         	(tri a, tri b,           	std::string t = "");	};
	class Pov_Plane       	: public Pov_Object { public: Pov_Plane       	(tri a, double d,        	std::string t = "");	};
	class Pov_Frame       	: public Pov_Union  { public: Pov_Frame       	(tri a, tri b,           	std::string t = "");	};
}
