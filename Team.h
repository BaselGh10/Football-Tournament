//
// Created by Basel Gh on 25/12/2022.
//

#ifndef CLASSES_H_TEAM_H
#define CLASSES_H_TEAM_H
#include "HashTable.h"
//#include "teamHash.h"

class Team{
public:
    int teamId;
    double value;
    int size;
    bool haveKeeper;
    int points;
    int gamesPlayed;
    int teamAbility;
    permutation_t teamSpirit;


    class CompareTeamByAbility {
    public:
        bool operator()(shared_ptr<Team> t1, shared_ptr<Team> t2) const {
            if(t1->teamAbility > t2->teamAbility){
                return false;
            }
            if(t1->teamAbility < t2->teamAbility){
                return true;
            }
            return t1->teamId < t2->teamId;
        }
    };


    class CompareTeamByID {
    public:
        bool operator()(shared_ptr<Team> t1, shared_ptr<Team> t2) const {
            return t1->teamId < t2->teamId;
        }
    };

    Team(int teamId): teamId(teamId) {
        //playersHash = *(new HashTable());
    }

    Team(int teamId, int teamAbility): teamId(teamId), teamAbility(teamAbility){}
    Team(int teamId, double value, bool haveKeeper, int points, int gamesPlayed, int teamAbility, int size): teamId(teamId), value(value), size(size), haveKeeper(haveKeeper), points(points), gamesPlayed(gamesPlayed), teamAbility(teamAbility) {
        teamSpirit = permutation_t().neutral();
        //playersHash = *(new HashTable());
    }
    ~Team() = default;

    //friend bool operator==(const Team& t1, const Team& t2);
    bool operator==(const Team& t2) {
        return this->teamId == t2.teamId;
    }
};

/*bool operator==(const Team& t1, const Team& t2) {
    return t1.teamId == t2.teamId;
}*/


#endif //CLASSES_H_TEAM_H
