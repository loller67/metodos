#pragma once

#include <Eigen/Sparse>
#include <Eigen/Dense>

using Eigen::MatrixXd;

typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Matrix;
typedef Eigen::SparseMatrix<double> SparseMatrix;

typedef Eigen::Matrix<size_t, Eigen::Dynamic, 1> IntVector;
typedef Eigen::VectorXd Vector;
typedef Eigen::RowVectorXd RowVector;
