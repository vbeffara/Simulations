#pragma once
#include <vb/Hypermap.h>

/*
 * Naming: m_* are genus 0 "true" maps, lat_* are genus 1 maps, rest are genus 0 hypermaps
 *
 * Sources in the literature:
 *	KS: Khadjavi-Scharaschkin
 *	LZ: Lando-Zvonkin
 *	SV: Sherbet-Voevodski
 *	Z: Zvonkin
 */

namespace vb {
	Hypermap H_artem 	(int n);	// Triangular lattice with one edge type split into n edges
	Hypermap H_genus0	(int n);	// One arbitrary triangulation of the sphere with 2n faces
	Hypermap H_genus1	(int n);	// One arbitrary triangulation of the torus with n vertices

	class HLib : public std::map <std::string,Hypermap> { public: HLib (); };

	#ifdef UNIT_TESTS
	TEST_CASE ("vb::Hypermap library") {
		CHECK (HLib().at("tripod_l").sigma.size() == 4);
		CHECK (!H_artem(12).is_simple(6));
		CHECK (H_genus0(15).is_triangulation());
		CHECK (H_genus1(18).is_graph());
	}
	#endif
}
