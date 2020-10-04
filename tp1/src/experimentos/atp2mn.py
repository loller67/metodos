import csv
import json

#input_path = CSV del torneo atp
#players_input_path = CSV de los metadatos de jugadores
#ranking_input_path = Ranking oficial de ATP

#output_path = Archivo de salida para tp1
#metada_output_path = Archivo de salida de metadatos

#Metadatos adicionales para experimentar
def parseFromCSV(input_path, players_input_path, ranking_input_path, output_path, metada_output_path):
    #Construimos la entrada para tp1, reasignando la id original por numeros del 1..n con n=Cantidad de jugadores que participan
    idMap = {}
    with open(input_path,'r') as csvfile, open(output_path,'w') as outfile:
	    score_reader = csv.reader(csvfile,delimiter=',',skipinitialspace=True)

	    # Skip the header
	    next(score_reader, None)
	    teams = set()
	    n_games = 0

	    # String with info of the games.
	    out_str = ''
	    lastTourney = ''
	    monthCounter = 0
	    for row in score_reader:
		    currentTourney = row[1]
		    if currentTourney != lastTourney:
			    monthCounter += 1
			    lastTourney = currentTourney
		    idPlayerA = row[7]
		    if not idPlayerA in teams:
			    idMap[idPlayerA] = len(teams)+1
			    teams.add(idPlayerA)
		    idPlayerB = row[15]
		    if not idPlayerB in teams:
			    idMap[idPlayerB] = len(teams)+1	
			    teams.add(idPlayerB)
		    n_games = n_games+1
		    out_str = out_str + str(monthCounter) + ' ' + str(idMap[idPlayerA]) + ' ' + '1' + ' ' + str(idMap[idPlayerB]) + ' ' + '0' + '\n'
	    # Write number of teams and games
	    outfile.write(str(len(teams)) + ' ' + str(n_games) + '\n')
	    # Write games details
	    outfile.write(out_str)

	    print("Done")


    #Parseamos datos de los jugadores del CSV
    playerMap = {}
    with open(players_input_path,'r') as csvfile:
	    player_reader = csv.reader(csvfile,delimiter=',',skipinitialspace=True)
	    next(player_reader, None)
	    for row in player_reader:
		    playerId = row[0]
		    playerFirstName = row[1]
		    playerLastName = row[2]
		    playerHand = row[3]
		    playerBd = row[4]
		    playerCountry = row[5]
		    playerMap[playerId] = {'firstName': playerFirstName, 'lastName': playerLastName, 'hand': playerHand, 'birthday': playerBd, 'country':playerCountry}

    #Parseamos el ranking oficial del ATP
    rankingMap = {}
    with open(ranking_input_path,'r') as csvfile:
	    ranking_reader = csv.reader(csvfile,delimiter=',',skipinitialspace=True)
	    next(ranking_reader, None)
	    for row in ranking_reader:
		    date = row[0]
		    if date not in rankingMap:
			    rankingMap[date] = {}
		    rank = int(row[1])
		    playerId = row[2]
		    playerPoints = int(row[3])
		    rankingMap[date][playerId] = {'rank': rank, 'points': playerPoints}

    #Construimos los metadatos
    with open(metada_output_path,'w') as outJsonFile:
	    date = "20190225" #Usamos el ranking de la ultima fecha
	    metaData = {}
	    #Guardamos la data unicamente de los jugadores que participaron
	    for i in idMap:
		    playerName = playerMap[i]['lastName']
		    pointsATP = 0 if i not in rankingMap[date] else rankingMap[date][i]['points']
		    metaData[idMap[i]] = {'name': playerName, 'pointsATP': pointsATP}
            	
	    json.dump(metaData, outJsonFile)
