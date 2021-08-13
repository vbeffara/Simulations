#pragma once
#include <vb/Hypermap.h>
#include <vb/util/Hub.h>

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
    auto H_artem(size_t n) -> Hypermap;  // Triangular lattice with one edge type split into n edges
    auto H_genus0(size_t n) -> Hypermap; // One arbitrary triangulation of the sphere with 2n faces
    auto H_genus1(size_t n) -> Hypermap; // One arbitrary triangulation of the torus with n vertices

    class HLib : public std::map<std::string, Hypermap> {
    public:
        HLib();
    };
} // namespace vb
