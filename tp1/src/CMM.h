#ifndef CMM_H
#define CMM_H

#include "RankingCalculator.h"

class CMM : public RankingCalculator {

    public:
        std::shared_ptr<SparceMatrix> generateRanking(std::shared_ptr<TeamsData> data) {
            using namespace std;

            auto cmm_b = buildCMM_b(*data);
            auto copyCmm = *cmm_b.first;
            auto copyB = *cmm_b.second;

            gaussian(copyCmm,copyB);

            auto res = solve(copyCmm, copyB);

            return res;
        }

    private:
        std::pair<std::shared_ptr<SparceMatrix>,std::shared_ptr<SparceMatrix>> buildCMM_b(const TeamsData &data) {
            using namespace std;
            const set<int> &teams = data.teams();

            std::shared_ptr<SparceMatrix> cmm(new SparceMatrix(teams.size(), teams.size()));
            std::shared_ptr<SparceMatrix> b(new SparceMatrix(teams.size(), 1));

            for(auto t1:teams){
                for(auto t2:teams){
                    if(t1 == t2){
                        cmm->at(t1-1,t2-1) = 2.0 + (double)(data.numberOfMatchesPlayed(t1));
                    }
                    else{
                        cmm->at(t1-1,t2-1) = -(double)(data.numberOfMatchesBetween(t1,t2));
                    }
                }
                double bVal = 1.0 + ((double)(data.numberOfWins(t1)) - (double)(data.numberOfLoses(t1))) / 2.0;
                b->insertValueAtRowColumn(bVal, t1-1, 0);
            }

            return { cmm, b };
        }

        void gaussian(SparceMatrix &M, SparceMatrix &b){
            using namespace std;
            auto rowsM = M.rows();

            for(size_t r1 = 0; r1 != rowsM - 1; ++r1){
                auto row1 = M.copyRowAtIndex(r1);
                auto v1 = row1.retrieveAt(0,r1);
                auto bR1 = b.retrieveAt(r1,0);
                for(size_t r2 = r1 + 1; r2 != rowsM; ++r2){
                    auto row2 = M.rowAtIndex(r2);
                    auto m = row2.retrieveAt(0, r1)/v1;
                    row2-= row1*m;
                    row2.insertValueAtRowColumn(0.0f, 0, r1);
                    auto bR2 = b.retrieveAt(r2,0);
                    b.insertValueAtRowColumn(bR2-bR1*m, r2,0);
                }
            }
        }

        std::shared_ptr<SparceMatrix> solve(const SparceMatrix &M, const SparceMatrix &b) {
            using namespace std;

            shared_ptr<SparceMatrix> ret(new SparceMatrix(b.rows(), 1));
            SparceMatrix xs(1, M.columns());

            for(size_t row = b.rows(); row != 0; --row){
                auto matRow = M.copyRowAtIndex(row-1);
                auto toSubtract = dot(xs, matRow);
                auto val = (b.retrieveAt(row-1, 0) - toSubtract)/matRow.retrieveAt(0,row-1);
                ret->insertValueAtRowColumn(val, row-1, 0);
                xs.insertValueAtRowColumn(val,0,row-1);
            }

            return ret;
        }

};

#endif //CMM_H
