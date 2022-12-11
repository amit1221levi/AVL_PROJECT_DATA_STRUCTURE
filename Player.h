#ifndef DATASTRUCTURESWORLDCUP__PLAYER_H_
#define DATASTRUCTURESWORLDCUP__PLAYER_H_

#include "Team.h"

class Team;

class Player {
private:
	int playerId;
	int goals;
	int cards;
	bool goalKeeper;
	int gamesPlayedAtJoin;
	int initialGamesPlayed;
	Team* team;

public:
	Player(int playerId, int gamesPlayed, int goals, int cards, bool goalKeeper, Team* team);
	Player();
	Player(Player& refPlayer);
	virtual ~Player();

	// Get methods
	int get_player_id()const;
	int get_games_played()const;
	int get_goals()const;
	int get_cards()const;
	Team* get_team()const;
	bool is_goal_keeper()const;

	// Set methods
	void set_games_played(int newGamesPlayed);
	void set_goals(int newGoals);
	void set_cards(int newCards);
	void set_team(Team* team);

	// Operators
	bool operator<(const Player& otherPlayer) const;
	bool operator>(const Player& otherPlayer) const;
	bool operator==(const Player& otherPlayer) const;
	bool operator!=(const Player& otherPlayer) const;
};


#endif //DATASTRUCTURESWORLDCUP_PLAYER_H_