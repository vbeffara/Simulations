/// \file
#include <vb/Pov.h>
#include <fstream>

namespace vb {
    std::ostream & Pov_Object::output_pov (std::ostream & os) {
        os << type << " { ";
        bool b=false;
        foreach (tri t, pts)     	{ if (b) os << ", "; os << t; b=true; }
        foreach (double d, coefs)	{ if (b) os << ", "; os << d; b=true; }
        pov_contents(os);
        if (texture != "") os << " texture { " << texture << " }";
        return os << " }";
    }

    std::ostream & Pov_Union::pov_contents       	(std::ostream & os) { foreach (Pov_Object *o, objs) os << o; return os; }
    std::ostream & Pov_Camera::pov_contents      	(std::ostream & os)	{ return os << "location " << a << " look_at " << b << " angle " << d; }
    std::ostream & Pov_Light_Source::pov_contents	(std::ostream & os)	{ return os << " color White*2"; }

	Pov_Frame::Pov_Frame (tri a, tri b, std::string t) : Pov_Union(t) { (*this)
        << new Pov_Sphere (tri(a.x,a.y,a.z), .1) << new Pov_Sphere (tri(a.x,a.y,b.z), .1) << new Pov_Sphere (tri(a.x,b.y,a.z), .1)
        << new Pov_Sphere (tri(a.x,b.y,b.z), .1) << new Pov_Sphere (tri(b.x,a.y,a.z), .1) << new Pov_Sphere (tri(b.x,a.y,b.z), .1)
        << new Pov_Sphere (tri(b.x,b.y,a.z), .1) << new Pov_Sphere (tri(b.x,b.y,b.z), .1)
        << new Pov_Cylinder (tri(a.x,a.y,a.z), tri(a.x,a.y,b.z), .1) << new Pov_Cylinder (tri(a.x,a.y,a.z), tri(a.x,b.y,a.z), .1)
        << new Pov_Cylinder (tri(a.x,a.y,a.z), tri(b.x,a.y,a.z), .1) << new Pov_Cylinder (tri(a.x,b.y,b.z), tri(a.x,a.y,b.z), .1)
        << new Pov_Cylinder (tri(a.x,b.y,b.z), tri(a.x,b.y,a.z), .1) << new Pov_Cylinder (tri(a.x,b.y,b.z), tri(b.x,b.y,b.z), .1)
        << new Pov_Cylinder (tri(b.x,a.y,b.z), tri(a.x,a.y,b.z), .1) << new Pov_Cylinder (tri(b.x,a.y,b.z), tri(b.x,b.y,b.z), .1)
        << new Pov_Cylinder (tri(b.x,a.y,b.z), tri(b.x,a.y,a.z), .1) << new Pov_Cylinder (tri(b.x,b.y,a.z), tri(a.x,b.y,a.z), .1)
        << new Pov_Cylinder (tri(b.x,b.y,a.z), tri(b.x,a.y,a.z), .1) << new Pov_Cylinder (tri(b.x,b.y,a.z), tri(b.x,b.y,b.z), .1);
    }

    std::ostream & Pov_Scene::output_pov (std::ostream & os) {
        os  << "#version 3.7;" << std::endl
            << "#include \"colors.inc\"" << std::endl
            << "#include \"stones.inc\"" << std::endl
            << "#include \"rad_def.inc\"" << std::endl
            << "global_settings { radiosity { Rad_Settings(Radiosity_Normal,off,off) } }" << std::endl
            << "background { color White }" << std::endl;
        return pov_contents(os);
    }

    void Pov_Scene::output_pov (const std::string &s) {
        std::ostringstream os; if (s == "") os << "output/" << title; else os << s; os << ".pov";
        std::ofstream f (os.str().c_str()); output_pov(f);
    }
}
