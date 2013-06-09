/// \file
#include <vb/Pov.h>
#include <fstream>

namespace vb {
    std::ostream & Pov_Object::output_pov (std::ostream & os) {
        os << type << " "; if (braces) os << "{";
        bool b=false;
        foreach (tri t, pts)     	{ os << (b ? ", " : " ") << t; b=true; }
        foreach (double d, coefs)	{ os << (b ? ", " : " ") << d; b=true; }
        foreach (Pov_Object *o, subs) os << " " << o;
        os << pov; if (braces) os << " }"; return os;
    }

    Pov_Scene::Pov_Scene (std::string s) : Pov_Object ("","","",false), title(s) {
        (*this) << new Pov_Object ("#version", "", "3.7;", false)
                << new Pov_Object ("#include", "", "\"colors.inc\"", false)
                << new Pov_Object ("#include", "", "\"stones.inc\"", false)
                << new Pov_Object ("#include", "", "\"rad_def.inc\"", false)
                << new Pov_Object ("global_settings", "", " radiosity { Rad_Settings(Radiosity_Normal,off,off) }")
                << new Pov_Object ("background", "", " color White");
    }

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

    void Pov_Scene::output_pov (const std::string &s) {
        std::ostringstream os; if (s == "") os << "output/" << title; else os << s; os << ".pov";
        std::ofstream f (os.str().c_str()); Pov_Object::output_pov(f);
    }
}
