#pragma once
#include <vb/Hub.h>
#include <chrono>
#include <iostream>

namespace vb {
	double time ();

	template <typename F> void timing (const std::string &label, const F & f) {
		double t = time(); auto result = f();
		H.L->info ("{:<50} time = {:<10} ans = {}", label, time()-t, result);
	}

	template <typename V> std::ostream & vprint (std::ostream & os, const V & v,
												 std::string op = ",", const std::string & in = "(", const std::string & out = ")") {
		std::string sep = ""; os << in;
		for (const auto &x : v) os << exchange(sep,op) << x;
		return os << out;
	}
}
