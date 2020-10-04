#include <map>
#include <vector>
#include <set>

#ifndef TEAMSDATA_H
#define TEAMSDATA_H

struct Match {
    int date;
    int team1;
    int team1Goals;
    int team2;
    int team2Goals;
};

class TeamsData{
    public:
        TeamsData(size_t teamsCount): _teamByTeamMatchesCount(Matrix<size_t,std::map<size_t,size_t>>(teamsCount,teamsCount)) {
            for(size_t i = 1; i<=teamsCount; ++i) {
                _teams.insert(i);
            }
        }

        void insertMatch(const Match &match){
            std::shared_ptr<Match> newMatch(new Match(match));

            _teamByTeamMatchesCount.at(newMatch->team1-1, newMatch->team2-1)++;
            _teamByTeamMatchesCount.at(newMatch->team2-1, newMatch->team1-1)++;

            _matches.push_back(newMatch);

            if(newMatch->team1Goals > newMatch->team2Goals){
                _teamWins[newMatch->team1].push_back(newMatch);
                _teamLoses[newMatch->team2].push_back(newMatch);
            }
            else{
                _teamWins[newMatch->team2].push_back(newMatch);
                _teamLoses[newMatch->team1].push_back(newMatch);
            }
        }

        const std::set<int>& teams() const{
            return _teams;
        }

        const std::vector<std::shared_ptr<Match>> getMatches() const {
            return _matches;
        }

        size_t numberOfMatchesBetween(int team1, int team2) const{
            return _teamByTeamMatchesCount.retrieveAt(team1-1, team2-1);
        }

        size_t numberOfMatchesPlayed(int team) const{
            return numberOfWins(team) + numberOfLoses(team);
        }

        size_t numberOfWins(int team) const{
            auto it = _teamWins.find(team);
            if(it == _teamWins.end())
                return 0;
            else
                return it->second.size();
        }

        size_t numberOfLoses(int team) const{
            auto it = _teamLoses.find(team);
            if(it == _teamLoses.end())
                return 0;
            else
                return it->second.size();
        }
    private:
        std::set<int> _teams;
        Matrix<size_t, std::map<size_t,size_t>> _teamByTeamMatchesCount;
        std::vector<std::shared_ptr<Match>> _matches;
        std::map<int, std::vector<std::shared_ptr<Match>>> _teamWins;
        std::map<int, std::vector<std::shared_ptr<Match>>> _teamLoses;
};


#endif //TEAMSDATA_H
