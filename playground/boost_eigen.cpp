#include <Eigen/Dense>
#include <boost/multiprecision/cpp_complex.hpp>
#include <boost/multiprecision/eigen.hpp>
#include <boost/multiprecision/mpc.hpp>
#include <iostream>

int main() {
    using namespace Eigen;
    typedef boost::multiprecision::mpc_complex_500 complex_type;
    //
    // We want to solve Ax = b for x,
    // define A and b first:
    //
    Matrix<complex_type, 2, 2> A, b;
    A << complex_type(2, 3), complex_type(-1, -2), complex_type(-1, -4), complex_type(3, 6);
    b << 1, 2, 3, 1;
    std::cout << "Here is the matrix A:\n" << A << std::endl;
    std::cout << "Here is the right hand side b:\n" << b << std::endl;
    //
    // Solve for x:
    //
    Matrix<complex_type, 2, 2> x = A.fullPivHouseholderQr().solve(b);
    std::cout << "The solution is:\n" << x << std::endl;
    //
    // Compute the error in the solution by using the norms of Ax - b and b:
    //
    complex_type::value_type relative_error = (A * x - b).norm() / b.norm();
    std::cout << "The relative error is: " << relative_error << std::endl;
    return 0;
}
