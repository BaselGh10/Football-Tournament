
#ifndef AVLTREE_H_UNIONFINDTEAMS_H
#define AVLTREE_H_UNIONFINDTEAMS_H

//#include <memory>
//#include <iostream>
#include "teamHash.h"
//#include "playerHash.h"
#include "playerHash.h"

using std::shared_ptr;
using std::weak_ptr;

/*class UnionTreeNode {
public:
    UnionTreeNode* father;
    shared_ptr<Team> team;
    shared_ptr<Player> player;
    ///double valueExtra;
    //weak_ptr<Team> theTeam;
    bool isPlayerActive;
    long size; ///
    permutation_t spirit;
    permutation_t spiritUntil;   /// 90% this will be deleted

    class CompareNodeByID {
    public:
        bool operator()(std::shared_ptr<UnionTreeNode> p1, std::shared_ptr<UnionTreeNode> p2) const {
            return p1->player->playerId < p2->player->playerId;
        }
    };

    UnionTreeNode() = default;
    UnionTreeNode(UnionTreeNode* father, shared_ptr<Team> team, shared_ptr<Player> player): father(father), team(team), player(player){}
    ~UnionTreeNode() = default;

    bool operator==(const UnionTreeNode& p2) {
        return this->player->playerId == p2.player->playerId;
    }



};*/



class UnionFindTeams
{

public:

    HashTable thePlayers;
    teamHash theTeams;
    playerHash members;


    /*explicit UnionFindTeams(): members(new DynamicArray<UnionTreeNode*>()){
        for (long i = 1; i <= members->size; ++i) {

            (*members)[i] = new UnionTreeNode();
            (*members)[i]->father = nullptr;
            (*members)[i]->team = shared_ptr<Team>(new Team(-i));
            (*members)[i]->player = shared_ptr<Player>(new Player(-i));  /// we added this
            ///(*members)[i]->valueExtra = 0;
            //(*members)[i]->theTeam = (*members)[i]->team;
            (*members)[i]->isPlayerActive = false;   ///
            (*members)[i]->size = 1;    /// what???
            (*members)[i]->spirit = (*members)[i]->player->spirit;
            (*members)[i]->spiritUntil = permutation_t();


        }

    }*/

    /*~UnionFindTeams(){
        for (long i = 1; i <=members->size ; ++i) {
            //(*members)[i]->team.reset();
            (*members)[i]->player.reset();
            delete (*members)[i];
        }
        members = nullptr;
    }*/
    UnionFindTeams() = default;
    ~UnionFindTeams() = default;

    void updateExtraValue(UnionTreeNode* toUpdate, UnionTreeNode* root)
    {
        if(toUpdate->father == nullptr || toUpdate == root)
        {
            return;
        }

        //UnionTreeNode* toUpdate2 = toUpdate;
        UnionTreeNode* toUpdate3 = toUpdate;

        double sumExtra1 = toUpdate->valueExtra;
        UnionTreeNode* firstNode = toUpdate;
        while(toUpdate->father != root){
            toUpdate = toUpdate->father;
            sumExtra1 += toUpdate->valueExtra;
        }
        toUpdate = firstNode;
        toUpdate->valueExtra = sumExtra1;


        /*permutation_t sumExtra2 = toUpdate2->spirit;
        UnionTreeNode* firstNode2 = toUpdate2;
        while(toUpdate2->father != root){
            toUpdate2 = toUpdate2->father;
            sumExtra2 = sumExtra2 * toUpdate2->spirit;
        }

        toUpdate2 = firstNode2;
        toUpdate2->spirit = sumExtra2;*/


        permutation_t sumExtra3 = toUpdate3->spirit;
        UnionTreeNode* firstNode3 = toUpdate3;
        while(toUpdate3->father != root){
            toUpdate3 = toUpdate3->father;
            sumExtra3 = toUpdate3->spirit * sumExtra3;
        }

        toUpdate3 = firstNode3;
        toUpdate3->spirit = sumExtra3;


    }

    //returns the root of toFind in the uptree
    UnionTreeNode* find(UnionTreeNode* toFind) {
        if(toFind->father == nullptr)
        {
            return toFind;
        }
        UnionTreeNode* root = find(toFind->father);
        updateExtraValue(toFind, root);
        toFind->father = root;
        return root;
    }


    //returns the team of the current player
    shared_ptr<Team> findTeam(long id) {
        shared_ptr<UnionTreeNode> toFind = members.findPlayerbyId(id);    //find((*members)[id]);        // was * not shared
        //return toFind->theTeam.lock();
        return toFind->team;
    }

    UnionTreeNode* findPlayer(long id) {
        //UnionTreeNode* toFind = find((*members)[id]);
        UnionTreeNode* toFind = members.findPlayerbyId(id).get();
        //return toFind->theTeam.lock();
        return toFind;
    }

    permutation_t findTotalSpirit(long id) {

        UnionTreeNode* toFind = members.findPlayerbyId(id).get();
        UnionTreeNode* toFind2 = toFind;
       // permutation_t result = toFind->spirit;  //// spirit
        //toFind = toFind->father;

        while(toFind->father){
            //result = result * toFind->spirit;
            toFind = toFind->father;
        }
        permutation_t result = toFind->spirit;  //// spirit
        while(toFind2->father){
            result = result * toFind2->spirit;
            toFind2 = toFind2->father;
        }
        return result;
    }


    bool IsPlayerActive(long id) {
        shared_ptr<UnionTreeNode> toFind = members.findPlayerbyId(id);
        return toFind->isPlayerActive;
    }


    double getTotalExtraValue(UnionTreeNode* toFind){
        double totalValue = 0;
        while (toFind != nullptr){
            totalValue += toFind->valueExtra;
            toFind = toFind->father;
        }
        return totalValue;
    }



    shared_ptr<Team> unionTeams(long buyerID, long boughtID)
    {
        shared_ptr<teamNode> sTeam = this->theTeams.findTeamById(boughtID);
        shared_ptr<teamNode> bTeam = this->theTeams.findTeamById(buyerID);


        if(this->theTeams.findTeamById(boughtID)->team->size <= this->theTeams.findTeamById(buyerID)->team->size){
            sTeam->head->valueExtra -= bTeam->head->valueExtra;

            sTeam->head->spirit = (bTeam->head->spirit).inv() * bTeam->head->mult * sTeam->head->spirit;
            bTeam->head->mult = bTeam->head->mult * sTeam->head->mult;

            sTeam->head->father =(bTeam->head).get();
            //sTeam->head = bTeam->head;
        }

        else{

            bTeam->head->valueExtra -= sTeam->head->valueExtra;


            sTeam->head->spirit = bTeam->head->mult * sTeam->head->spirit;
            bTeam->head->spirit = (sTeam->head->spirit).inv() * bTeam->head->spirit;
            sTeam->head->mult = bTeam->head->mult * sTeam->head->mult;

            bTeam->head->father = (sTeam->head).get();
            bTeam->head = sTeam->head;
            bTeam->head->team = bTeam->team;
        }


        return bTeam->head->team;
    }

};

#endif //AVLTREE_H_UNIONFINDTEAMS_H
