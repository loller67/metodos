#ifndef CMM_ATP_H
#define CMM_ATP_H

#include "RankingCalculator.h"
#include "CMM.h"

class CMM_ATP : public RankingCalculator {

    public:
        std::shared_ptr<SparceMatrix> generateRanking(std::shared_ptr<TeamsData> data) {
            //Guardamos cada match en su fecha
            //Interpretamos cada fecha como un torneo
            std::map<int, std::vector<std::shared_ptr<Match>>> matchesByDate;
            for (auto match : data->getMatches()) {
                matchesByDate[match->date].push_back(match);
            }

            //Separamos los torneos por categorias
            //Las categorias las determinamos por la cantidad de partidos que componen el torneo
            std::map<int, std::vector<std::shared_ptr<Match>>> matchesByCategory;
            for (auto tournament : matchesByDate) {
                int tournamentClass = tournamentType(tournament.second.size());
                for (auto match : tournament.second) {
                    matchesByCategory[tournamentClass].push_back(match);
                }
            }

            //Vector donde vamos a guardar el rating final
            std::shared_ptr<SparceMatrix> rating(new SparceMatrix(data->teams().size(), 1));

            //Calculamos el score por cada categoria
            std::map<int, int> tournamentsPlayed;
            for (auto tournament : matchesByCategory) {

                //Calculamos el score de la categoría
                std::map<int, double> tournamentScore = getTournamentScore(tournament.second);
                for (auto res : tournamentScore) {
                    int team = res.first;
                    double score = res.second;
                    //Sumamos su score por el peso de la competicion
                    double current = rating->retrieveAt(team-1, 0) + score * scoreWeight(tournament.first);
                    rating->insertValueAtRowColumn(current, team-1, 0);
                }
            }

            return rating;
        }


    private:
        std::map<int, double> getTournamentScore(std::vector<std::shared_ptr<Match>> &tournament) {
            std::map<int, double> res;

            size_t teamCount = 0;
            std::map<int, int> originalToAsigned;
            std::map<int, int> asignedToOriginal;

            //Reasignamos los numeros de equipo
            for(auto match : tournament) {
                if(originalToAsigned.count(match->team1) == 0) {
                    teamCount++;
                    originalToAsigned[match->team1] = teamCount;
                    asignedToOriginal[teamCount] = match->team1;
                }
                if(originalToAsigned.count(match->team2) == 0) {
                    teamCount++;
                    originalToAsigned[match->team2] = teamCount;
                    asignedToOriginal[teamCount] = match->team2;
                }
                match->team1 = originalToAsigned[match->team1];
                match->team2 = originalToAsigned[match->team2];
            }
            //Construimos la entrada para aplicar CMM
            std::shared_ptr<TeamsData> teamData(new TeamsData(teamCount));
            for(auto match : tournament) {
                teamData->insertMatch(*match);
            }

            //Calculamos el puntaje con CMM
            std::shared_ptr<SparceMatrix> ranking = _cmm.generateRanking(teamData);

            //Reasignamos los numeros de equipo originales
            for(size_t i = 1; i <= teamCount; i++) {
                double score = ranking->retrieveAt(i-1, 0);
                int team = asignedToOriginal[i];
                res[team]=score;
            }
            return res;
        }

        double scoreWeight(int tournamentType) {
            //GrandSlam
            if(tournamentType==0) {
                return 2000.0/3750.0;
            //Master
            } else if (tournamentType==1) {
                return 1000.0/3750.0;
            //ATP WorldTour 500
            }else if (tournamentType==2){
                return 500.0/3750.0;
            //ATP WORLD TOUR 250
            }else {
                return 250.0/3750.0;
            }
        }

        int tournamentType(size_t tournamentMatches) {
            //GrandSlam
            if(tournamentMatches > 100) {
                return 0;
            //Master
            } else if (tournamentMatches > 50) {
                return 1;
            //ATP WorldTour 500
            }else if (tournamentMatches > 30){
                return 2;
            //ATP WORLD TOUR 250
            }else {
                return 3;
            }
        }

        CMM _cmm;
};


#endif //CMM_ATP_H
