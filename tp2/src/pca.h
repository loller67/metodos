#pragma once
#include "types.h"

class PCA {
public:
    PCA(unsigned int n_components);

    void fit(const Matrix& X);

    Matrix transform(const Matrix& X);

    //Agrego este método adicional para no volver a calcular toda la matriz de autovectores por cada alpha
    //En lugar utilizo PCA con alpha=X.cols y transformo los datos con beta <= alpha componentes
    Matrix transformBeta(const Matrix& X, unsigned int beta);

private:
    size_t _alpha;
    Matrix _base;
};
