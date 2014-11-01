#pragma once
#include <vb/Hypermap.h>
#include <map>

/*
 * Sources in the literature:
 *
 * KS: Khadjavi-Scharaschkin
 * SV: Sherbet-Voevodski
 * Z: Zvonkin
 */

namespace vb {
	Hypermap H_artem (int n);	// Triangular lattice with one edge type split into n edges

	class HLib : public std::map <std::string,Hypermap> { public: HLib (); };
}
