#include <algorithm>
//#include <chrono>
#include <iostream>
#include <map>
#include "knn.h"

using namespace std;


KNNClassifier::KNNClassifier(unsigned int n_neighbors) : _k(n_neighbors), _sortedNearestNeighbors()
{
}

void KNNClassifier::fit(const Matrix& X, const IntVector& y)
{
    _X = X;
    _y = y;
}

/**
* Devuelve un vector con la distancia al cuadrado de 'row' con respecto a cada elemento del entrenamiento
*/
Vector KNNClassifier::_distanceToRow(const RowVector& row)
{
    auto ret = Vector(_X.rows());
    for (Eigen::Index i = 0; i < _X.rows(); ++i)
    {
        ret(i) = (_X.row(i) - row).squaredNorm();
    }
    return ret;
}

/**
* Predice el dígito utilizando KNN
*/
size_t KNNClassifier::_predictRow(const RowVector& row)
{
    auto dist = _distanceToRow(row);

    size_t n = 0;
    IntVector neighbors(_X.rows());
    std::generate(neighbors.data(), neighbors.data() + neighbors.size(), [&n] { return n++; });
    std::sort(neighbors.data(), neighbors.data() + neighbors.size(),
              [&dist](int i1, int i2) { return dist(i1) < dist(i2); });

    _sortedNearestNeighbors.push_back(neighbors);

    return _predict_cached_row(neighbors, _k);
}

size_t KNNClassifier::_predict_cached_row(IntVector distances, size_t new_k){
    std::map<size_t, size_t> neighborScore;

    for (size_t i = 0; i < new_k; ++i) {
        neighborScore[_y(distances(i))]++;
    }

    auto res = std::max_element(neighborScore.begin(), neighborScore.end(),
                                [](const std::pair<double, int> &a, const std::pair<double, int> &b) -> bool
                                {
                                    return a.second < b.second;
                                });

    return res->first;
}

IntVector KNNClassifier::predictWithK(size_t k_neighbors) {
    // Creamos el vector columna a devolver
    auto ret = IntVector(_sortedNearestNeighbors.size());
    
    for (size_t i = 0; i < _sortedNearestNeighbors.size(); ++i)
    {
        size_t result = _predict_cached_row(_sortedNearestNeighbors[i], k_neighbors);
        ret(i) = result;
    }

    return ret;
}

IntVector KNNClassifier::predict(const Matrix& X)
{
    // Creamos vector columna a devolver
    _sortedNearestNeighbors.clear();
    auto ret = IntVector(X.rows());

    for (Eigen::Index i = 0; i < X.rows(); ++i)
    {
        ret(i) = _predictRow(X.row(i));
    }

    return ret;
}
