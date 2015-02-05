#pragma once
#include <vb/Hub.h>
#include <vb/Hypermap.h>
#include <vb/Image.h>
#include <vb/LinearAlgebra.h>

namespace vb {
	template <typename T> struct Star {
		std::complex<T> z;
		unsigned long d;
	};

	template <typename T> class Constellation { public:
		using cplx = std::complex<T>;

		std::vector<Star<T>>	b,w,f;
		std::vector<cplx>   	p;
	};
}
