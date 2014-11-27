#pragma once
#include <vb/cpx.h>
#include <vb/math.h>

namespace vb {
	template <typename T> class Elliptic { public:
		using cplx = std::complex<T>;

		Elliptic (const cplx & q_) : q(q_) {}

		cplx theta1 (const cplx & z) {
			cplx out(0), old(1);
			for (int n=0; out!=old; ++n) { old=out; out += T(2) * pow(T(-1),n) * pow(q, T((n+.5)*(n+.5))) * sin(T(2*n+1)*z); }
			return out;
		}

		cplx theta1_z (const cplx & z) {
			cplx out(0), old(1);
			for (int n=0; out!=old; ++n) { old=out; out += T(2) * pow(T(-1),n) * pow(q, T((n+.5)*(n+.5))) * T(2*n+1) * cos(T(2*n+1)*z); }
			return out;
		}

		cplx theta1_q (const cplx & z) {
			cplx out(0), old(1);
			for (int n=0; out!=old; ++n) { old=out; out += T(2) * pow(T(-1),n) * T((n+.5)*(n+.5)) * pow(q, T((n+.5)*(n+.5)-1)) * sin(T(2*n+1)*z); }
			return out;
		}

		cplx theta1_zq (const cplx & z) {
			cplx out(0), old(1);
			for (int n=0; out!=old; ++n) { old=out; out += T(2) * pow(T(-1),n) * T((n+.5)*(n+.5)) * pow(q, T((n+.5)*(n+.5)-1)) * T(2*n+1) * cos(T(2*n+1)*z); }
			return out;
		}

		cplx theta1_zz (const cplx & z) {
			cplx out(0), old(1);
			for (int n=0; out!=old; ++n) { old=out; out -= T(2) * pow(-1,n) * pow(q, T((n+.5)*(n+.5))) * pow(T(2*n+1),2) * sin(T(2*n+1)*z); }
			return out;
		}

		cplx theta1_zzq (const cplx & z) {
			cplx out(0), old(1);
			for (int n=0; out!=old; ++n) { old=out; out -= T(2) * pow(-1,n) * T((n+.5)*(n+.5)) * pow(q, T((n+.5)*(n+.5)-1)) * pow(T(2*n+1),2) * sin(T(2*n+1)*z); }
			return out;
		}

		cplx theta1_zzz (const cplx & z) {
			cplx out(0), old(1);
			for (int n=0; out!=old; ++n) { old=out; out -= T(2) * pow(T(-1),n) * pow(q, T((n+.5)*(n+.5))) * pow(T(2*n+1),3) * cos(T(2*n+1)*z); }
			return out;
		}

		cplx theta1_zzzq (const cplx & z) {
			cplx out(0), old(1);
			for (int n=0; out!=old; ++n) { old=out; out -= T(2) * pow(T(-1),n) * T((n+.5)*(n+.5)) * pow(q, T((n+.5)*(n+.5)-1)) * pow(T(2*n+1),3) * cos(T(2*n+1)*z); }
			return out;
		}

		cplx theta2 (const cplx & z) {
			cplx out(0), old(1);
			for (int n=0; out!=old; ++n) { old=out; out += T(2) * pow(q, T((n+.5)*(n+.5))) * cos(T(2*n+1)*z); }
			return out;
		}

		cplx theta2_z (const cplx & z) {
			cplx out(0), old(1);
			for (int n=0; out!=old; ++n) { old=out; out -= T(2) * pow(q, T((n+.5)*(n+.5))) * T(2*n+1) * sin(T(2*n+1)*z); }
			return out;
		}

		cplx theta2_q (const cplx & z) {
			cplx out(0), old(1);
			for (int n=0; out!=old; ++n) { old=out; out += T(2) * T((n+.5)*(n+.5)) * pow(q, T((n+.5)*(n+.5)-1)) * cos(T(2*n+1)*z); }
			return out;
		}

		cplx theta3 (const cplx & z) {
			cplx out(1), old(2);
			for (int n=1; out!=old; ++n) { old=out; out += T(2) * pow(q, n*n) * cos(T(2*n)*z); }
			return out;
		}

		cplx theta3_q (const cplx & z) {
			cplx out(0), old(1);
			for (int n=1; out!=old; ++n) { old=out; out += T(2) * T(n*n) * pow(q, n*n-1) * cos(T(2*n)*z); }
			return out;
		}

		cplx theta4 (const cplx & z) {
			cplx out(1), old(2);
			for (int n=1; out!=old; ++n) { old=out; out += T(2) * pow(-q, n*n) * cos(T(2*n)*z); }
			return out;
		}

		cplx theta4_q (const cplx & z) {
			cplx out(0), old(1);
			for (int n=1; out!=old; ++n) { old=out; out -= T(2) * T(n*n) * pow(-q, T(n*n-1)) * cos(T(2*n)*z); }
			return out;
		}

		cplx eta1 () {
			return - pi_<T>()*pi_<T>()/T(6) * theta1_zzz(0) / theta1_z(0);
		}

		cplx eta1_q () {
			return - pi_<T>()*pi_<T>()/T(6) * (theta1_z(0)*theta1_zzzq(0)
				- theta1_zzz(0)*theta1_zq(0)) / pow(theta1_z(0),2);
		}

		cplx sigma (const cplx & z) {
			return exp(eta1()*z*z) * theta1(pi_<T>()*z) / (pi_<T>()*theta1_z(0));
		}

		cplx sigma_q (const cplx & z) {
			return z*z*eta1_q() * exp(eta1()*z*z) * theta1(pi_<T>()*z) / (pi_<T>()*theta1_z(0))
				+ exp(eta1()*z*z) * theta1_q(pi_<T>()*z) / (pi_<T>()*theta1_z(0))
				- exp(eta1()*z*z) * theta1(pi_<T>()*z) * theta1_zq(0) / (pi_<T>()*pow(theta1_z(0),2));
		}

		cplx e1 () {
			return pi_<T>()*pi_<T>() * (pow(theta2(0),4) + T(2) * pow(theta4(0),4)) / T(3);
		}

		cplx e1_q () {
			return pi_<T>()*pi_<T>() * (T(4) * theta2_q(0) * pow(theta2(0),3)
				+ T(8) * theta4_q(0) * pow(theta4(0),3) ) / T(3);
		}

		cplx zeta (const cplx & z) {
			return T(2) * z * eta1() + pi_<T>() * theta1_z(pi_<T>()*z) / theta1(pi_<T>()*z);
		}

		cplx zeta_q (const cplx & z) {
			return T(2) * z * eta1_q()
			+ pi_<T>() * theta1_zq(pi_<T>()*z) / theta1(pi_<T>()*z)
			- pi_<T>() * theta1_z(pi_<T>()*z) * theta1_q(pi_<T>()*z) / pow(theta1(pi_<T>()*z),2);
		}

		cplx wp (const cplx & z) {
			return e1() + pow(pi_<T>()*theta3(0)*theta4(0)*theta2(pi_<T>()*z)/theta1(pi_<T>()*z),2);
		}

		cplx wp_z (const cplx & z) {
			return T(2) * pi_<T>() * (theta2_z(pi_<T>()*z)/theta2(pi_<T>()*z) - theta1_z(pi_<T>()*z)/theta1(pi_<T>()*z)) * (wp(z)-e1());
		}

		cplx wp_q (const cplx & z) {
			return e1_q() + T(2) * (wp(z)-e1()) * (theta3_q(0)/theta3(0)
				+ theta4_q(0)/theta4(0)
				+ theta2_q(pi_<T>()*z)/theta2(pi_<T>()*z) - theta1_q(pi_<T>()*z)/theta1(pi_<T>()*z));
		}

		cplx g2 () {
			cplx a=theta2(0), b=theta3(0);
			return pow(pi_<T>(),4)/T(.75)*(pow(a,8)-pow(a,4)*pow(b,4)+pow(b,8));
		}

		cplx g3 () {
			cplx a=theta2(0), b=theta3(0);
			return pow(pi_<T>(),6)/(pow(T(1.5),3))*(pow(a,12)-T(1.5)*(pow(a,8)*pow(b,4)+pow(a,4)*pow(b,8))+pow(b,12));
		}

		cplx j_ () {
			cplx ll = pow(theta2_(0,q)/theta3_(0,q),4);
			return T(256) * pow(T(1)-ll+ll*ll,3) / pow(ll*(T(1)-ll),2);
		}

		cplx q;
	};
}
