#include <algorithm>
#include <chrono>
#include <iostream>
#include "eigen.h"

using namespace std;

pair<double, Vector> power_iteration(const Matrix& A, unsigned iterations, double epsilon)
{
    Vector v = Vector::Random(A.cols());
    for (size_t i = 0; i < iterations; ++i) {
        Vector o = v;
        v = A * v;
        v.normalize();
        if ((v - o).squaredNorm() < epsilon * epsilon) {
            break;
        }
    }
    double eigenvalue = ((v.transpose()*A*v)/(v.transpose()*v))(0);

    return make_pair(eigenvalue, v / v.norm());
}

pair<Vector, Matrix> get_first_eigenvalues(const Matrix& A, unsigned n, unsigned iterations, double epsilon)
{
    Matrix M(A);
    Matrix eigenvectors(A.rows(), n);
    Vector eigenvalues(n);
    for (size_t i = 0; i < n; ++i) {
        pair<double, Vector> e = power_iteration(M, iterations, epsilon);
        eigenvalues(i) = e.first;
        eigenvectors.col(i) = e.second;
        M = M - e.first * (e.second * e.second.transpose());
    }

    return make_pair(eigenvalues, eigenvectors);
}
