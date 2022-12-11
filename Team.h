#ifndef DATASTRUCTURESWORLDCUP__TEAM_H_
#define DATASTRUCTURESWORLDCUP__TEAM_H_

#define MIN_VLD_PLAYER_NUM 11

#include "Player.h"
#include "AVLTree.h"
#include "wet1util.h"


class Player;

class Team {
private:
	int teamId;
	AvlTree<Player, Player*> playersByScore;
	AvlTree<int, Player*> playersById;
	int points;
	int playerCounter;
	int gamesCounter;
	int goalsCounter;
	int cardsCounter;
	int goalKeeperCounter;
	int topScorerId;
	
public:
	Team(int teamId, int points);
	virtual ~Team() = default;

	// Player management
	StatusType add_player(Player* newPlayer);
	StatusType remove_player(int playerId);
	StatusType update_player_stats(int playerId, int gamesPlayed, int scoredGoals, int cardsReceived);
	void merge_teams(Team* team1, Team* team2);
	void clear_players();
	Player** merge_arrays(Player** arr1, Player** arr2, bool sort_by_id, int arr1_len, int arr2_len);

	// Get methods
	int get_team_id()const;
	int get_points()const;
	output_t<int> get_top_scorer_id()const;
	int get_all_players_count()const;
	int get_games_played()const;
	void get_all_players_id(int* const output)const;
	void get_all_players(Player** const byIdOutput, Player** const byScoreOutput)const;
	int get_team_goals()const;
	int get_team_cards()const;
	int get_team_goalkeepers_num()const;


	// Set methods
	void set_points(int newPoints);
	void set_games_played_after_update(int gamesPlayed);

	// Match management methods
	bool is_team_valid()const;
	int sum_for_match()const;

	// Operators
	bool operator<(const Team& otherTeam) const;
	bool operator>(const Team& otherTeam) const;
	bool operator==(const Team& otherTeam) const;
	bool operator!=(const Team& otherTeam) const;

};

#endif //DATASTRUCTURESWORLDCUP_TEAM_H_