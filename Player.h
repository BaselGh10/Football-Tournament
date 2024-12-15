//
// Created by Basel Gh on 25/12/2022.
//

#ifndef CLASSES_H_PLAYER_H
#define CLASSES_H_PLAYER_H
#include "wet2util.h"

class Player{
public:
    int playerId;
    int teamId;
    const permutation_t spirit;
    int gamesPlayed;
    int ability;
    int cards;
    bool goalKeeper;


    class ComparePlayerByID {
    public:
        bool operator()(std::shared_ptr<Player> p1, std::shared_ptr<Player> p2) const {
            return p1->playerId < p2->playerId;
        }
    };

    Player(int playerId):playerId(playerId){};
    Player(int playerId, int teamId, const permutation_t spirit, int gamesPlayed, int ability, int cards, bool goalKeeper):
            playerId(playerId), teamId(teamId), spirit(spirit), gamesPlayed(gamesPlayed), ability(ability), cards(cards), goalKeeper(goalKeeper) {};
    ~Player() = default;
    //friend bool operator==(const Player& p1, const Player& p2);
    bool operator==(const Player& p2) {
        return this->playerId == p2.playerId;
    }

};
/*bool operator==(const Player& p1, const Player& p2) {
    return p1.playerId == p2.playerId;
}*/

#endif //CLASSES_H_PLAYER_H
