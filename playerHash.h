//
// Created by Basel Gh on 03/01/2023.
//

#ifndef WORLDCUP23A2_CPP_PLAYERHASH_H
#define WORLDCUP23A2_CPP_PLAYERHASH_H
#include "RankTree.h"
#include "DynamicArray.h"
//#include <iostream>
//#include <memory>
#include "AVLTree.h"
#include "Player.h"
#include "UnionFind.h"
#include "treeNode.h"

using std::string;
using std::shared_ptr;

class playerHash {
    ///DynamicArray<AVLTree<Player, UnionTreeNode::CompareNodeByID>> array;
    DynamicArray<AVLTree<UnionTreeNode, UnionTreeNode::CompareNodeByID>> array;
    long arraySize;
    long numberOfElements;
public:
    playerHash() : array(), arraySize(INIT_SIZE), numberOfElements(0) {}
    ~playerHash() = default;

    long getSize() const {
        return numberOfElements;
    }
    unsigned long hash(shared_ptr<UnionTreeNode> p) {
        return p->player->playerId % arraySize;
    }

    void insert(shared_ptr<UnionTreeNode> to_add) {
        numberOfElements++;
        array[hash(to_add)].insert(to_add);

        if (numberOfElements >= arraySize) {
            rehash();
        }
    }

    void remove(shared_ptr<UnionTreeNode> to_remove) {
        numberOfElements--;
        array[hash(to_remove)].remove(to_remove);
//        if((double)numberOfElements <= (double)((arraySize) / 4)){
//            rehashDown();
//        }
    }

    bool find(long id) {
        shared_ptr<UnionTreeNode> toFind = shared_ptr<UnionTreeNode>(new UnionTreeNode(nullptr,shared_ptr<Team>(new Team(-1)),shared_ptr<Player>(new Player(id))));

        return array[hash(toFind)].find(toFind) != nullptr;
    }

    UnionTreeNode* findHead(UnionTreeNode* toFind) {
        while(toFind->father != nullptr)
        {
            toFind = toFind->father;
        }
        return toFind;
    }

    shared_ptr<UnionTreeNode> findPlayerbyId(long id) {
        shared_ptr<UnionTreeNode> toFind = shared_ptr<UnionTreeNode>(new UnionTreeNode(nullptr,shared_ptr<Team>(new Team(-1)),shared_ptr<Player>(new Player(id))));

        return array[hash(toFind)].find(toFind);
    }
    bool find(shared_ptr<UnionTreeNode> to_find) {
        return array[hash(to_find)].find(to_find) != nullptr;
    }

    shared_ptr<UnionTreeNode> findPlayer(shared_ptr<UnionTreeNode> to_find){
        return array[hash(to_find)].find(to_find);
    }

    void rehash() {
        DynamicArray<AVLTree<UnionTreeNode, UnionTreeNode::CompareNodeByID>> newArray(arraySize * 2);
        long oldSize = arraySize;
        arraySize = 2 * arraySize;
        try{
            for (long i = 0; i < oldSize; ++i) {
                for (AVLTree<UnionTreeNode, UnionTreeNode::CompareNodeByID>::AVLIter it = array[i].begin(); it != array[i].end() ; ++it){
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
        DynamicArray<AVLTree<UnionTreeNode, UnionTreeNode::CompareNodeByID>> newArray(arraySize / 2);
        long oldSize = arraySize;
        arraySize = arraySize / 2;
        try{
            for (long i = 0; i < oldSize; ++i) {
                for (AVLTree<UnionTreeNode, UnionTreeNode::CompareNodeByID>::AVLIter it = array[i].begin(); it != array[i].end() ; ++it){
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

    void merge(const playerHash& toMergeFrom) {
        DynamicArray<AVLTree<UnionTreeNode, UnionTreeNode::CompareNodeByID>> newArray((arraySize + toMergeFrom.arraySize) * 2);
        long oldSize = arraySize;
        long toMergeSize = toMergeFrom.arraySize;
        arraySize = (this->numberOfElements + toMergeFrom.numberOfElements) * 2;
        try{
            for (long i = 0; i < oldSize; ++i) {
                for (AVLTree<UnionTreeNode, UnionTreeNode::CompareNodeByID>::AVLIter it = array[i].begin(); it != array[i].end() ; ++it){
                    newArray[hash(*it)].insert(*it);
                }
            }
            for (long i = 0; i < toMergeSize; ++i) {
                for (AVLTree<UnionTreeNode, UnionTreeNode::CompareNodeByID>::AVLIter it = toMergeFrom.array[i].begin(); it != toMergeFrom.array[i].end() ; ++it){
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

#endif //WORLDCUP23A2_CPP_PLAYERHASH_H
