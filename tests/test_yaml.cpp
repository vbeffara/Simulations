#include <iostream>
#include <vb/Hypermap_lib.h>

using namespace std; using namespace vb;

int main (int, char **) {
	YAML::Node hyp = YAML::LoadFile ("hypermaps.yaml");
	auto M = hyp.as<map<string,Hypermap>>();
	for (auto p : M) {
		assert (p.second == HLib()[p.first]);
		cout << p.first << ":" << endl;
		cout << p.second << endl;
	}
}
