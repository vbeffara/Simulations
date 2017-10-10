#include <vb/Cube.h>
#include <vb/Pov.h>

namespace vb {
	Cube::Cube (int x, int y, int z) : Bitmap <Adder> (x+z,y+z), sx(x), sy(y), sz(z), data(sx*sy*sz,0) {
		for (int x=0; x<sz; ++x) for (int y=0; y<sz; ++y) at(coo(sx+x,sy+y)) = Adder ((x/10+y/10) % 2 ? 200 : 150);
		for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) at(coo(x,y)).dim(sz);
		for (int x=0; x<sx; ++x) for (int z=0; z<sz; ++z) at(coo(x,z+sy)).dim(sy);
		for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) at(coo(z+sx,y)).dim(sx);
	}

	Cube_iterator	Cube::begin	()	{ return Cube_iterator (this, coo3(0,0,0)); 	}
	Cube_iterator	Cube::end  	()	{ return Cube_iterator (this, coo3(0,0,sz));	}

	void Cube::output_pov (std::string s) {
		auto squares = std::make_unique <Pov_Union> ();
		for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) if (at(coo3(x,y,z))==255)
			* squares << std::make_unique <Pov_Box> (tri(x-.5*sx,y-.5*sy,z-.5*sz), tri(x+1-.5*sx,y+1-.5*sy,z+1-.5*sz));
		* squares << std::make_unique <Pov_Texture> ("pigment { color rgb <.3,.5,.8> } normal { bumps .1 scale .1 } finish { reflection {0} ambient 0 diffuse .5 brilliance 1.5 roughness .1 }");

		auto squares2 = std::make_unique <Pov_Union> ();
		for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) if (at(coo3(x,y,z))==255/2)
			* squares2 << std::make_unique <Pov_Box> (tri(x-.5*sx,y-.5*sy,z-.5*sz), tri(x+1-.5*sx,y+1-.5*sy,z+1-.5*sz));
		* squares2 << std::make_unique <Pov_Texture> ("pigment { color rgb <.3,.5,.8> transmit .8 } normal { bumps .1 scale .1 } finish { reflection {0} ambient 0 diffuse .5 brilliance 1.5 roughness .1 }");

		auto cube = std::make_unique <Pov_Union>(); *cube
			<< std::make_unique <Pov_Frame> (tri(-.5*sx,-.5*sy,-.5*sz), tri(.5*sx,.5*sy,.5*sz), "pigment { color rgb <.8,0,0> }")
			<< std::move(squares) << std::move(squares2)
			<< std::make_unique <Pov_Object> ("rotate 360*clock*y", false);

		Pov_Scene SS;
		SS	<< std::make_unique <Pov_Camera> (tri(1.30*sx,.9*sy,-1.5*sz), tri(0,0,0), 60)
		  	<< std::make_unique <Pov_Light_Source>	(tri(50*sx,30*sy,-15*sz))
		  	<< std::move(cube);

		if (mirrors) {
			auto corner = std::make_unique <Pov_Union> (); * corner
				<< std::make_unique <Pov_Plane>  	(tri(1,0,0), -.75*sx)
				<< std::make_unique <Pov_Plane>  	(tri(0,0,1),  .75*sz)
				<< std::make_unique <Pov_Texture>	("pigment { color White } finish { reflection {.5} ambient 0 diffuse 0 }");
			auto ground = std::make_unique <Pov_Union> (); * ground
				<< std::make_unique <Pov_Plane>  	(tri(0,1,0), -.75*sy)
				<< std::make_unique <Pov_Texture>	("pigment { color White } finish { reflection 0 ambient 0.2 diffuse 0.1 }");
			SS << std::move(corner) << std::move(ground);
		}

		SS.output_pov(s == "" ? H.title : s);
	}
} // namespace vb
