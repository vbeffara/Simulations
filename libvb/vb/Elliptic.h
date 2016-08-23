#pragma once
#include <vb/math.h>

namespace vb {
	template <typename T> class Elliptic { public:
		using cplx = typename cpx_t<T>::type;

		Elliptic (cplx q_);

		cplx eta1  	() const;
		cplx eta1_q	() const;
		cplx e1    	() const;
		cplx e1_q  	() const;
		cplx g2    	() const;
		cplx g3    	() const;
		cplx j     	() const;

		cplx theta1     	(const cplx & z) const;
		cplx theta1_z   	(const cplx & z) const;
		cplx theta1_zz  	(const cplx & z) const;
		cplx theta1_zzz 	(const cplx & z) const;
		cplx theta1_q   	(const cplx & z) const;
		cplx theta1_zq  	(const cplx & z) const;
		cplx theta1_zzq 	(const cplx & z) const;
		cplx theta1_zzzq	(const cplx & z) const;
		cplx theta2     	(const cplx & z) const;
		cplx theta2_z   	(const cplx & z) const;
		cplx theta2_q   	(const cplx & z) const;
		cplx theta3     	(const cplx & z) const;
		cplx theta3_q   	(const cplx & z) const;
		cplx theta4     	(const cplx & z) const;
		cplx theta4_q   	(const cplx & z) const;

		cplx sigma  	(const cplx & z) const;
		cplx sigma_q	(const cplx & z) const;
		cplx zeta   	(const cplx & z) const;
		cplx zeta_q 	(const cplx & z) const;
		cplx wp     	(const cplx & z) const;
		cplx wp_z   	(const cplx & z) const;
		cplx wp_q   	(const cplx & z) const;

		cplx q;

		cplx sum (std::function <cplx(int)>) const;

		cplx q14, eta1_, eta1_q_, e1_;
	};
}
