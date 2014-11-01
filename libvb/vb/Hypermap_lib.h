#pragma once
#include <vb/Hypermap.h>
#include <map>

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
	Hypermap H_artem (int n);	// Triangular lattice with one edge type split into n edges

	class HLib : public std::map <std::string,Hypermap> { public: HLib (); };
}
