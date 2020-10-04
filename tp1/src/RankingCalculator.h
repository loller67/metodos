#ifndef RANKINGCALCULATOR_H
#define RANKINGCALCULATOR_H

#include "matrix.h"
#include "TeamsData.h"

class RankingCalculator {

    public:
        virtual std::shared_ptr<SparceMatrix> generateRanking(std::shared_ptr<TeamsData> data) = 0;

};

#endif //RANKINGCALCULATOR_H
