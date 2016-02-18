#include <yaml-cpp/yaml.h>
#include <iostream>
#include <vb/Hypermap_lib.h>

using namespace std; using namespace vb;

namespace YAML {
	template<> struct convert <Hypermap> {
		static Node encode (const Hypermap & h) {
		    Node node;
		    node["sigma"] = h.sigma.cycles();
		    node["alpha"] = h.alpha.cycles();
		    node["phi"]   = h.phi.cycles();
		    return node;
		}

		static bool decode (const Node & node, Hypermap & h) {
			auto sigma = node["sigma"].as<Cycles>();
			auto alpha = node["alpha"].as<Cycles>();
			auto phi = node["phi"].as<Cycles>();
			h = Hypermap (sigma, alpha, phi);
		    return true;
		}
	};
}

int main (int, char **) {
	YAML::Node hyp = YAML::LoadFile ("hypermaps.yaml");
	auto M = hyp.as<map<string,Hypermap>>();
	for (auto p : M) {
		assert (p.second == HLib()[p.first]);
		cout << p.first << ":" << endl;
		cout << p.second << endl;
	}
}
