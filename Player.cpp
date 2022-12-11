#include "Player.h"
Player::Player(int playerId, int gamesPlayed, int goals, int cards, bool goalKeeper, Team* team) : playerId(playerId), goals(goals), cards(cards), goalKeeper(goalKeeper), initialGamesPlayed(gamesPlayed), team(team) {
    gamesPlayedAtJoin = team->get_games_played();
}


Player::Player() {
    playerId = 0;
    goals = 0;
    cards = 0;
    goalKeeper = false;
    initialGamesPlayed = 0;
    gamesPlayedAtJoin = 0;
    team = nullptr;  // TODO: check if nessesary
}

Player::Player(Player& refPlayer) {
    playerId = refPlayer.get_player_id();
    goals = refPlayer.get_goals();
    cards = refPlayer.get_cards();
    goalKeeper = refPlayer.is_goal_keeper();
    initialGamesPlayed = refPlayer.initialGamesPlayed;
    gamesPlayedAtJoin = refPlayer.gamesPlayedAtJoin;
    team = refPlayer.get_team();
}

Player::~Player() {
    team = nullptr;  // TODO: check if nessesary
}

// Get Methods___________________________________________________________________________________________________
int Player::get_player_id()const {
    return playerId;
}

int Player::get_games_played()const {
    return initialGamesPlayed + (team->get_games_played() - gamesPlayedAtJoin);
}

int Player::get_goals()const {
    return goals;
}

int Player::get_cards()const {
    return cards;
}

bool Player::is_goal_keeper()const {
    return goalKeeper;
}

Team* Player::get_team()const {
    return team;
}


// Set Methods_______________________________________________________________________________________________________
void Player::set_goals(int newGoals) {
    goals = newGoals;
}

void Player::set_cards(int newCards) {
    cards = newCards;
}

void Player::set_games_played(int newGamesPlayed) {
    initialGamesPlayed = newGamesPlayed;
}

void Player::set_team(Team* newTeam) {
    team = newTeam;
}

// Operators____________________________________________________________________________________________________________
bool Player::operator<(const Player& otherPlayer) const {
    if (goals < otherPlayer.goals)
    {
        return true;
    }
    if (goals == otherPlayer.goals)
    {
        if (cards > otherPlayer.cards) {
            return true;
        }
        if (cards == otherPlayer.cards) {
            return (otherPlayer.playerId > playerId);
        }
    }
    return false;
}

bool Player::operator>(const Player& otherPlayer) const
{
    if (goals > otherPlayer.goals)
    {
        return true;
    }
    if (goals == otherPlayer.goals)
    {
        if (cards < otherPlayer.cards) {
            return true;
        }
        if (cards == otherPlayer.cards) {
            return (otherPlayer.playerId < playerId);
        }
    }
    return false;
}

bool Player::operator==(const Player& otherPlayer) const
{
    return playerId == otherPlayer.playerId;
}

bool Player::operator!=(const Player& otherPlayer) const
{
    return playerId != otherPlayer.playerId;
}
