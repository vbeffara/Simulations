/// \file
#include <vb/Pov.h>

namespace vb {
    std::ostream & Pov_Object::output_pov (std::ostream & os) {
        os << type << " { ";
        bool b=false;
        foreach (tri t, pts)     	{ if (b) os << ", "; os << t; b=true; }
        foreach (double d, coefs)	{ if (b) os << ", "; os << d; b=true; }
        pov_contents(os);
        return os << " }";
    }

    std::ostream & Pov_Camera::pov_contents (std::ostream & os)      	{ return os << "location " << a << " look_at " << b << " angle " << d; }
    std::ostream & Pov_Light_Source::pov_contents (std::ostream & os)	{ return os << " color White"; }
    std::ostream & Pov_Sphere::pov_contents (std::ostream & os)      	{ return os << " texture { pigment { color Red } }"; }
    std::ostream & Pov_Cylinder::pov_contents (std::ostream & os)    	{ return os << " texture { pigment { color Red } }"; }
    std::ostream & Pov_Box::pov_contents (std::ostream & os)         	{ return os << " texture { pigment { color rgb <.3,.7,1> } }"; }
    std::ostream & Pov_Plane::pov_contents (std::ostream & os)       	{ return os << " texture { pigment { color White } } finish { reflection {.8} ambient 0 diffuse 0 }"; }

    std::ostream & Pov_Frame::pov_contents (std::ostream & os) { return os
        << new Pov_Sphere (tri(a.x,a.y,a.z), .1) << new Pov_Sphere (tri(a.x,a.y,b.z), .1)
        << new Pov_Sphere (tri(a.x,b.y,a.z), .1) << new Pov_Sphere (tri(a.x,b.y,b.z), .1)
        << new Pov_Sphere (tri(b.x,a.y,a.z), .1) << new Pov_Sphere (tri(b.x,a.y,b.z), .1)
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
        foreach (Pov_Object *o, objs) os << o;
        return os;
    }

    void Pov_Scene::output_pov (const std::string &s) {
        std::ostringstream os;
        if (s == "") os << "output/" << title; else os << s;
        os << ".pov";
        std::cerr << "*** TODO: This should go in " << os.str() << std::endl;
        output_pov (std::cout);
    }
}
