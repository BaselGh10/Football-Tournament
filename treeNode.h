//
// Created by Basel Gh on 03/01/2023.
//

#ifndef WORLDCUP23A2_CPP_TREENODE_H
#define WORLDCUP23A2_CPP_TREENODE_H
#include "Team.h"

class UnionTreeNode {
public:
    UnionTreeNode* father;
    shared_ptr<Team> team;
    shared_ptr<Player> player;
    bool isPlayerActive;
    permutation_t spirit;
    int valueExtra;
    permutation_t mult;



    class CompareNodeByID {
    public:
        bool operator()(std::shared_ptr<UnionTreeNode> p1, std::shared_ptr<UnionTreeNode> p2) const {
            return p1->player->playerId < p2->player->playerId;
        }
    };

    UnionTreeNode() = default;
    UnionTreeNode(UnionTreeNode* father, shared_ptr<Team> team, shared_ptr<Player> player): father(father), team(team), player(player){}
    UnionTreeNode(UnionTreeNode* father, shared_ptr<Team> team, shared_ptr<Player> player,bool isPlayerActive, permutation_t spirit): father(father), team(team), player(player), isPlayerActive(isPlayerActive), spirit(spirit){}
    ~UnionTreeNode() = default;

    bool operator==(const UnionTreeNode& p2) {
        return this->player->playerId == p2.player->playerId;
    }



};

#endif //WORLDCUP23A2_CPP_TREENODE_H
