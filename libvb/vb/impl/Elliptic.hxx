#pragma once
#include <vb/Elliptic.h>

namespace vb {
	template <typename T> auto Elliptic<T>::sum (std::function<std::complex<T>(int)> f) const -> cplx {
		cplx out(0), old(1);
		for (int n=0; out!=old; ++n) {
			old = out;
			cplx dd = f(n);
			if (std::isnormal(double(real(dd)))) out += dd;
		}
		return out;
	}

	template <typename T> Elliptic<T>::Elliptic (const cplx & q_) : q(q_), q14(pow(q,T(.25))), eta1_(eta1()), eta1_q_(eta1_q()), e1_(e1()) {}

	template <typename T> auto Elliptic<T>::theta1 (const cplx & z) const -> cplx {
		return sum ([&](int n) { return T(2 * pow(-1,n)) * q14 * pow(q, n*(n+1)) * sin(T(2*n+1)*z); });
	}

	template <typename T> auto Elliptic<T>::theta1_z (const cplx & z) const -> cplx {
		return sum ([&](int n) { return T(2 * pow(-1,n)) * q14 * pow(q, n*(n+1)) * T(2*n+1) * cos(T(2*n+1)*z); });
	}

	template <typename T> auto Elliptic<T>::theta1_q (const cplx & z) const -> cplx {
		return sum ([&](int n) { return T(2 * pow(-1,n)) * T((n+.5)*(n+.5)) * q14 * pow(q, n*(n+1)-1) * sin(T(2*n+1)*z); });
	}

	template <typename T> auto Elliptic<T>::theta1_zq (const cplx & z) const -> cplx {
		return sum ([&](int n) { return T(2 * pow(-1,n)) * T((n+.5)*(n+.5)) * q14 * pow(q, n*(n+1)-1) * T(2*n+1) * cos(T(2*n+1)*z); });
	}

	template <typename T> auto Elliptic<T>::theta1_zz (const cplx & z) const -> cplx {
		return sum ([&](int n) { return - T(2 * pow(-1,n)) * q14 * pow(q, n*(n+1)) * T(pow(2*n+1,2)) * sin(T(2*n+1)*z); });
	}

	template <typename T> auto Elliptic<T>::theta1_zzq (const cplx & z) const -> cplx {
		return sum ([&](int n) { return - T(2 * pow(-1,n)) * T((n+.5)*(n+.5)) * q14 * pow(q, n*(n+1)-1) * T(pow(2*n+1,2)) * sin(T(2*n+1)*z); });
	}

	template <typename T> auto Elliptic<T>::theta1_zzz (const cplx & z) const -> cplx {
		return sum ([&](int n) { return - T(2 * pow(-1,n)) * q14 * pow(q, n*(n+1)) * T(pow(2*n+1,3)) * cos(T(2*n+1)*z); });
	}

	template <typename T> auto Elliptic<T>::theta1_zzzq (const cplx & z) const -> cplx {
		return sum ([&](int n) { return - T(2 * pow(-1,n)) * T((n+.5)*(n+.5)) * q14 * pow(q, n*(n+1)-1) * T(pow(2*n+1,3)) * cos(T(2*n+1)*z); });
	}

	template <typename T> auto Elliptic<T>::theta2 (const cplx & z) const -> cplx {
		return sum ([&](int n) { return T(2) * q14 * pow(q, n*(n+1)) * cos(T(2*n+1)*z); });
	}

	template <typename T> auto Elliptic<T>::theta2_z (const cplx & z) const -> cplx {
		return sum ([&](int n) { return - T(2) * q14 * pow(q, n*(n+1)) * T(2*n+1) * sin(T(2*n+1)*z); });
	}

	template <typename T> auto Elliptic<T>::theta2_q (const cplx & z) const -> cplx {
		return sum ([&](int n) { return T(2) * T((n+.5)*(n+.5)) * q14 * pow(q, n*(n+1)-1) * cos(T(2*n+1)*z); });
	}

	template <typename T> auto Elliptic<T>::theta3 (const cplx & z) const -> cplx {
		return sum ([&](int n) { return T(n==0 ? 1 : 2) * pow(q, n*n) * cos(T(2*n)*z); });
	}

	template <typename T> auto Elliptic<T>::theta3_q (const cplx & z) const -> cplx {
		return sum ([&](int n) { return T(2) * T((n+1)*(n+1)) * pow(q, n*(n+2)) * cos(T(2*n+2)*z); });
	}

	template <typename T> auto Elliptic<T>::theta4 (const cplx & z) const -> cplx {
		return sum ([&](int n) { return T(n==0 ? 1 : 2) * pow(-q, n*n) * cos(T(2*n)*z); });
	}

	template <typename T> auto Elliptic<T>::theta4_q (const cplx & z) const -> cplx {
		return sum ([&](int n) { return - T(2) * T((n+1)*(n+1)) * pow(-q, n*(n+2)) * cos(T(2*n+2)*z); });
	}

	template <typename T> auto Elliptic<T>::eta1 () const -> cplx {
		return - pi_<T>()*pi_<T>()/T(6) * theta1_zzz(cplx(0)) / theta1_z(cplx(0));
	}

	template <typename T> auto Elliptic<T>::eta1_q () const -> cplx {
		return - pi_<T>()*pi_<T>()/T(6) * (theta1_z(cplx(0))*theta1_zzzq(cplx(0)) - theta1_zzz(cplx(0))*theta1_zq(cplx(0))) / pow(theta1_z(cplx(0)),2);
	}

	template <typename T> auto Elliptic<T>::sigma (const cplx & z) const -> cplx {
		return exp(eta1_*z*z) * theta1(pi_<T>()*z) / (pi_<T>()*theta1_z(cplx(0)));
	}

	template <typename T> auto Elliptic<T>::sigma_q (const cplx & z) const -> cplx {
		cplx pz = pi_<T>()*z; return	( theta1(pz) * (z*z*eta1_q_ - theta1_zq(cplx(0)) / theta1_z(cplx(0))) + theta1_q(pz) )
		                            	* exp(eta1_*z*z) / (pi_<T>()*theta1_z(cplx(0)));
	}

	template <typename T> auto Elliptic<T>::e1 () const -> cplx {
		return pi_<T>()*pi_<T>() * (pow(theta2(cplx(0)),4) + T(2) * pow(theta4(cplx(0)),4)) / T(3);
	}

	template <typename T> auto Elliptic<T>::e1_q () const -> cplx {
		return pi_<T>()*pi_<T>() * (T(4) * theta2_q(cplx(0)) * pow(theta2(cplx(0)),3)
			+ T(8) * theta4_q(cplx(0)) * pow(theta4(cplx(0)),3) ) / T(3);
	}

	template <typename T> auto Elliptic<T>::zeta (const cplx & z) const -> cplx {
		cplx pz = pi_<T>()*z; return T(2) * z * eta1_ + pi_<T>() * theta1_z(pz) / theta1(pz);
	}

	template <typename T> auto Elliptic<T>::zeta_q (const cplx & z) const -> cplx {
		cplx pz = pi_<T>()*z; return	T(2) * z * eta1_q_ + pi_<T>() * ( theta1_zq(pz)
		                            	- theta1_z(pz) * theta1_q(pz) / theta1(pz) ) / theta1(pz);
	}

	template <typename T> auto Elliptic<T>::wp (const cplx & z) const -> cplx {
		cplx pz = pi_<T>()*z; return e1_ + pow(pi_<T>()*theta3(cplx(0))*theta4(cplx(0))*theta2(pz)/theta1(pz),2);
	}

	template <typename T> auto Elliptic<T>::wp_z (const cplx & z) const -> cplx {
		cplx pz = pi_<T>()*z; return T(2) * pi_<T>() * (theta2_z(pz)/theta2(pz) - theta1_z(pz)/theta1(pz)) * (wp(z)-e1_);
	}

	template <typename T> auto Elliptic<T>::wp_q (const cplx & z) const -> cplx {
		cplx pz = pi_<T>()*z; return e1_q() + T(2) * (wp(z)-e1_) *
			(theta3_q(cplx(0))/theta3(cplx(0)) + theta4_q(cplx(0))/theta4(cplx(0)) + theta2_q(pz)/theta2(pz) - theta1_q(pz)/theta1(pz));
	}

	template <typename T> auto Elliptic<T>::g2 () const -> cplx {
		cplx a=theta2(cplx(0)), b=theta3(cplx(0));
		return pow(pi_<T>(),4)/T(.75)*(pow(a,8)-pow(a,4)*pow(b,4)+pow(b,8));
	}

	template <typename T> auto Elliptic<T>::g3 () const -> cplx {
		cplx a=theta2(cplx(0)), b=theta3(cplx(0));
		return pow(pi_<T>(),6)/(pow(T(1.5),3))*(pow(a,12)-T(1.5)*(pow(a,8)*pow(b,4)+pow(a,4)*pow(b,8))+pow(b,12));
	}

	template <typename T> auto Elliptic<T>::j () const -> cplx {
		cplx ll = pow(theta2(cplx(0))/theta3(cplx(0)),4);
		return T(256) * pow(T(1)-ll+ll*ll,3) / pow(ll*(T(1)-ll),2);
	}
}
