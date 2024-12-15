#include "worldcup23a2.h"
//#include "Team.h"
//#include "Player.h"
//#include "RankTree.h"
//#include "UnionFind.h"
//#include "teamNode.h"

world_cup_t::world_cup_t()
{
    this->allTeams = shared_ptr<RankTree<Team,Team::CompareTeamByID>>(new RankTree<Team,Team::CompareTeamByID>);
    this->allTeamsByAbility = shared_ptr<RankTree<Team,Team::CompareTeamByAbility>>(new RankTree<Team,Team::CompareTeamByAbility>);
    this->uniTeams = shared_ptr<UnionFindTeams>(new UnionFindTeams());
}

world_cup_t::~world_cup_t() = default;


StatusType world_cup_t::add_team(int teamId)
{
	if(teamId <= 0){
        return StatusType::INVALID_INPUT;
    }
    shared_ptr<Team> toCheck = shared_ptr<Team>(new Team(teamId,0, false,0,0,0,0));
    if(this->allTeams->find(toCheck)){
        return StatusType::FAILURE;
    }
    this->allTeams->insert(toCheck);
    this->allTeamsByAbility->insert(toCheck);
    if(!this->uniTeams->theTeams.find(teamId)) {
        shared_ptr<teamNode> tn = shared_ptr<teamNode>(new teamNode(toCheck, permutation_t(), true, nullptr));
        this->uniTeams->theTeams.insert(tn);
    }
    return StatusType::SUCCESS;
}

StatusType world_cup_t::remove_team(int teamId)
{
    if(teamId <= 0){
        return StatusType::INVALID_INPUT;
    }
    shared_ptr<Team> toCheck = shared_ptr<Team>(new Team(teamId));
    if(!this->allTeams->find(toCheck)){
        return StatusType::FAILURE;
    }
    shared_ptr<Team> toCheck2 = shared_ptr<Team>(new Team(teamId, this->allTeams->find(toCheck)->teamAbility));
    shared_ptr<teamNode> toFind = shared_ptr<teamNode>(new teamNode(toCheck));
    shared_ptr<teamNode> tn = this->uniTeams->theTeams.findteamNode(toFind);
    if(this->uniTeams->theTeams.findteamNode(tn)->team->size != 0) {
        this->uniTeams->theTeams.findteamNode(tn)->head->isPlayerActive = false;
    }
    this->uniTeams->theTeams.findteamNode(tn)->head = nullptr;
    this->uniTeams->theTeams.remove(tn);
    this->allTeams->remove(toCheck);
    this->allTeamsByAbility->remove(toCheck2);
    return StatusType::SUCCESS;
}

StatusType world_cup_t::add_player(int playerId, int teamId,
                                   const permutation_t &spirit, int gamesPlayed,
                                   int ability, int cards, bool goalKeeper)
{
    shared_ptr<Team> toCheck = shared_ptr<Team>(new Team(teamId));

    if (playerId <= 0 || teamId <= 0 || cards < 0 || gamesPlayed < 0 || !spirit.isvalid()){
        return StatusType::INVALID_INPUT;
    }

    if(this->uniTeams->thePlayers.find(playerId)){
        return StatusType::FAILURE;
    }


    if (this->uniTeams->thePlayers.find(playerId) == 1 || !this->allTeams->find(toCheck)){
        return StatusType::FAILURE;
    }

    shared_ptr<Team> t1 = this->allTeams->find(toCheck);


    shared_ptr<Player> newPlayer = shared_ptr<Player>(new Player(playerId,teamId,spirit,gamesPlayed,ability,cards,goalKeeper));
    //t1->playersHash.insert(newPlayer);
    //t2->playersHash.insert(newPlayer);
    t1->size++;
    int old = t1->teamAbility;
    this->uniTeams->thePlayers.insert(newPlayer);
    t1->teamAbility += newPlayer->ability;
    t1->teamSpirit = t1->teamSpirit * newPlayer->spirit;

    shared_ptr<Team> toCheck2 = shared_ptr<Team>(new Team(teamId, old));
    shared_ptr<Team> t2 = this->allTeamsByAbility->find(toCheck2);
    t2->teamAbility = t1->teamAbility;
    t2->teamSpirit = t1->teamSpirit;
    t2->size = t1->size;


    if(newPlayer->goalKeeper){
        t1->haveKeeper = true;
        t2->haveKeeper = true;
    }

    if(t1->size == 1){
        shared_ptr<UnionTreeNode> np = shared_ptr<UnionTreeNode>(new UnionTreeNode(nullptr,t1,newPlayer, true,spirit));
        shared_ptr<teamNode> toFind = shared_ptr<teamNode>(new teamNode(t1));
        shared_ptr<teamNode> tn = this->uniTeams->theTeams.findteamNode(toFind);
        this->uniTeams->theTeams.findteamNode(tn)->head = np;
        this->uniTeams->theTeams.findteamNode(tn)->head->isPlayerActive = true;
        this->uniTeams->theTeams.findteamNode(tn)->head->spirit = spirit;
        this->uniTeams->members.insert(np);
        this->uniTeams->theTeams.findteamNode(tn)->toMult = spirit;
        this->uniTeams->members.findPlayerbyId(playerId)->valueExtra = gamesPlayed;
        //
        this->uniTeams->members.findPlayerbyId(playerId)->mult = spirit;

    }

    else{
        shared_ptr<UnionTreeNode> node = shared_ptr<UnionTreeNode>(new UnionTreeNode(this->uniTeams->theTeams.findTeamById(t1->teamId)->head.get(),t1,newPlayer, true,spirit));
        this->uniTeams->members.insert(node);


        this->uniTeams->members.findPlayerbyId(playerId)->father = this->uniTeams->theTeams.findTeamById(teamId)->head.get();
        this->uniTeams->members.findPlayerbyId(playerId)->valueExtra = gamesPlayed - this->uniTeams->theTeams.findTeamById(teamId)->head->valueExtra;
        //
        this->uniTeams->members.findPlayerbyId(playerId)->mult = (this->uniTeams->theTeams.findTeamById(teamId)->head->spirit.inv()) * this->uniTeams->theTeams.findTeamById(teamId)->head->mult * spirit;
        this->uniTeams->members.findPlayerbyId(playerId)->spirit = this->uniTeams->members.findPlayerbyId(playerId)->mult;
        this->uniTeams->theTeams.findTeamById(teamId)->head->mult = (this->uniTeams->theTeams.findTeamById(teamId)->head->mult) * spirit;

    }

    this->allTeamsByAbility->remove(toCheck2);
    this->allTeamsByAbility->insert(t2);


    return StatusType::SUCCESS;
}

output_t<int> world_cup_t::play_match(int teamId1, int teamId2)
{
    if(teamId1 <= 0 || teamId2 <= 0 || teamId1 == teamId2){
        return StatusType::INVALID_INPUT;
    }

    shared_ptr<Team> toCheck1 = shared_ptr<Team>(new Team(teamId1));
    shared_ptr<Team> toCheck2 = shared_ptr<Team>(new Team(teamId2));

    if(!this->allTeams->find(toCheck1) || !this->allTeams->find(toCheck2)){
        return StatusType::FAILURE;
    }
    shared_ptr<Team> t1 = this->allTeams->find(toCheck1);
    shared_ptr<Team> t2 = this->allTeams->find(toCheck2);


    int result;

    if (t1->haveKeeper == false || t2->haveKeeper == false){
        return StatusType::FAILURE;
    }

    int t1_ability = t1->points + t1->teamAbility;
    int t2_ability = t2->points + t2->teamAbility;
    if(t1_ability > t2_ability){
        t1->points += 3;
        result = 1;
    }

    else if(t2_ability > t1_ability){
        t2->points += 3;
        result = 3;
    }
    else{
        int t1_st = t1->teamSpirit.strength();
        int t2_st = t2->teamSpirit.strength();

        if(t1_st > t2_st){
            t1->points += 3;
            result = 2;
        }
        else if(t2_st > t1_st){
            t2->points += 3;
            result = 4;
        }
        else{
            t1->points += 1;
            t2->points += 1;
            result = 0;
        }

    }


    this->uniTeams->theTeams.findTeamById(teamId1)->head->valueExtra++;
    this->uniTeams->theTeams.findTeamById(teamId2)->head->valueExtra++;

    this->uniTeams->theTeams.findTeamById(teamId1)->team->gamesPlayed++;
    this->uniTeams->theTeams.findTeamById(teamId2)->team->gamesPlayed++;

    shared_ptr<Team> toCheck3 = shared_ptr<Team>(new Team(teamId1, this->allTeams->find(toCheck1)->teamAbility));
    shared_ptr<Team> t3 = this->allTeamsByAbility->find(toCheck3);
    shared_ptr<Team> toCheck4 = shared_ptr<Team>(new Team(teamId2, this->allTeams->find(toCheck2)->teamAbility));
    shared_ptr<Team> t4 = this->allTeamsByAbility->find(toCheck4);
    t3->points = t1->points;
    t4->points = t2->points;
    t3->gamesPlayed = t1->gamesPlayed;
    t4->gamesPlayed = t2->gamesPlayed;
    output_t<int> out = output_t<int>(result);
    return out;
}

output_t<int> world_cup_t::num_played_games_for_player(int playerId)
{
    if(playerId <= 0){
        return StatusType::INVALID_INPUT;
    }

    if(!this->uniTeams->thePlayers.find(playerId)){
        return StatusType::FAILURE;
    }
    this->uniTeams->find(this->uniTeams->members.findPlayerbyId(playerId).get());
    int result = this->uniTeams->getTotalExtraValue(this->uniTeams->members.findPlayerbyId(playerId).get());
    output_t<int> out = output_t<int>(result);
    return out;
}

StatusType world_cup_t::add_player_cards(int playerId, int cards)
{
    if(playerId <= 0 || cards < 0){
        return StatusType::INVALID_INPUT;
    }

    if(!this->uniTeams->thePlayers.find(playerId)){
        return StatusType::FAILURE;
    }
    if(!this->uniTeams->members.findHead(this->uniTeams->members.findPlayerbyId(playerId).get())->isPlayerActive){
        return StatusType::FAILURE;
    }
    this->uniTeams->thePlayers.findPlayerbyId(playerId)->cards += cards ;
	return StatusType::SUCCESS;
}

output_t<int> world_cup_t::get_player_cards(int playerId)
{
    if(playerId <= 0){
        return StatusType::INVALID_INPUT;
    }

    if(!this->uniTeams->thePlayers.find(playerId)){
        return StatusType::FAILURE;
    }

    int playerCards = (this->uniTeams->thePlayers.findPlayerbyId(playerId))->cards;
    output_t<int> out = output_t<int>(playerCards);
	return out;
}

output_t<int> world_cup_t::get_team_points(int teamId)
{
    if(teamId <= 0){
        return StatusType::INVALID_INPUT;
    }
    shared_ptr<Team> toCheck = shared_ptr<Team>(new Team(teamId));
    if(!this->allTeams->find(toCheck)){
        return StatusType::FAILURE;
    }

    shared_ptr<Team> t1 = this->allTeams->find(toCheck);
    int team_points = t1 ->points;
    output_t<int> out = output_t<int>(team_points);
    return out;
}

output_t<int> world_cup_t::get_ith_pointless_ability(int i)
{

    if(i < 0 || i >= this->allTeamsByAbility->getSize() || this->allTeamsByAbility->getSize() == 0){
        return StatusType::FAILURE;
    }
    int result = this->allTeamsByAbility->select(this->allTeamsByAbility->getRoot(),i+1)->teamId;
    output_t<int> out = output_t<int>(result);
    return out;
}

output_t<permutation_t> world_cup_t::get_partial_spirit(int playerId)
{
    if(playerId <= 0){
        return StatusType::INVALID_INPUT;
    }
    if(!this->uniTeams->thePlayers.find(playerId)){
        return StatusType::FAILURE;
    }
    if(!this->uniTeams->members.findHead(this->uniTeams->members.findPlayerbyId(playerId).get())->isPlayerActive){
        return StatusType::FAILURE;
    }
    this->uniTeams->find(this->uniTeams->members.findPlayerbyId(playerId).get());
    permutation_t result = this->uniTeams->findTotalSpirit(playerId);
    output_t<permutation_t> out = output_t<permutation_t>(result);
    return out;
}

StatusType world_cup_t::buy_team(int teamId1, int teamId2)
{
    if(teamId1 <= 0 || teamId2 <= 0 || teamId1 == teamId2){
        return StatusType::INVALID_INPUT;
    }

    shared_ptr<Team> toCheck1 = shared_ptr<Team>(new Team(teamId1));
    shared_ptr<Team> toCheck2 = shared_ptr<Team>(new Team(teamId2));

    if(!this->allTeams->find(toCheck1) || !this->allTeams->find(toCheck2)){
        return StatusType::FAILURE;
    }
    shared_ptr<Team> t1 = this->allTeams->find(toCheck1);
    shared_ptr<Team> t2 = this->allTeams->find(toCheck2);


    shared_ptr<Team> toCheck3 = shared_ptr<Team>(new Team(teamId1, t1->teamAbility));
    shared_ptr<Team> toCheck4 = shared_ptr<Team>(new Team(teamId2, t2->teamAbility));
    shared_ptr<Team> t3 = this->allTeamsByAbility->find(toCheck3);
    shared_ptr<Team> t4 = this->allTeamsByAbility->find(toCheck4);
    if(t2->size == 0){
        shared_ptr<teamNode> toFind = shared_ptr<teamNode>(new teamNode(t2));
        shared_ptr<teamNode> tn = this->uniTeams->theTeams.findteamNode(toFind);
        this->uniTeams->theTeams.findteamNode(tn)->head = nullptr;
        this->uniTeams->theTeams.remove(tn);
        this->allTeams->remove(t2);
        this->allTeamsByAbility->remove(t4);
        return StatusType::SUCCESS;
    }
    if(t1->size == 0){
        shared_ptr<teamNode> toFind5 = shared_ptr<teamNode>(new teamNode(t1));
        shared_ptr<teamNode> tn5 = this->uniTeams->theTeams.findteamNode(toFind5);
        shared_ptr<teamNode> toFind6 = shared_ptr<teamNode>(new teamNode(t2));
        shared_ptr<teamNode> tn6 = this->uniTeams->theTeams.findteamNode(toFind6);
        //this->uniTeams->theTeams.findteamNode(tn6)->head->isPlayerActive = false;
        tn5->head = tn6->head;
        tn5->head->team = t1;
        t1->size = t2->size;
        this->allTeamsByAbility->remove(t3);
        t1->teamAbility = t2->teamAbility;
        t1->teamSpirit = t2->teamSpirit;
        t3->teamAbility = t2->teamAbility;
        t1->points = t2->points;
        t1->haveKeeper = t2->haveKeeper;
        this->allTeams->remove(t2);
        this->allTeamsByAbility->remove(t4);
        this->allTeamsByAbility->insert(t3);
        this->uniTeams->theTeams.findteamNode(tn6)->head = nullptr;
        this->uniTeams->theTeams.remove(tn6);
        return StatusType::SUCCESS;
    }
    shared_ptr<teamNode> toFind1 = shared_ptr<teamNode>(new teamNode(t1));
    shared_ptr<teamNode> tn1 = this->uniTeams->theTeams.findteamNode(toFind1);
    shared_ptr<teamNode> toFind2 = shared_ptr<teamNode>(new teamNode(t2));
    shared_ptr<teamNode> tn2 = this->uniTeams->theTeams.findteamNode(toFind2);
    //this->uniTeams->theTeams.findteamNode(tn2)->head->isPlayerActive = false;
    int sumPoints = t1->points + t2->points;
    int sumAbility = t1->teamAbility + t2->teamAbility;
    int newSize = t1->size + t2->size;
    permutation_t newSpirit = t1->teamSpirit * t2->teamSpirit;
    this->allTeams->remove(t1);
    this->allTeams->remove(t2);
    this->allTeamsByAbility->remove(t3);
    this->allTeamsByAbility->remove(t4);
    shared_ptr<Team> newTeam = this->uniTeams->unionTeams(teamId1, teamId2);
    this->uniTeams->theTeams.findteamNode(tn2)->head = nullptr;
    this->uniTeams->theTeams.remove(tn2);
    newTeam->points = sumPoints;
    newTeam->teamAbility = sumAbility;
    newTeam->size = newSize;
    newTeam->teamSpirit = newSpirit;
    if(t1->haveKeeper || t2->haveKeeper){
        newTeam->haveKeeper = true;
    }


    /*this->allTeams->remove(t1);
    this->allTeams->remove(t2);
    this->allTeamsByAbility->remove(t3);
    this->allTeamsByAbility->remove(t4);*/

    this->allTeams->insert(newTeam);
    this->allTeamsByAbility->insert(newTeam);


    return StatusType::SUCCESS;
}
