#include "matrix.h"
#include "TeamsData.h"

#include "CMM.h"
#include "WP.h"
#include "CMM_ATP.h"

#include <iostream>
#include <fstream>

//Utilizadas para imprimir con mayor precision la salida
#include <iomanip>
#include <limits>


void showHelp();
void readInput(const std::string &inFileName, std::shared_ptr<TeamsData> &outData);
void writeOutput(const std::string &outFileName, const SparceMatrix &ranking);

int main(int argc, char** argv){
    using namespace std;

    if(argc < 4){
        showHelp();
        return 1;
    }

    string inFile(argv[1]);
    string outFile(argv[2]);
    int method(std::atoi(argv[3]));

    cout << "Reading input..." << endl;

    std::shared_ptr<TeamsData> data;
    readInput(inFile, data);

    cout << "Running method..." << endl;

    std::shared_ptr<RankingCalculator> rankingCalculator;

    switch(method) {
        //CMM
        case 0:
            rankingCalculator.reset(new CMM());
            break;
        //WP
        case 1:
            rankingCalculator.reset(new WP());
            break;
        //CMM Mod
        case 2:
            rankingCalculator.reset(new CMM_ATP());
            break;
        default:
            cout << "Invalid method... " << endl;
            showHelp();
            return 1;
    }

    std::shared_ptr<SparceMatrix> ranking = rankingCalculator->generateRanking(data);

    cout << "Writing " << outFile << "... " << endl;
    writeOutput(outFile, *ranking);
    cout << *ranking;

    return 0;
}

void showHelp(){
    using namespace std;
    cout << "Forma ejecución './tp entrada salida metodo' donde" << endl;
    cout << "-entrada: nombre archivo de entrada " << endl;
    cout << "-salida: nombre archivo de salida" << endl;
    cout << "-metodo: es un número [0=CMM, 1=WP, 2=CMM_ATP]" << endl;
}

void readInput(const std::string &inFileName, std::shared_ptr<TeamsData> &outData){
    using namespace std;
    ifstream file(inFileName);
    if(file.good()){
        int teams, matches;
        file >> teams >> matches;

        outData.reset(new TeamsData(teams));

        for (int currentMatch = 0; currentMatch < matches; ++currentMatch) {
            int date, team1, team1Goals, team2, team2Goals;
            file >> date >> team1 >> team1Goals >> team2 >> team2Goals;
            Match match = {date, team1, team1Goals, team2, team2Goals};
            outData->insertMatch(match);
        }
        file.close();
    }
}

void writeOutput(const std::string &outFileName, const SparceMatrix &ranking) {
    using namespace std;
    ofstream file(outFileName);
    size_t rows = ranking.rows();
    for(size_t r = 0; r < rows; ++r){
        file << setprecision(numeric_limits<double>::digits10) << ranking.retrieveAt(r, 0) << endl;
    }
    file.close();
}
