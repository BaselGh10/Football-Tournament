//
// Created by Basel Gh on 31/12/2022.
//

#ifndef WORLDCUP23A2_CPP_TEAMHASH_H
#define WORLDCUP23A2_CPP_TEAMHASH_H
#include "teamNode.h"

class teamHash {
    ///DynamicArray<AVLTree<teamNode, teamNode::CompareTeamNodeByID>> array;
    DynamicArray<AVLTree<teamNode, teamNode::CompareTeamNodeByID>> array;
    long arraySize;
    long numberOfElements;
public:
    teamHash() : array(), arraySize(INIT_SIZE), numberOfElements(0) {}
    ~teamHash() = default;

    long getSize() const {
        return numberOfElements;
    }
    unsigned long hash(shared_ptr<teamNode> p) {
        return p->team->teamId % arraySize;
    }

    void insert(shared_ptr<teamNode> to_add) {
        numberOfElements++;
        array[hash(to_add)].insert(to_add);

        if (numberOfElements >= arraySize) {
            rehash();
        }
    }

    void remove(shared_ptr<teamNode> to_remove) {
        numberOfElements--;
        array[hash(to_remove)].remove(to_remove);
//        if((double)numberOfElements <= (double)((arraySize) / 4)){
//            rehashDown();
//        }
    }

    bool find(long id) {
        shared_ptr<Team> t = shared_ptr<Team>(new Team(id));
        shared_ptr<teamNode> toFind = shared_ptr<teamNode>(new teamNode(t));

        return array[hash(toFind)].find(toFind) != nullptr;
    }

    shared_ptr<teamNode> findTeamById(long id) {
        shared_ptr<Team> t = shared_ptr<Team>(new Team(id));
        shared_ptr<teamNode> toFind = shared_ptr<teamNode>(new teamNode(t));

        return array[hash(toFind)].find(toFind);
    }

    bool find(shared_ptr<teamNode> to_find) {
        return array[hash(to_find)].find(to_find) != nullptr;
    }

    shared_ptr<teamNode> findteamNode(shared_ptr<teamNode> to_find){
        return array[hash(to_find)].find(to_find);
    }

    void rehash() {
        DynamicArray<AVLTree<teamNode, teamNode::CompareTeamNodeByID>> newArray(arraySize * 2);
        long oldSize = arraySize;
        arraySize = 2 * arraySize;
        try{
            for (long i = 0; i < oldSize; ++i) {
                for (AVLTree<teamNode, teamNode::CompareTeamNodeByID>::AVLIter it = array[i].begin(); it != array[i].end() ; ++it){
                    newArray[hash(*it)].insert(*it);
                }
            }
        } catch (...){
            arraySize = oldSize;
            throw;
        }
        this->array.~DynamicArray();
        this->array = newArray;
        newArray.array = nullptr;
    }

    void rehashDown() {
        DynamicArray<AVLTree<teamNode, teamNode::CompareTeamNodeByID>> newArray(arraySize / 2);
        long oldSize = arraySize;
        arraySize = arraySize / 2;
        try{
            for (long i = 0; i < oldSize; ++i) {
                for (AVLTree<teamNode, teamNode::CompareTeamNodeByID>::AVLIter it = array[i].begin(); it != array[i].end() ; ++it){
                    newArray[hash(*it)].insert(*it);
                }
            }
        } catch (...){
            arraySize = oldSize;
            throw;
        }
        (this->array).~DynamicArray();
        this->array = newArray;
        newArray.array = nullptr;

    }

    void merge(const teamHash& toMergeFrom) {
        DynamicArray<AVLTree<teamNode, teamNode::CompareTeamNodeByID>> newArray((arraySize + toMergeFrom.arraySize) * 2);
        long oldSize = arraySize;
        long toMergeSize = toMergeFrom.arraySize;
        arraySize = (this->numberOfElements + toMergeFrom.numberOfElements) * 2;
        try{
            for (long i = 0; i < oldSize; ++i) {
                for (AVLTree<teamNode, teamNode::CompareTeamNodeByID>::AVLIter it = array[i].begin(); it != array[i].end() ; ++it){
                    newArray[hash(*it)].insert(*it);
                }
            }
            for (long i = 0; i < toMergeSize; ++i) {
                for (AVLTree<teamNode, teamNode::CompareTeamNodeByID>::AVLIter it = toMergeFrom.array[i].begin(); it != toMergeFrom.array[i].end() ; ++it){
                    newArray[hash(*it)].insert(*it);
                }
            }
        } catch (...){
            arraySize = oldSize;
            throw;
        }
        (this->array).~DynamicArray();
        this->array = newArray;
        this->numberOfElements += toMergeFrom.numberOfElements;
        newArray.array = nullptr;
    }

};
#endif //WORLDCUP23A2_CPP_TEAMHASH_H
