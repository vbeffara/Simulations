#include <vb/Cube.h>
#include <vb/Pov.h>

namespace vb {
	Cube::Cube (int x, int y, int z, const std::string &name) : Bitmap <Adder> (x+z,y+z,name), sx(x), sy(y), sz(z), data(sx*sy*sz,0) {
		for (int x=0; x<sz; ++x) for (int y=0; y<sz; ++y) at(coo(sx+x,sy+y)) = (x/10+y/10) % 2 ? 200 : 150;
		for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) at(coo(x,y)).dim(sz);
		for (int x=0; x<sx; ++x) for (int z=0; z<sz; ++z) at(coo(x,z+sy)).dim(sy);
		for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) at(coo(z+sx,y)).dim(sx);
	}

	Cube_iterator	Cube::begin	()	{ return Cube_iterator (*this, coo3(0,0,0)); 	}
	Cube_iterator	Cube::end  	()	{ return Cube_iterator (*this, coo3(0,0,sz));	}

	void Cube::output_pov (std::string s) {
		Pov_Union * corner = new Pov_Union(); * corner
			<< new Pov_Plane  	(tri(1,0,0), -.75*sx)
			<< new Pov_Plane  	(tri(0,0,1),  .75*sz)
			<< new Pov_Texture	("pigment { color White } finish { reflection {.5} ambient 0 diffuse 0 }");

		Pov_Union * ground = new Pov_Union(); * ground
			<< new Pov_Plane  	(tri(0,1,0), -.75*sy)
			<< new Pov_Texture	("pigment { color White } finish { reflection 0 ambient 0.2 diffuse 0.1 }");

		Pov_Union * squares = new Pov_Union();
		for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) if (at(coo3(x,y,z))==255)
			* squares << new Pov_Box (tri(x-.5*sx,y-.5*sy,z-.5*sz), tri(x+1-.5*sx,y+1-.5*sy,z+1-.5*sz));
		* squares << new Pov_Texture ("pigment { color rgb <.3,.5,.8> } normal { bumps .1 scale .1 } finish { reflection {0} ambient 0 diffuse .5 brilliance 1.5 roughness .1 }");

		Pov_Union * squares2 = new Pov_Union();
		for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) if (at(coo3(x,y,z))==255/2)
			* squares2 << new Pov_Box (tri(x-.5*sx,y-.5*sy,z-.5*sz), tri(x+1-.5*sx,y+1-.5*sy,z+1-.5*sz));
		* squares2 << new Pov_Texture ("pigment { color rgb <.3,.5,.8> transmit .8 } normal { bumps .1 scale .1 } finish { reflection {0} ambient 0 diffuse .5 brilliance 1.5 roughness .1 }");

		Pov_Union * cube = new Pov_Union(); *cube
			<< new Pov_Frame (tri(-.5*sx,-.5*sy,-.5*sz), tri(.5*sx,.5*sy,.5*sz), "pigment { color rgb <.8,0,0> }")
			<< squares << squares2
			<< new Pov_Object ("rotate 360*clock*y", false);

		Pov_Scene SS;
		SS	<< new Pov_Camera (tri(1.30*sx,.9*sy,-1.5*sz), tri(0,0,0), 60)
		  	<< new Pov_Light_Source	(tri(50*sx,30*sy,-15*sz))
		  	<< cube;
		if (mirrors) SS << corner << ground;
		SS.output_pov(s == "" ? title : s);
	}
}
