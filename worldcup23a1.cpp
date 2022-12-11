#include "worldcup23a1.h"

#define POINTS_FOR_WIN 3
#define POINTS_FOR_TIE 1
#define POINTS_FOR_LOSS 0
#define PLAYERS_TO_COMPARE 2

world_cup_t::world_cup_t() {
	playersCounter = 0;
	teamCounter = 0;
	topScorerId = 0;
	teams = AvlTree<int, Team*>();
	playersByScore = AvlTree<Player, Player*>();
	playersById = AvlTree<int, Player*>();
}

world_cup_t::~world_cup_t() = default;

StatusType world_cup_t::add_team(int teamId, int points) {
	if (teamId <= 0 || points < 0) {
		return StatusType::INVALID_INPUT;
	}
	// Check there is no team with given id
	Team* team;
	TreeStatusType teamsFindResult = teams.find(teamId, &team);
	if (teamsFindResult == TreeStatusType::TREE_SUCCESS) {
		return StatusType::FAILURE;
	}

	// Create new team and add to teams tree
	Team* newTeam = new Team(teamId, points);
	TreeStatusType teamsAddResult = teams.insert(teamId, newTeam);
	if (teamsAddResult == TreeStatusType::TREE_FAILURE) {
		delete newTeam;
		return StatusType::FAILURE;
	}
	else if (teamsAddResult == TreeStatusType::TREE_ALLOCATION_ERROR) {
		delete newTeam;
		return StatusType::ALLOCATION_ERROR;
	}
	teamCounter++;
	return StatusType::SUCCESS;
}

StatusType world_cup_t::remove_team(int teamId) {
	if (teamId <= 0) {
		return StatusType::INVALID_INPUT;
	}
	// Find team
	Team* teamToRemove;
	TreeStatusType teamsFindResult = teams.find(teamId, &teamToRemove);
	if (teamsFindResult == TreeStatusType::TREE_FAILURE || teamToRemove->get_all_players_count() != 0) {
		return StatusType::FAILURE;
	}
	// Remove team from teams tree
	TreeStatusType teamsRemoveResult = teams.remove(teamId);
	if (teamsRemoveResult == TreeStatusType::TREE_FAILURE) {
		return StatusType::FAILURE;
	}
	delete teamToRemove;
	teamCounter--;
	return StatusType::SUCCESS;
}

StatusType world_cup_t::add_player(int playerId, int teamId, int gamesPlayed, int goals, int cards, bool goalKeeper) {
	// Check input is valid
	if (playerId <= 0 || teamId <= 0 || gamesPlayed < 0 || goals < 0 || cards < 0 || (gamesPlayed == 0 and (goals > 0 || cards > 0))) {
		return StatusType::INVALID_INPUT;
	}

	// Check there is no player with given id
	Player* playerPtr;
	TreeStatusType playersByIdSearchResult = playersById.find(playerId, &playerPtr);
	if (playersByIdSearchResult == TreeStatusType::TREE_SUCCESS) {
		return StatusType::FAILURE;
	}

	// Add player to team
	Team* teamFound;
	TreeStatusType teamsFindResult = teams.find(teamId, &teamFound);
	if (teamsFindResult == TreeStatusType::TREE_FAILURE) {
		return StatusType::FAILURE;
	}
	

	// Add player to global data structure
	Player* newPlayer = new Player(playerId, gamesPlayed, goals, cards, goalKeeper, teamFound);
	TreeStatusType playersByIdAddResult = playersById.insert(playerId, newPlayer);
	if (playersByIdAddResult == TreeStatusType::TREE_FAILURE) {
		delete newPlayer;
		return StatusType::FAILURE;
	}
	else if (playersByIdAddResult == TreeStatusType::TREE_ALLOCATION_ERROR) {
		delete newPlayer;
		return StatusType::ALLOCATION_ERROR;
	}
	TreeStatusType playersByScoreAddResult = playersByScore.insert(*newPlayer, newPlayer);
	if (playersByScoreAddResult == TreeStatusType::TREE_FAILURE) {
		delete newPlayer;
		TreeStatusType playersByIdRemoveResult = playersById.remove(playerId);
		if (playersByIdRemoveResult == TreeStatusType::TREE_FAILURE) {
			// throw exception
		}
		return StatusType::FAILURE;
	}
	else if (playersByScoreAddResult == TreeStatusType::TREE_ALLOCATION_ERROR) {
		delete newPlayer;
		TreeStatusType playersByIdRemoveResult = playersById.remove(playerId);
		if (playersByIdRemoveResult == TreeStatusType::TREE_FAILURE) {
			// throw exception
		}
		return StatusType::ALLOCATION_ERROR;
	}
	StatusType teamAddPlayerStatus = teamFound->add_player(newPlayer);
	if (teamAddPlayerStatus != StatusType::SUCCESS) {  // check player addition to team
		return teamAddPlayerStatus;
	}
	// Update top scorer
	Player* topScorer = playersByScore.find_max();
	if (topScorer != NULL) {
		topScorerId = topScorer->get_player_id();
	}
	playersCounter++;  // Update global player counter
	return StatusType::SUCCESS;
}

StatusType world_cup_t::remove_player(int playerId) {
	// Check input is valid
	if (playerId <= 0) {
		return StatusType::INVALID_INPUT;
	}

	// Find player in global data structure
	Player* playerPtr;
	TreeStatusType playersByIdSearchResult = playersById.find(playerId, &playerPtr);
	if (playersByIdSearchResult == TreeStatusType::TREE_FAILURE) {
		return StatusType::FAILURE;
	}

	// Remove player from team
	Team* teamFound = playerPtr->get_team();
	StatusType teamRemovePlayerStatus = teamFound->remove_player(playerId);
	if (teamRemovePlayerStatus != StatusType::SUCCESS) {  // check player removal to team
		return teamRemovePlayerStatus;
	}
	// Remove player from global data structure
	TreeStatusType playersByScoreRemoveResult = playersByScore.remove(*playerPtr);
	TreeStatusType playersByIdRemoveResult = playersById.remove(playerId);
	if (playersByIdRemoveResult != TreeStatusType::TREE_SUCCESS || playersByScoreRemoveResult != TreeStatusType::TREE_SUCCESS) {
		return StatusType::FAILURE;
	}
	// Update top scorer
	Player* topScorer = playersByScore.find_max();
	if (topScorer != NULL) {
		topScorerId = topScorer->get_player_id();
	}
	delete playerPtr;
	playersCounter--;
	return StatusType::SUCCESS;
}

StatusType world_cup_t::update_player_stats(int playerId, int gamesPlayed, int scoredGoals, int cardsReceived) {
	if (playerId <= 0 || gamesPlayed < 0 || scoredGoals < 0 || cardsReceived < 0) {
		return StatusType::INVALID_INPUT;
	}
	// Find player
	Player* playerPtr;
	TreeStatusType playersByIdSearchResult = playersById.find(playerId, &playerPtr);
	if (playersByIdSearchResult == TreeStatusType::TREE_FAILURE) {
		return StatusType::FAILURE;
	}

	// Remove player from tree ordered by score before updating stats
	TreeStatusType playersByScoreRemoveResult = playersByScore.remove(*playerPtr);
	if (playersByScoreRemoveResult == TreeStatusType::TREE_FAILURE) {
		return StatusType::FAILURE;
	}
	playerPtr->get_team()->remove_player(playerId);
	// Update stats
	playerPtr->set_goals(playerPtr->get_goals() + scoredGoals);
	playerPtr->set_cards(playerPtr->get_cards() + cardsReceived);
	playerPtr->set_games_played(playerPtr->get_games_played() + gamesPlayed - playerPtr->get_team()->get_games_played());

	// Re-insert player to score ordered tree after stats update
	TreeStatusType playersByScoreAddResult = playersByScore.insert(*playerPtr, playerPtr);
	if (playersByScoreAddResult == TreeStatusType::TREE_FAILURE) {
		return StatusType::FAILURE;
	}
	StatusType reAddPlayerToTeamResult = playerPtr->get_team()->add_player(playerPtr);
	if (reAddPlayerToTeamResult != StatusType::SUCCESS) {
		return StatusType::FAILURE;
	}

	// Update top scorer
	Player* topScorer = playersByScore.find_max();
	if (topScorer != NULL) {
		topScorerId = topScorer->get_player_id();
	}

	// Update player in team
	return StatusType::SUCCESS;
}

StatusType world_cup_t::play_match(int teamId1, int teamId2) {
	// Check input is valid
	if (teamId1 <= 0 || teamId2 <= 0 || teamId1 == teamId2) {
		return StatusType::INVALID_INPUT;
	}

	// Find teams
	Team* team1;
	Team* team2;
	TreeStatusType team1FindResult = teams.find(teamId1, &team1);
	TreeStatusType team2FindResult = teams.find(teamId2, &team2);
	if (team1FindResult == TreeStatusType::TREE_FAILURE || team2FindResult == TreeStatusType::TREE_FAILURE) {
		return StatusType::FAILURE;
	}

	// Check teams are valid for match
	if (!team1->is_team_valid() || !team2->is_team_valid()) {
		return StatusType::FAILURE;
	}

	int team1Score = team1->get_points() + team1->get_team_goals() - team1->get_team_cards();
	int team2Score = team2->get_points() + team2->get_team_goals() - team2->get_team_cards();
	if (team1Score > team2Score) {  // Team1 wins
		team1->set_points(team1->get_points() + POINTS_FOR_WIN);
		team2->set_points(team2->get_points() + POINTS_FOR_LOSS);
	}
	else if (team1Score < team2Score) {  // Team2 wins
		team1->set_points(team1->get_points() + POINTS_FOR_LOSS);
		team2->set_points(team2->get_points() + POINTS_FOR_WIN);
	}
	else {  // Tie
		team1->set_points(team1->get_points() + POINTS_FOR_TIE);
		team2->set_points(team2->get_points() + POINTS_FOR_TIE);
	}
	team1->set_games_played_after_update(team1->get_games_played() + 1);
	team2->set_games_played_after_update(team2->get_games_played() + 1);
	return StatusType::SUCCESS;
}

output_t<int> world_cup_t::get_num_played_games(int playerId) {
	// Check input is valid
	if (playerId <= 0) {
		return output_t<int>(StatusType::INVALID_INPUT);
	}
	// Find player
	Player* playerPtr;
	TreeStatusType playersByIdSearchResult = playersById.find(playerId, &playerPtr);
	if (playersByIdSearchResult == TreeStatusType::TREE_FAILURE) {
		return output_t<int>(StatusType::FAILURE);
	}
	return output_t<int>(playerPtr->get_games_played());
}

output_t<int> world_cup_t::get_team_points(int teamId) {
	// Check input is valid
	if (teamId <= 0) {
		return output_t<int>(StatusType::INVALID_INPUT);
	}
	Team* teamFound;
	TreeStatusType teamFindResult = teams.find(teamId, &teamFound);
	if (teamFindResult == TreeStatusType::TREE_FAILURE) {
		return output_t<int>(StatusType::FAILURE);
	}
	return output_t<int>(teamFound->get_points());
}

StatusType world_cup_t::unite_teams(int teamId1, int teamId2, int newTeamId) {
	// Check input is valid
	if (teamId1 <= 0 || teamId2 <= 0 || newTeamId <= 0 || teamId1 == teamId2) {
		return StatusType::INVALID_INPUT;
	}
	// Get 2 teams
	Team* team1;
	TreeStatusType team1FindResult = teams.find(teamId1, &team1);
	if (team1FindResult == TreeStatusType::TREE_FAILURE) {
		return StatusType::FAILURE;
	}
	Team* team2;
	TreeStatusType team2FindResult = teams.find(teamId2, &team2);
	if (team2FindResult == TreeStatusType::TREE_FAILURE) {
		return StatusType::FAILURE;
	}
	
	Team* checkNewTeam;
	TreeStatusType checkNewTeamFindResult = teams.find(newTeamId, &checkNewTeam);
	if (checkNewTeamFindResult == TreeStatusType::TREE_SUCCESS && newTeamId != teamId1 && newTeamId != teamId2) {
		return StatusType::FAILURE;
	}

	// Create new team
	Team* newTeam = new Team(newTeamId, team1->get_points() + team2->get_points());
	// Merge 2 teams into new team
	newTeam->merge_teams(team1, team2);

	// Remove 2 previous teams
	team1->clear_players();
	team2->clear_players();
	this->remove_team(teamId1);
	this->remove_team(teamId2);
	

	// Add new team
	TreeStatusType teamsAddResult = teams.insert(newTeamId, newTeam);
	if (teamsAddResult == TreeStatusType::TREE_FAILURE) {
		teams.insert(teamId1, team1);
		teams.insert(teamId2, team2);
		delete newTeam;
		return StatusType::FAILURE;
	}
	else if (teamsAddResult == TreeStatusType::TREE_ALLOCATION_ERROR) {
		teams.insert(teamId1, team1);
		teams.insert(teamId2, team2);
		delete newTeam;
		return StatusType::ALLOCATION_ERROR;
	}

	teamCounter++;
	
	return StatusType::SUCCESS;
}


output_t<int> world_cup_t::get_top_scorer(int teamId) {
	// Check intput is valid
	if (teamId == 0) {
		return output_t<int>(StatusType::INVALID_INPUT);
	}
	// Find global top scorer
	if (teamId < 0) {
		if (topScorerId == 0) {
			return output_t<int>(StatusType::FAILURE);
		}
		return output_t<int>(topScorerId);
	}
	// Team top scorer
	else {
		Team* teamFound;
		TreeStatusType teamFindResult = teams.find(teamId, &teamFound);
		if (teamFindResult == TreeStatusType::TREE_FAILURE) {
			return output_t<int>(StatusType::FAILURE);
		}
		return teamFound->get_top_scorer_id();
	}
}

output_t<int> world_cup_t::get_all_players_count(int teamId) {
	// Check intput is valid
	if (teamId == 0) {
		return output_t<int>(StatusType::INVALID_INPUT);
	}
	// Find global number of players
	if (teamId < 0) {
		return output_t<int>(playersCounter);
	}
	// Find team number of players
	else {
		Team* teamFound;
		TreeStatusType teamFindResult = teams.find(teamId, &teamFound);
		if (teamFindResult == TreeStatusType::TREE_FAILURE) {
			return output_t<int>(StatusType::FAILURE);
		}
		return output_t<int>(teamFound->get_all_players_count());
	}
}

StatusType world_cup_t::get_all_players(int teamId, int* const output) {
	// Check intput is valid
	if (teamId == 0) {
		return StatusType::INVALID_INPUT;
	}
	// Get all global players
	if (teamId < 0) {
		Player* allPlayers = new Player [playersCounter];
		playersByScore.get_tree_keys_in_order(allPlayers);
		for (int playerIdx = 0; playerIdx < playersCounter; playerIdx++) {
			output[playerIdx] = allPlayers[playerIdx].get_player_id();
		}
		delete[] allPlayers;
	}
	// Get all team players
	else {
		Team* teamFound;
		TreeStatusType teamFindResult = teams.find(teamId, &teamFound);
		if (teamFindResult == TreeStatusType::TREE_FAILURE) {
			return StatusType::FAILURE;
		}
		teamFound->get_all_players_id(output);
	}
	return StatusType::SUCCESS;
}

int compare_players(Player* player1, Player* player2, Player* refrencePlayer) {
	// Function that finds closest (score-wise) out of 2 players to given refrence player
	// returns positive number if player1 is closest to refrencePlayer, negative number if player2 is closest to refrencePlayer
	if (refrencePlayer == NULL) {
		return 0;
	}
	// Create candidate for closest player array
	Player** candidatePlayers = new Player * [PLAYERS_TO_COMPARE];
	candidatePlayers[0] = player1;
	candidatePlayers[1] = player2;
	// Arrays containing absalut difference between given player and candidates (per stat)
	int goalDiffs[PLAYERS_TO_COMPARE]{};
	int cardsDiffs[PLAYERS_TO_COMPARE]{};
	int playerIdDiffs[PLAYERS_TO_COMPARE]{};

	// Compute absalut differences
	for (int playerNum = 0; playerNum < PLAYERS_TO_COMPARE; playerNum++) {
		Player* currPlayer = candidatePlayers[playerNum];
		if (currPlayer != NULL) {
			// Insert absolute difference of each field
			goalDiffs[playerNum] = refrencePlayer->get_goals() - currPlayer->get_goals() >= 0 ? refrencePlayer->get_goals() - currPlayer->get_goals() : -1 * (refrencePlayer->get_goals() - currPlayer->get_goals());
			cardsDiffs[playerNum] = refrencePlayer->get_cards() - currPlayer->get_cards() >= 0 ? refrencePlayer->get_cards() - currPlayer->get_cards() : -1 * (refrencePlayer->get_cards() - currPlayer->get_cards());
			playerIdDiffs[playerNum] = refrencePlayer->get_player_id() - currPlayer->get_player_id() >= 0 ? refrencePlayer->get_player_id() - currPlayer->get_player_id() : -1 * (refrencePlayer->get_player_id() - currPlayer->get_player_id());
		}
		else { // one of the players is NULL
			return 2 * playerNum - 1; // returns 1 if player2 is NULL, returns -1 if player1 is NULL
		}
	}

	// For each stat, check if minimum difference is unique. If not unique follow rules to determine closest neighbor
	int closestPlayerNum = 0;  // Index of closest neighbor
	if (goalDiffs[0] != goalDiffs[1]) {  // There is a unique minimal goals absalut difference
		closestPlayerNum = goalDiffs[0] < goalDiffs[1] ? 0 : 1;
	}
	else if (cardsDiffs[0] != cardsDiffs[1]) {  // There is a unique minimal cards absalut difference
		closestPlayerNum = cardsDiffs[0] < cardsDiffs[1] ? 0 : 1;
	}
	else if (playerIdDiffs[0] != playerIdDiffs[1]) { // There is a unique minimal player id absalut difference
		closestPlayerNum = playerIdDiffs[0] < playerIdDiffs[1] ? 0 : 1;
	}
	else {  // Total tie!! Tie-breaker is greater playerId 
		closestPlayerNum = player1->get_player_id() > player2->get_player_id() ? 0 : 1;
	}
	return 1 - 2 * closestPlayerNum; // returns 1 if player1 is closest to refrencePlayer, -1 if player2 is closest to refrencePlayer
}


output_t<int> world_cup_t::get_closest_player(int playerId, int teamId) {
	// Check input is valid
	if (playerId <= 0 || teamId <= 0) {
		return output_t<int>(StatusType::INVALID_INPUT);
	}

	// Get player
	Player* playerPtr;
	TreeStatusType playersByIdSearchResult = playersById.find(playerId, &playerPtr);
	if (playersByIdSearchResult == TreeStatusType::TREE_FAILURE) {
		return output_t<int>(StatusType::FAILURE);
	}

	// Remove player with playerId (refrencePlayer), inorder to find other player that is closest 
	// (distance between refrencePlayer and refrencePlayer is 0 :) )
	playersByScore.remove(*playerPtr);
	// Find closest player
	Player* closestPlayer = playersByScore.find_closest_key(playerPtr, compare_players);
	// Add back the refrencePlayer to tree
	playersByScore.insert(*playerPtr, playerPtr);

	// Couldn't find closest player
	if (closestPlayer == NULL) {
		return output_t<int>(StatusType::FAILURE);
	}

	int closestPlayerId = closestPlayer->get_player_id();
	
	return output_t<int>(closestPlayerId);

}


bool is_team_valid(Team * team) {
	if (team == NULL) {
		return false;
	}
	return team->is_team_valid();
}

output_t<int> world_cup_t::knockout_winner(int minTeamId, int maxTeamId) {
	// Check input is valid
	if (minTeamId < 0 || maxTeamId<0 || minTeamId>maxTeamId) {
		return output_t<int>(StatusType::INVALID_INPUT);
	}
	// Get valid competing teams (between given IDs)
	int numCompetingTeams = 0;
	Team** competingTeams = teams.get_tree_values_ranged_in_order(&numCompetingTeams, minTeamId, maxTeamId, is_team_valid);
	if (numCompetingTeams == 0) {  // If now teams competing, return failure
		return output_t<int>(StatusType::FAILURE);
	}
	TeamScore** competitionArray = new TeamScore * [numCompetingTeams];
	for (int i = 0; i < numCompetingTeams; i++) {
		TeamScore* newTeamScore = new TeamScore;
		newTeamScore->teamId = competingTeams[i]->get_team_id();
		newTeamScore->teamScore = competingTeams[i]->sum_for_match();
		competitionArray[i] = newTeamScore;
	}

	// TODO: check all mathimatical operations and int<->double castings
	int numSubArrays = int(log2((double)numCompetingTeams));
	bool* needSubArrayOfSize = new bool[numSubArrays + 1];
	int tempNumCompetingTeams = numCompetingTeams;
	for (int size = numSubArrays; size >= 0; size--) { // TODO: check lower bound is correct
		if (tempNumCompetingTeams >= pow(2.0, double(size))) {
			needSubArrayOfSize[size] = true;
			tempNumCompetingTeams = tempNumCompetingTeams % int(pow(2.0, double(size)));
		}
		else {
			needSubArrayOfSize[size] = false;
		}
	}

	int* startIndices = new int[numSubArrays+1];
	int* endIndices = new int[numSubArrays+1];

	// Initialize start/end indices
	int currIndex = 0;
	for (int size = numSubArrays; size >= 0; size--) {
		if (needSubArrayOfSize[size]) {
			startIndices[size] = currIndex;
			endIndices[size] = currIndex + int(pow(2.0, double(size))) - 1;
			currIndex += int(pow(2.0, double(size)));
		}
		else {
			startIndices[size] = -1;
			endIndices[size] = -1;
		}
	}
	if (currIndex != numCompetingTeams) {
		// exception
	}

	TeamScore* winner = new TeamScore;
	bool winnerValid = false;
	winner->teamId = 0;
	winner->teamScore = 0;
	winner->subArrayScore = 0;
	if (needSubArrayOfSize[0]) {
		competitionArray[startIndices[0]]->subArrayScore = competitionArray[startIndices[0]]->teamScore;
	}
	for (int round = 0; round < numSubArrays+1; round++) {
		if (needSubArrayOfSize[round]) {
			if (!winnerValid) {
				winner->teamId = competitionArray[startIndices[round]]->teamId;
				winnerValid = true;
				winner->teamScore = competitionArray[startIndices[round]]->subArrayScore + winner->teamScore;
			}
			else{
				if (winner->teamScore < competitionArray[startIndices[round]]->subArrayScore) {
					winner->teamId = competitionArray[startIndices[round]]->teamId;
				}
				winner->teamScore = competitionArray[startIndices[round]]->subArrayScore + winner->teamScore + POINTS_FOR_WIN;
			}
		}
		for (int size = numSubArrays ; size > round; size--) {
			if (!needSubArrayOfSize[size]) {
				continue;
			}
			int middleIndex = startIndices[size] + ((endIndices[size] - startIndices[size]) / 2);
			int leftSum = 0;
			int rightSum = 0;
			for (int i = startIndices[size]; i <= middleIndex; i++) {
				leftSum += competitionArray[i]->teamScore;
			}
			for (int i = middleIndex + 1; i <= endIndices[size]; i++) {
				rightSum += competitionArray[i]->teamScore;
			}
			if (round == 0) {
				for (int i = startIndices[size]; i <= endIndices[size]; i++) {
					competitionArray[i]->subArrayScore = leftSum + rightSum + POINTS_FOR_WIN*size;
				}
			}
			if (leftSum <= rightSum) {
				startIndices[size] = middleIndex + 1;
			}
			else {
				endIndices[size] = middleIndex;
			}
		}
	}

	int winnerId = winner->teamId;

	// Release all helper arrays/variables
	delete winner;
	delete[] startIndices;
	delete[] endIndices;
	delete[] needSubArrayOfSize;
	for (int i = 0; i < numCompetingTeams; i++) {
		delete competitionArray[i];
	}
	delete[] competitionArray;
	delete[] competingTeams;


	return output_t<int>(winnerId);
}