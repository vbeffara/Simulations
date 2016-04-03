/// \file
#include <vb/Hub.h>
#include <vb/Pov.h>
#include <fstream>
#include <sstream>

namespace vb {
	Pov_Object::Pov_Object (std::string s, bool b) : type(s), braces(b), commas(0) {}

	std::ostream & Pov_Object::output_pov (std::ostream & os) {
		os << type << " "; if (braces) os << "{ ";
		for (unsigned i=0; i<subs.size(); ++i) os << subs[i] << (i<commas ? ", " : " ");
		if (braces) os << "}";
		os << std::endl; return os;
	}

	void Pov_Object::output_pov (const std::string &s) {
		std::string os = H.dir + s + ".pov";
		std::ofstream f (os.c_str()); output_pov(f);
	}

	Pov_Object & Pov_Object::operator<< (tri a)   	{ return (*this) << new Pov_Coordinate (a); }
	Pov_Object & Pov_Object::operator<< (double x)	{ return (*this) << new Pov_Coefficient (x); }

	Pov_Scene::Pov_Scene () : Pov_Object ("") {
		(*this) << new Pov_Object ("#version 3.7;")
				<< new Pov_Object ("#include \"colors.inc\"")
				<< new Pov_Object ("#include \"rad_def.inc\"")
				<< new Pov_Object ("global_settings { radiosity { Rad_Settings(Radiosity_Normal,off,off) } }")
				<< new Pov_Object ("background { color White }");
	}

	Pov_Union::Pov_Union (std::string) : Pov_Object ("union", true) {}

	Pov_Texture::Pov_Texture (std::string t) : Pov_Object("texture", true) { (*this) << new Pov_Object(t); }

	Pov_Camera::Pov_Camera (tri a, tri b, double d) : Pov_Object("camera", true) {
		std::ostringstream os; os << "location " << a << " look_at " << b << " angle " << d;
		(*this) << new Pov_Object(os.str());
	}

	Pov_Light_Source::Pov_Light_Source (tri a) : Pov_Object("light_source", true) {
		(*this) << a << new Pov_Object ("color White*2", false);
	}

	Pov_Sphere::Pov_Sphere (tri a, double r, std::string) : Pov_Object("sphere", true) { (*this) << a << r; commas=1; }

	Pov_Cylinder::Pov_Cylinder (tri a, tri b, double r, std::string) : Pov_Object("cylinder", true) { (*this) << a << b << r; commas=2; }

	Pov_Box::Pov_Box (tri a, tri b, std::string) : Pov_Object("box", true) { (*this) << a << b; commas=1; }

	Pov_Plane::Pov_Plane (tri a, double d, std::string) : Pov_Object("plane", true) { (*this) << a << d; commas=1; }

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
		<< new Pov_Texture (t);
	}
}
