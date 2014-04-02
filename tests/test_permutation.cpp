#include <vb/Hypermap.h>
#include <iostream>
#include <algorithm>

using namespace vb;
using namespace std;

vector<Permutation> permutations (int n, bool p (Permutation &) = [](Permutation&){return true;}) {
	Permutation s(n); vector<Permutation> output;
	do { if (p(s)) output.push_back(s); } while (next_permutation(s.begin(),s.end()));
	return output;
}

vector<int> signature (Permutation & s) {
	vector<int> output;
	for (auto c : s.cycles()) output.push_back(c.size());
	sort (output.begin(),output.end());
	return output;
}

bool connected (Permutation s, Permutation a) {
	int n=s.size(); vector<int> l(n); for (int i=0; i<n; ++i) l[i]=i;
	bool dirty=true; while (dirty) {
		dirty=false;
		for (int i=0; i<n; ++i) if (l[s[i]]<l[i]) { l[i]=l[s[i]]; dirty=true; }
		for (int i=0; i<n; ++i) if (l[a[i]]<l[i]) { l[i]=l[a[i]]; dirty=true; }
	}
	for (int i=0; i<n; ++i) if (l[i]>0) return false;
	return true;
}

vector<Hypermap> maps (vector<int> s, vector<int> a, vector<int> p) {
	Cycles cs; int i=0;
	for (int l : s) { vector<int> c; for (int j=0; j<l; ++j) c.push_back(i++); cs.push_back (c); }
	Permutation sigma (cs), alpha (sigma.size());
	vector<Hypermap> output;
	do {
		if (signature(alpha) != a) continue;
		Permutation phi = (sigma*alpha).inverse(); if (signature(phi) != p) continue;
		output.push_back (Hypermap(sigma,alpha,phi)); cerr<<".";
	} while (next_permutation(alpha.begin(),alpha.end()));
	return output;
}

int main (int argc, char ** argv) {
	auto ms = maps (vector<int> {2,2,3}, vector<int> {2,2,3}, vector<int> {2,2,3});
	for (auto m : ms) {
		cout << m;
		cout << "  sigma:     " << m.sigma;
		cout << "  alpha:     " << m.alpha;
		cout << "  phi:       " << m.phi;
		cout << "  connected: " << connected (m.sigma,m.alpha) << endl;
	}
	cout << "Total: " << ms.size() << " hypermap(s)" << endl;
}
