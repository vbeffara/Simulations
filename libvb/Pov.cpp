/// \file
#include <vb/Hub.h>
#include <vb/Pov.h>
#include <fstream>

namespace vb {
	Pov_Object::Pov_Object (std::string s, bool b) : type(std::move(s)), braces(b), commas(0) {}

	std::ostream & Pov_Object::output_pov (std::ostream & os) {
		os << type << " "; if (braces) os << "{ ";
		for (unsigned i=0; i<subs.size(); ++i) os << subs[i].get() << (i<commas ? ", " : " ");
		if (braces) os << "}";
		os << std::endl; return os;
	}

	void Pov_Object::output_pov (const std::string &s) {
		std::string os = H.dir + s + ".pov";
		std::ofstream f (os.c_str()); output_pov(f);
	}

	Pov_Object & Pov_Object::operator<< (tri a)   	{ return (*this) << std::make_unique <Pov_Coordinate> (a); }
	Pov_Object & Pov_Object::operator<< (double x)	{ return (*this) << std::make_unique <Pov_Coefficient> (x); }

	Pov_Scene::Pov_Scene () : Pov_Object ("") {
		(*this) << std::make_unique <Pov_Object> (R"("#version 3.7;")")
				<< std::make_unique <Pov_Object> (R"("#include \"colors.inc\"")")
				<< std::make_unique <Pov_Object> (R"("#include \"rad_def.inc\"")")
				<< std::make_unique <Pov_Object> (R"("global_settings { radiosity { Rad_Settings(Radiosity_Normal,off,off) } }")")
				<< std::make_unique <Pov_Object> (R"("background { color White }")");
	}

	Pov_Union::Pov_Union (const std::string &) : Pov_Object ("union", true) {}

	Pov_Texture::Pov_Texture (std::string t) : Pov_Object("texture", true) { (*this) << std::make_unique <Pov_Object> (t); }

	Pov_Camera::Pov_Camera (tri a, tri b, double d) : Pov_Object("camera", true) {
		std::string s = fmt::format ("location {} look_at {} angle {}", a, b, d);
		(*this) << std::make_unique <Pov_Object> (s);
	}

	Pov_Light_Source::Pov_Light_Source (tri a) : Pov_Object("light_source", true) {
		(*this) << a << std::make_unique <Pov_Object> ("color White*2", false);
	}

	Pov_Sphere::Pov_Sphere (tri a, double r, const std::string &) : Pov_Object("sphere", true) { (*this) << a << r; commas=1; }

	Pov_Cylinder::Pov_Cylinder (tri a, tri b, double r, const std::string &) : Pov_Object("cylinder", true) { (*this) << a << b << r; commas=2; }

	Pov_Box::Pov_Box (tri a, tri b, const std::string &) : Pov_Object("box", true) { (*this) << a << b; commas=1; }

	Pov_Plane::Pov_Plane (tri a, double d, const std::string &) : Pov_Object("plane", true) { (*this) << a << d; commas=1; }

	Pov_Frame::Pov_Frame (tri a, tri b, std::string t) : Pov_Union() { (*this)
		<< std::make_unique <Pov_Sphere> (tri(a.x,a.y,a.z), .1)
		<< std::make_unique <Pov_Sphere> (tri(a.x,a.y,b.z), .1)
		<< std::make_unique <Pov_Sphere> (tri(a.x,b.y,a.z), .1)
		<< std::make_unique <Pov_Sphere> (tri(a.x,b.y,b.z), .1)
		<< std::make_unique <Pov_Sphere> (tri(b.x,a.y,a.z), .1)
		<< std::make_unique <Pov_Sphere> (tri(b.x,a.y,b.z), .1)
		<< std::make_unique <Pov_Sphere> (tri(b.x,b.y,a.z), .1)
		<< std::make_unique <Pov_Sphere> (tri(b.x,b.y,b.z), .1)
		<< std::make_unique <Pov_Cylinder> (tri(a.x,a.y,a.z), tri(a.x,a.y,b.z), .1)
		<< std::make_unique <Pov_Cylinder> (tri(a.x,a.y,a.z), tri(a.x,b.y,a.z), .1)
		<< std::make_unique <Pov_Cylinder> (tri(a.x,a.y,a.z), tri(b.x,a.y,a.z), .1)
		<< std::make_unique <Pov_Cylinder> (tri(a.x,b.y,b.z), tri(a.x,a.y,b.z), .1)
		<< std::make_unique <Pov_Cylinder> (tri(a.x,b.y,b.z), tri(a.x,b.y,a.z), .1)
		<< std::make_unique <Pov_Cylinder> (tri(a.x,b.y,b.z), tri(b.x,b.y,b.z), .1)
		<< std::make_unique <Pov_Cylinder> (tri(b.x,a.y,b.z), tri(a.x,a.y,b.z), .1)
		<< std::make_unique <Pov_Cylinder> (tri(b.x,a.y,b.z), tri(b.x,b.y,b.z), .1)
		<< std::make_unique <Pov_Cylinder> (tri(b.x,a.y,b.z), tri(b.x,a.y,a.z), .1)
		<< std::make_unique <Pov_Cylinder> (tri(b.x,b.y,a.z), tri(a.x,b.y,a.z), .1)
		<< std::make_unique <Pov_Cylinder> (tri(b.x,b.y,a.z), tri(b.x,a.y,a.z), .1)
		<< std::make_unique <Pov_Cylinder> (tri(b.x,b.y,a.z), tri(b.x,b.y,b.z), .1)
		<< std::make_unique <Pov_Texture> (t);
	}
}
