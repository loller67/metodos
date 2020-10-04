#include <algorithm>
//#include <chrono>
#include <pybind11/pybind11.h>
#include <iostream>
#include <exception>
#include "linear_regression.h"

using namespace std;
namespace py=pybind11;

LinearRegression::LinearRegression()
{
}

void LinearRegression::fit(Matrix X, Matrix y)
{
    X.conservativeResize(X.rows(), X.cols()+1);
    X.col(X.cols()-1) = MatrixXd::Ones(X.rows(), 1);

    Matrix A = X.transpose()*X;
    Matrix b = X.transpose()*y;

    coef = A.householderQr().solve(b);
}


Matrix LinearRegression::predict(Matrix X)
{
    X.conservativeResize(X.rows(), X.cols()+1);
    X.col(X.cols()-1) = MatrixXd::Ones(X.rows(), 1);

    auto ret = X*coef;

    return ret;
}

Matrix LinearRegression::coefs()
{
    return coef;
}

