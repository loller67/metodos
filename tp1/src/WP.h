#ifndef WP_H
#define WP_H

#include "RankingCalculator.h"

class WP : public RankingCalculator {

    public:
        std::shared_ptr <SparceMatrix> generateRanking(std::shared_ptr <TeamsData> data) {
            using namespace std;
            std::shared_ptr<SparceMatrix> wp(new SparceMatrix(data->teams().size(), 1));
            for (auto t : data->teams()) {
                double percentage = (double)(data->numberOfWins(t))/(double)(data->numberOfMatchesPlayed(t));
                wp->insertValueAtRowColumn(percentage, t-1, 0);
            }

            return wp;
        }
};


#endif //WP_H
