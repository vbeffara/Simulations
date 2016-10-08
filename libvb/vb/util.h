#pragma once
#include <vb/Hub.h>

namespace vb {
	double time ();

	template <typename F> void timing (const std::string &label, const F & f) {
		double t = time(); auto result = f();
		H.L->info ("{:<40} time = {:<10} ans = {}", label, time()-t, result);
	}
}
