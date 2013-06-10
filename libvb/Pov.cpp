/// \file
#include <vb/common.h> // foreach
#include <vb/Pov.h>
#include <fstream>

namespace vb {
	Pov_Object::Pov_Object (std::string s, std::string t, std::string p, bool b) : type(s), pov(p), braces(b) {
		if (t != "") (*this) << new Pov_Object ("texture", t);
	}

	Pov_Object::~Pov_Object () { foreach (Pov_Object *o, subs) delete o; };

	std::ostream & Pov_Object::output_pov (std::ostream & os) {
		os << type << " "; if (braces) os << "{";
		bool b=false;
		foreach (tri t, pts)     	{ os << (b ? ", " : " ") << t; b=true; }
		foreach (double d, coefs)	{ os << (b ? ", " : " ") << d; b=true; }
		foreach (Pov_Object *o, subs) os << " " << o;
		os << pov; if (braces) os << " }"; return os;
	}

	Pov_Scene::Pov_Scene () : Pov_Object ("","","",false) {
		(*this) << new Pov_Object ("#version", "", "3.7;", false)
				<< new Pov_Object ("#include", "", "\"colors.inc\"", false)
				<< new Pov_Object ("#include", "", "\"stones.inc\"", false)
				<< new Pov_Object ("#include", "", "\"rad_def.inc\"", false)
				<< new Pov_Object ("global_settings", "", " radiosity { Rad_Settings(Radiosity_Normal,off,off) }")
				<< new Pov_Object ("background", "", " color White");
	}

	void Pov_Scene::output_pov (const std::string &s) {
		std::ostringstream os; os << "output/" << s << ".pov";
		std::ofstream f (os.str().c_str()); Pov_Object::output_pov(f);
	}

	Pov_Union::Pov_Union (std::string t) : Pov_Object ("union",t) {}

	Pov_Camera::Pov_Camera (tri a, tri b, double d) : Pov_Object("camera") {
		std::ostringstream os; os << " location " << a << " look_at " << b << " angle " << d; pov = os.str();
	}

	Pov_Light_Source::Pov_Light_Source (tri a) : Pov_Object("light_source") {
		(*this) << a << new Pov_Object ("color", "", "White*2", false);
	}

	Pov_Sphere::Pov_Sphere (tri a, double r, std::string t) : Pov_Object("sphere",t) { (*this) << a << r; }

	Pov_Cylinder::Pov_Cylinder (tri a, tri b, double r, std::string t) : Pov_Object("cylinder",t) { (*this) << a << b << r; }

	Pov_Box::Pov_Box (tri a, tri b, std::string t) : Pov_Object("box",t) { (*this) << a << b; }

	Pov_Plane::Pov_Plane (tri a, double d, std::string t) : Pov_Object("plane",t) { (*this) << a << d; }

	Pov_Frame::Pov_Frame (tri a, tri b, std::string t) : Pov_Union() { (*this)
		<< new Pov_Sphere (tri(a.x,a.y,a.z), .1) << new Pov_Sphere (tri(a.x,a.y,b.z), .1) << new Pov_Sphere (tri(a.x,b.y,a.z), .1)
		<< new Pov_Sphere (tri(a.x,b.y,b.z), .1) << new Pov_Sphere (tri(b.x,a.y,a.z), .1) << new Pov_Sphere (tri(b.x,a.y,b.z), .1)
		<< new Pov_Sphere (tri(b.x,b.y,a.z), .1) << new Pov_Sphere (tri(b.x,b.y,b.z), .1)
		<< new Pov_Cylinder (tri(a.x,a.y,a.z), tri(a.x,a.y,b.z), .1) << new Pov_Cylinder (tri(a.x,a.y,a.z), tri(a.x,b.y,a.z), .1)
		<< new Pov_Cylinder (tri(a.x,a.y,a.z), tri(b.x,a.y,a.z), .1) << new Pov_Cylinder (tri(a.x,b.y,b.z), tri(a.x,a.y,b.z), .1)
		<< new Pov_Cylinder (tri(a.x,b.y,b.z), tri(a.x,b.y,a.z), .1) << new Pov_Cylinder (tri(a.x,b.y,b.z), tri(b.x,b.y,b.z), .1)
		<< new Pov_Cylinder (tri(b.x,a.y,b.z), tri(a.x,a.y,b.z), .1) << new Pov_Cylinder (tri(b.x,a.y,b.z), tri(b.x,b.y,b.z), .1)
		<< new Pov_Cylinder (tri(b.x,a.y,b.z), tri(b.x,a.y,a.z), .1) << new Pov_Cylinder (tri(b.x,b.y,a.z), tri(a.x,b.y,a.z), .1)
		<< new Pov_Cylinder (tri(b.x,b.y,a.z), tri(b.x,a.y,a.z), .1) << new Pov_Cylinder (tri(b.x,b.y,a.z), tri(b.x,b.y,b.z), .1)
		<< new Pov_Object ("texture", "", t);
	}
}
