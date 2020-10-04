#pragma once

#include "types.h"


class KNNClassifier {
public:

    KNNClassifier(unsigned int n_neighbors);

    void fit(const Matrix& X, const IntVector& y);

    IntVector predict(const Matrix& X);

    IntVector predictWithK(size_t k_neighbors);

private:

    Vector _distanceToRow(const RowVector& row);

    size_t _predictRow(const RowVector& row);
    size_t _predict_cached_row(IntVector, size_t);

    size_t _k;
    Matrix _X;
    IntVector _y;

    std::vector< IntVector > _sortedNearestNeighbors;

};
