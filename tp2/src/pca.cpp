#include <iostream>
#include "pca.h"
#include "eigen.h"

using namespace std;

PCA::PCA(unsigned int n_components) : _alpha(n_components)
{
    
}

void PCA::fit(const Matrix& X)
{
    Matrix center = X.rowwise() - X.colwise().mean();
    Matrix cov = (center.transpose() * center) / double(X.rows() - 1);
    _base = get_first_eigenvalues(cov, _alpha).second;
}

Matrix PCA::transform(const Matrix& X)
{
    return transformBeta(X, _alpha);
}

Matrix PCA::transformBeta(const Matrix& X, unsigned int beta)
{
    return X * _base.block(0, 0, X.rows(), beta);
}
