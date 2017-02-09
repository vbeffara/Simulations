#include <vb/Hub.h>
#include <vb/LinearAlgebra.h>
#include <iomanip>

using namespace vb; using namespace cln; using namespace std;

template <typename T> void uBlas (string s) {
	Matrix<T> m (3,3); for (int i=0; i<3; ++i) for (int j=0; j<3; ++j) m(i,j) = int(pow(2*i+1,j));
	Vector<T> v (3); for (int i=0; i<3; ++i) v(i) = int(3*i-2);
	auto x = solve(m,v);

	H.L->info ("uBLAS  | {} | Matrix   m = {}", s, m);
	H.L->info ("uBLAS  | {} | Vector   v = {}", s, v);
	H.L->info ("uBLAS  | {} | Solution x = {}", s, x);
}

template <typename T> void Eigen3 (string s) {
	Eigen::Matrix <T,3,3> m; for (int i=0; i<3; ++i) for (int j=0; j<3; ++j) m(i,j) = int(pow(2*i+1,j));
	Eigen::Matrix <T,3,1> v; for (int i=0; i<3; ++i) v(i) = int(3*i-2);
	Eigen::Matrix <T,3,1> x = m.colPivHouseholderQr().solve(v);

	H.L->info ("Eigen3 | {} | Matrix   m = {}", s, m);
	H.L->info ("Eigen3 | {} | Vector   v = {}", s, v.transpose());
	H.L->info ("Eigen3 | {} | Solution x = {}", s, x.transpose());
}

int main () {
	cerr << setprecision(30);
	uBlas <double>  ("double");
	uBlas <cl_R>    ("CLR   ");
	uBlas <cl_N>    ("CLN   ");
	Eigen3 <double> ("double");
	Eigen3 <cl_R>   ("CLR   ");
	Eigen3 <cl_N>   ("CLN   ");
}
