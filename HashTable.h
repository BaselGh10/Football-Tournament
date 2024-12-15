

#ifndef WET2_HASHTABLE_H
#define WET2_HASHTABLE_H
#include "RankTree.h"
#include "DynamicArray.h"
//#include <iostream>
//#include <memory>
#include "AVLTree.h"
#include "Player.h"

using std::string;
using std::shared_ptr;


class HashTable {
    ///DynamicArray<AVLTree<Player, Player::ComparePlayerByID>> array;
    DynamicArray<AVLTree<Player, Player::ComparePlayerByID>> array;
    long arraySize;
    long numberOfElements;
public:
    HashTable() : array(), arraySize(INIT_SIZE), numberOfElements(0) {}
    ~HashTable() = default;

    long getSize() const {
        return numberOfElements;
    }
    unsigned long hash(shared_ptr<Player> p) {
        return p->playerId % arraySize;
    }

    void insert(shared_ptr<Player> to_add) {
        numberOfElements++;
        array[hash(to_add)].insert(to_add);

        if (numberOfElements >= arraySize) {
            rehash();
        }
    }

    void remove(shared_ptr<Player> to_remove) {
        numberOfElements--;
        array[hash(to_remove)].remove(to_remove);
//        if((double)numberOfElements <= (double)((arraySize) / 4)){
//            rehashDown();
//        }
    }

    bool find(long id) {
        shared_ptr<Player> toFind = shared_ptr<Player>(new Player(id));

        return array[hash(toFind)].find(toFind) != nullptr;
    }
    shared_ptr<Player> findPlayerbyId(long id) {
        shared_ptr<Player> toFind = shared_ptr<Player>(new Player(id));

        return array[hash(toFind)].find(toFind);
    }
    bool find(shared_ptr<Player> to_find) {
        return array[hash(to_find)].find(to_find) != nullptr;
    }

    shared_ptr<Player> findPlayer(shared_ptr<Player> to_find){
        return array[hash(to_find)].find(to_find);
    }

    void rehash() {
        DynamicArray<AVLTree<Player, Player::ComparePlayerByID>> newArray(arraySize * 2);
        long oldSize = arraySize;
        arraySize = 2 * arraySize;
        try{
            for (long i = 0; i < oldSize; ++i) {
                for (AVLTree<Player, Player::ComparePlayerByID>::AVLIter it = array[i].begin(); it != array[i].end() ; ++it){
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
        DynamicArray<AVLTree<Player, Player::ComparePlayerByID>> newArray(arraySize / 2);
        long oldSize = arraySize;
        arraySize = arraySize / 2;
        try{
            for (long i = 0; i < oldSize; ++i) {
                for (AVLTree<Player, Player::ComparePlayerByID>::AVLIter it = array[i].begin(); it != array[i].end() ; ++it){
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

    void merge(const HashTable& toMergeFrom) {
        DynamicArray<AVLTree<Player, Player::ComparePlayerByID>> newArray((arraySize + toMergeFrom.arraySize) * 2);
        long oldSize = arraySize;
        long toMergeSize = toMergeFrom.arraySize;
        arraySize = (this->numberOfElements + toMergeFrom.numberOfElements) * 2;
        try{
            for (long i = 0; i < oldSize; ++i) {
                for (AVLTree<Player, Player::ComparePlayerByID>::AVLIter it = array[i].begin(); it != array[i].end() ; ++it){
                    newArray[hash(*it)].insert(*it);
                }
            }
            for (long i = 0; i < toMergeSize; ++i) {
                for (AVLTree<Player, Player::ComparePlayerByID>::AVLIter it = toMergeFrom.array[i].begin(); it != toMergeFrom.array[i].end() ; ++it){
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



#endif //WET2_HASHTABLE_H
