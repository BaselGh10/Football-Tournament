//
// Created by Basel Gh on 31/12/2022.
//

#ifndef WORLDCUP23A2_CPP_TEAMNODE_H
#define WORLDCUP23A2_CPP_TEAMNODE_H
#include "Team.h"
#include "treeNode.h"


class teamNode {
public:
    shared_ptr<Team> team;
    permutation_t toMult;
    bool isActive;
    shared_ptr<UnionTreeNode> head;

    class CompareTeamNodeByID {
    public:
        bool operator()(shared_ptr<teamNode> t1, shared_ptr<teamNode> t2) const {
            return t1->team->teamId < t2->team->teamId;
        }
    };

    teamNode(shared_ptr<Team> team):team(team){}
    teamNode(shared_ptr<Team> team, permutation_t toMult, bool isActive, shared_ptr<UnionTreeNode> head):team(team),toMult(toMult),isActive(isActive), head(head){}
    ~teamNode() = default;

    //friend bool operator==(const teamNode& t1, const teamNode& t2);
    bool operator==(const teamNode& t2) {
        return this->team->teamId == t2.team->teamId;
    }

};

/*bool operator==(const teamNode& t1, const teamNode& t2) {
    return t1.team->teamId == t2.team->teamId;
}*/

#endif //WORLDCUP23A2_CPP_TEAMNODE_H
