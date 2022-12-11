#include "Team.h"


Team::Team(int teamId, int points) : teamId(teamId), points(points) {
	playerCounter = 0;
	gamesCounter = 0;
	goalsCounter = 0;
	cardsCounter = 0;
	goalKeeperCounter = 0;
	topScorerId = 0;
	playersByScore = AvlTree<Player, Player*>();
	playersById = AvlTree<int, Player*>();
}


// Player Management Methods
StatusType Team::add_player(Player* newPlayer) {
	int playerId = newPlayer->get_player_id();
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
		if (playersByIdRemoveResult != TreeStatusType::TREE_SUCCESS) {
			// throw exception
		}
		return StatusType::FAILURE;
	}
	else if (playersByScoreAddResult == TreeStatusType::TREE_ALLOCATION_ERROR) {
		delete newPlayer;
		TreeStatusType playersByIdRemoveResult = playersById.remove(playerId);
		if (playersByIdRemoveResult != TreeStatusType::TREE_SUCCESS) {
			// throw exception
		}
		return StatusType::ALLOCATION_ERROR;
	}
	goalsCounter += newPlayer->get_goals();
	cardsCounter += newPlayer->get_cards();
	if (newPlayer->is_goal_keeper()) {
		goalKeeperCounter++;
	}
	// Update top scorer
	Player* topScorer = playersByScore.find_max();
	if (topScorer != NULL) {
		topScorerId = topScorer->get_player_id();
	}
	playerCounter++;
	return StatusType::SUCCESS;
}


StatusType Team::remove_player(int playerId) {
	Player* playerPtr;
 	TreeStatusType playersByIdSearchResult = playersById.find(playerId, &playerPtr);
	if (playersByIdSearchResult == TreeStatusType::TREE_FAILURE) {
		return StatusType::FAILURE;
	}
	goalsCounter -= playerPtr->get_goals();
	cardsCounter -= playerPtr->get_cards();
	if (playerPtr->is_goal_keeper()) {
		goalKeeperCounter--;
	}
	TreeStatusType playersByScoreRemoveResult = playersByScore.remove(*playerPtr);
	TreeStatusType playersByIdRemoveResult = playersById.remove(playerId);
	if (playersByScoreRemoveResult != TreeStatusType::TREE_SUCCESS || playersByIdRemoveResult != TreeStatusType::TREE_SUCCESS) {
		return StatusType::FAILURE;
	}
	// Update top scorer
	Player* topScorer = playersByScore.find_max();
	if (topScorer != NULL) {
		topScorerId = topScorer->get_player_id();
	}
	playerCounter--;
	return StatusType::SUCCESS;
}

StatusType Team::update_player_stats(int playerId, int gamesPlayed, int scoredGoals, int cardsReceived) {
	// Find player
	Player* playerPtr;
	TreeStatusType playersByIdSearchResult = playersById.find(playerId, &playerPtr);
	if (playersByIdSearchResult == TreeStatusType::TREE_FAILURE) {
		return StatusType::FAILURE;
	}

	// Remove player from tree ordered by score before updating stats
	TreeStatusType playersByScoreRemoveResult = playersByScore.remove(*playerPtr);
	if (playersByScoreRemoveResult != TreeStatusType::TREE_SUCCESS) {
		return StatusType::FAILURE;
	}

	// Update stats
	playerPtr->set_goals(playerPtr->get_goals() + scoredGoals);
	playerPtr->set_cards(playerPtr->get_cards() + cardsReceived);
	playerPtr->set_games_played(playerPtr->get_games_played() + gamesPlayed);

	// Re-insert player to score ordered tree after stats update
	TreeStatusType playersByScoreAddResult = playersByScore.insert(*playerPtr, playerPtr);
	if (playersByScoreAddResult == TreeStatusType::TREE_FAILURE) {
		return StatusType::FAILURE;
	}
	// Update top scorer
	Player* topScorer = playersByScore.find_max();
	if (topScorer != NULL) {
		topScorerId = topScorer->get_player_id();
	}
	goalsCounter += scoredGoals;
	cardsCounter += cardsReceived;
	return StatusType::SUCCESS;
}


// Get Methods_______________________________________________________________________________________________________
int Team::get_team_id()const {
	return teamId;
}

int Team::get_points()const {
	return points;
}

output_t<int> Team::get_top_scorer_id()const {
	if (topScorerId == 0) {
		return output_t<int>(StatusType::FAILURE);
	}
	return output_t<int>(topScorerId);
}

int Team::get_all_players_count()const {
	return playerCounter;
}

int Team::get_games_played()const {
	return gamesCounter;
}

void Team::get_all_players_id(int* const output)const {
	Player* allPlayers = new Player [playerCounter];
	playersByScore.get_tree_keys_in_order(allPlayers);
	for (int playerIdx = 0; playerIdx < playerCounter; playerIdx++) {
		output[playerIdx] = allPlayers[playerIdx].get_player_id();
	}
	delete[] allPlayers;
}

int Team::get_team_goals()const {
	return goalsCounter;
}


int Team::get_team_cards()const {
	return cardsCounter;
}

int Team::get_team_goalkeepers_num()const {
	return goalKeeperCounter;
}

// Set Methods______________________________________________________________________________________________________
void Team::set_points(int newPoints) {
	points = newPoints;
}

void Team::set_games_played_after_update(int gamesPlayed) {
	gamesCounter = gamesPlayed;
}

// Operators_______________________________________________________________________________________________________
bool Team::operator<(const Team& otherTeam) const {
	return teamId < otherTeam.teamId;
}

bool Team::operator>(const Team& otherTeam) const {
	return teamId > otherTeam.teamId;
}

bool Team::operator==(const Team& otherTeam) const {
	return teamId == otherTeam.teamId;
}

bool Team::operator!=(const Team& otherTeam) const {
	return teamId != otherTeam.teamId;
}

bool Team::is_team_valid()const {
	return (playerCounter >= MIN_VLD_PLAYER_NUM) && (goalKeeperCounter > 0);
}

void Team::clear_players() {
	int numPlayers = playerCounter; // Total number of players before clear
	int* playerIds = new int[playerCounter];
	this->get_all_players_id(playerIds);
	for (int playerIdx = 0; playerIdx < numPlayers; playerIdx++) {
		this->remove_player(playerIds[playerIdx]);
	}
	delete[] playerIds;
}

void Team::merge_teams(Team* team1, Team* team2) {
	// Check input is valid
	if (team1 == NULL || team2 == NULL) {
		return;
	}

	// Number of players in each team
	int numPlayersTeam1 = team1->get_all_players_count();
	int numPlayersTeam2 = team2->get_all_players_count();

	if (numPlayersTeam1 + numPlayersTeam2 == 0) {
		return;
	}

	// Get players of each team, sorted by id and by score
	Player** playersByIdTeam1 = new Player * [numPlayersTeam1];
	Player** playersByIdTeam2 = new Player * [numPlayersTeam2];
	Player** playersByScoreTeam1 = new Player * [numPlayersTeam1];
	Player** playersByScoreTeam2 = new Player * [numPlayersTeam2];
	team1->get_all_players(playersByIdTeam1, playersByScoreTeam1);
	team2->get_all_players(playersByIdTeam2, playersByScoreTeam2);

	// Update player's games played at join
	for (int team1PlayerIdx = 0; team1PlayerIdx < numPlayersTeam1; team1PlayerIdx++) {
		int currGamesPlayed1 = playersByIdTeam1[team1PlayerIdx]->get_games_played();
		playersByIdTeam1[team1PlayerIdx]->set_games_played(currGamesPlayed1);
	}
	for (int team2PlayerIdx = 0; team2PlayerIdx < numPlayersTeam2; team2PlayerIdx++) {
		int currGamesPlayed2 = playersByIdTeam2[team2PlayerIdx]->get_games_played();
		playersByIdTeam2[team2PlayerIdx]->set_games_played(currGamesPlayed2);
	}
	// Merge sorted Player pointer arrays into single sorted array (for sort by id and by score)
	Player** playersMergedById = merge_arrays(playersByIdTeam1, playersByIdTeam2, true, numPlayersTeam1, numPlayersTeam2);
	Player** playersMergedByScore = merge_arrays(playersByScoreTeam1, playersByScoreTeam2, false, numPlayersTeam1, numPlayersTeam2);

	int* sortedPlayerIds = new int[numPlayersTeam1 + numPlayersTeam2];  	// Create array of player IDs, sorted
	Player* sortedPlayers = new Player[numPlayersTeam1 + numPlayersTeam2];  // Create array of players, sorted by score
	for (int i = 0; i < numPlayersTeam1 + numPlayersTeam2; i++) {
		// Create sorted key arrays (id and score) for merged value array
		sortedPlayerIds[i] = playersMergedById[i]->get_player_id();
		Player* sortedPlayer = new Player(*playersMergedByScore[i]);
		sortedPlayers[i] = *sortedPlayer;
		// Upate team of players
		playersMergedById[i]->set_team(this);
		playersMergedByScore[i]->set_team(this);
	}

	// Create merged trees out of sorted merged array
	TreeStatusType playersByIdMergeResult = playersById.create_tree_from_sorted_array(sortedPlayerIds, playersMergedById, numPlayersTeam1 + numPlayersTeam2);
	TreeStatusType playersByScoreMergeResult = playersByScore.create_tree_from_sorted_array(sortedPlayers, playersMergedByScore, numPlayersTeam1 + numPlayersTeam2);
	if (playersByIdMergeResult != TreeStatusType::TREE_SUCCESS || playersByScoreMergeResult != TreeStatusType::TREE_SUCCESS) {
		// throw exception
	}

	delete[] playersByIdTeam1;
	delete[] playersByIdTeam2;
	delete[] playersByScoreTeam1;
	delete[] playersByScoreTeam2;
	delete[] sortedPlayerIds;
	delete[] sortedPlayers;


	// Update other feilds
	playerCounter = numPlayersTeam1 + numPlayersTeam2;
	goalsCounter = team1->get_team_goals() + team2->get_team_goals();
	cardsCounter = team1->get_team_cards() + team2->get_team_cards();
	goalKeeperCounter = team1->get_team_goalkeepers_num() + team2->get_team_goalkeepers_num();
	topScorerId = playersByScore.find_max()->get_player_id();
}


Player** Team::merge_arrays(Player** arr1, Player** arr2, bool sortById, int arr1Len, int arr2Len) {
	Player** mergedArr = new Player* [arr1Len + arr2Len];
	int i = 0, j = 0, k = 0;

	while (i < arr1Len && j < arr2Len)
	{
		// Pick the smaller element and put it in mergedArr
		if (sortById ? arr1[i]->get_player_id() < arr2[j]->get_player_id() : *arr1[i] < *arr2[j])
		{
			mergedArr[k] = arr1[i];
			i++;
		}
		else
		{
			mergedArr[k] = arr2[j];
			j++;
		}
		k++;
	}

	// Insert leftovers from both input arrays
	while (i < arr1Len)
	{
		mergedArr[k] = arr1[i];
		i++; k++;
	}

	while (j < arr2Len)
	{
		mergedArr[k] = arr2[j];
		j++; k++;
	}
	return mergedArr;
}

void Team::get_all_players(Player** const byIdOutput, Player** const byScoreOutput)const {
	playersById.get_tree_values_in_order(byIdOutput);
	playersByScore.get_tree_values_in_order(byScoreOutput);
}

int Team::sum_for_match()const {
	return points + (goalsCounter - cardsCounter);
}