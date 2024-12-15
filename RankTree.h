

#ifndef WET1_RankTree_H
#define WET1_RankTree_H

#include <iostream>
#include <memory>
//#include <math.h>
using std::shared_ptr;
using std::weak_ptr;

template<class T, class Pred>
class RankTree {
    struct TreeNode {
        shared_ptr<TreeNode> right;
        shared_ptr<TreeNode> left;
        shared_ptr<TreeNode> father;
        shared_ptr<T> data;
        long height;
        long numberOfSons;
        long gradeOfSubtree;
    };

    Pred predicate;
    shared_ptr<TreeNode> root;
    shared_ptr<TreeNode> max;
    shared_ptr<TreeNode> min;
    long size; // initialize size

    void empty() {
        if(size > 0){
            empty_aux(root);
            root = nullptr;
            size = 0;
            min = nullptr;
            max = nullptr;
        }
    }

    void empty_aux(shared_ptr<TreeNode>& toDelete){
        if(!toDelete){
            return;
        }
        empty_aux(toDelete->left);
        empty_aux(toDelete->right);
        if (toDelete->father) {
            toDelete->father.reset();
        }
        if (toDelete->left) {
            toDelete->left.reset();
        }
        if (toDelete->right) {
            toDelete->right.reset();
        }
        toDelete->data.reset();
        toDelete.reset();
    }
    // private functions
    shared_ptr<TreeNode> initNode(shared_ptr<T> data) const {
        shared_ptr<TreeNode> node = shared_ptr<TreeNode>(new TreeNode());

        node->data = data;
        node->left = nullptr;
        node->right = nullptr;
        node->father = nullptr;
        node->height = 0;
        node->numberOfSons = 0;
        //node->gradeOfSubtree = data->grade;
        return node;
    }


    shared_ptr<TreeNode> findNode(shared_ptr<TreeNode> treeNode, shared_ptr<T> toFind) const {
        if (treeNode == nullptr) {
            return nullptr;
        }
        if (treeNode->data == nullptr) {
            return nullptr;
        }
        if (*treeNode->data == *toFind) {
            return treeNode;
        }

        if (predicate(toFind, treeNode->data)) {
            return findNode(treeNode->left, toFind);
        }
        return findNode(treeNode->right, toFind);
    }

    long BF(shared_ptr<TreeNode> treeNode) const {
        long hl = 0, hr = 0;
        if (treeNode->height == 0) {
            return hl -hr;
        }

        if (treeNode->left != nullptr) {
            hl = treeNode->left->height;
        }

        if (treeNode->right != nullptr) {
            hr = treeNode->right->height;
        }

        return hl - hr;
    }

    long getHeight(shared_ptr<TreeNode> node) const {
        return node == nullptr ? -1 : node->height;
    }

    long getNumOfSons(shared_ptr<TreeNode> node) const {
        return node == nullptr ? 0 : node->numberOfSons;
    }

    long getGradeOfSubTree(shared_ptr<TreeNode> node) const {
        return node == nullptr ? 0 : node->gradeOfSubtree;
    }

    shared_ptr<TreeNode> insertNode(shared_ptr<TreeNode> toAdd, shared_ptr<TreeNode> target,
                                    shared_ptr<TreeNode> targetFather) {
        if (target == nullptr) {
            toAdd->father = targetFather;
            toAdd->numberOfSons = 1;
            //toAdd->gradeOfSubtree = toAdd->data->grade;
            return toAdd;
        }

        // toAdd < target
        if (predicate(toAdd->data, target->data)) {
            target->left = insertNode(toAdd, target->left, target);
//            target->numberOfSons += 1;
        } else { // target >= toAdd
            target->right = insertNode(toAdd, target->right, target);
//            target->numberOfSons += 1;
        }
        target->numberOfSons = getNumOfSons(target->left) + getNumOfSons(target->right) + 1;
        //target->gradeOfSubtree = getGradeOfSubTree(target->left) + getGradeOfSubTree(target->right) + target->data->grade;
        target->height = maxInt(getHeight(target->left), getHeight(target->right)) + 1;

        target = balanceTree(target);

        target->height = maxInt(getHeight(target->left), getHeight(target->right)) + 1;
        return target;
    }

    shared_ptr<TreeNode> balanceTree(shared_ptr<TreeNode> target) {
        if (BF(target) >= -1 && BF(target) <= 1) {
            return target;
        }
        if (BF(target) < -1) {
            if (BF(target->right) == 1) {
                rotateRL(target);
            } else {
                rotateRR(target);
            }
        } else if (BF(target) > 1) {
            if (BF(target->left) == -1) {
                rotateLR(target);
            } else {
                rotateLL(target);
            }
        }
        return target->father;
    }

    void removeNode(shared_ptr<TreeNode> toRemove) {
        if (toRemove->right && toRemove->left) {
            shared_ptr<TreeNode> temp = toRemove->right;
            while (temp->left) {
                temp = temp->left;
            }
            shared_ptr<T> tempData = temp->data;
            temp->data = toRemove->data;
            toRemove->data = tempData;

            toRemove = temp;
        }

        shared_ptr<TreeNode> toBalance = toRemove->father;
        removeNodeWithLessThanTwoSons(toRemove);

        while (toBalance) {
            toBalance->height = maxInt(getHeight(toBalance->left), getHeight(toBalance->right)) + 1;
            toBalance->numberOfSons = getNumOfSons(toBalance->right) + getNumOfSons(toBalance->left) + 1;
            //toBalance->gradeOfSubtree = getGradeOfSubTree(toBalance->right) + getGradeOfSubTree(toBalance->left) + toBalance->data->grade;

            balanceTree(toBalance);
            toBalance = toBalance->father;
        }
    }

    void removeNodeWithLessThanTwoSons(shared_ptr<TreeNode> toRemove) {
        if (!toRemove->left && !toRemove->right) { // toRemove is a leaf
            if (!toRemove->father) { // root is a leaf
                this->root = nullptr;
            } else if (toRemove->father->left == toRemove) {
                toRemove->father->left = nullptr;
            } else {
                toRemove->father->right = nullptr;
            }
            return;
        }
        if (!toRemove->left) { // toRemove has right son only
            if (!toRemove->father) { // root is a
                this->root = toRemove->right;
                toRemove->right->father = nullptr;
            } else if (toRemove->father->left == toRemove) {
                toRemove->father->left = toRemove->right;
                toRemove->right->father = toRemove->father;
            } else {
                toRemove->father->right = toRemove->right;
                toRemove->right->father = toRemove->father;
            }
            return;
        }

        if (!toRemove->right) { // toRemove has left son only
            if (!toRemove->father) { // root is a
                this->root = toRemove->left;
                toRemove->left->father = nullptr;
            } else if (toRemove->father->left == toRemove) {
                toRemove->father->left = toRemove->left;
                toRemove->left->father = toRemove->father;
            } else {
                toRemove->father->right = toRemove->left;
                toRemove->left->father = toRemove->father;
            }
        }
    }

    void fixMax() {
        this->max = root;
        if (root) {
            while (this->max->right) {
                this->max = this->max->right;
            }
        }
    }

    void fixMin() {
        this->min = root;
        if (root) {
            while (this->min->left) {
                this->min = this->min->left;
            }
        }
    }

    // rotations
    // **** adjust height of changed nodes.
    void rotateLL(shared_ptr<TreeNode> node) {
        shared_ptr<TreeNode> leftSon = node->left;

        leftSon->father = node->father;
        if (node->father == nullptr) {
            this->root = leftSon;
        } else if (node->father->left == node){
            node->father->left = leftSon;
        } else {
            node->father->right = leftSon;
        }

        node->left = leftSon->right;
        if (node->left != nullptr) {
            node->left->father = node;
        }

        leftSon->right = node;
        node->father = leftSon;

        node->numberOfSons = getNumOfSons(node->right) + getNumOfSons(node->left) + 1;
        //node->gradeOfSubtree = getGradeOfSubTree(node->right) + getGradeOfSubTree(node->left) + node->data->grade;

        leftSon->numberOfSons = getNumOfSons(leftSon->right) + getNumOfSons(leftSon->left) + 1;
        //leftSon->gradeOfSubtree = getGradeOfSubTree(leftSon->right) + getGradeOfSubTree(leftSon->left) + leftSon->data->grade;

        node->height = maxInt(getHeight(node->left), getHeight(node->right)) + 1;
    }

    void rotateRR(shared_ptr<TreeNode> node) {
        shared_ptr<TreeNode> rightSon = node->right;

        rightSon->father = node->father;
        if (node->father == nullptr) {
            this->root = rightSon;
        } else if (node->father->left == node){
            node->father->left = rightSon;
        } else {
            node->father->right = rightSon;
        }

        node->right = rightSon->left;
        if (node->right != nullptr) {
            node->right->father = node;
        }

        rightSon->left = node;
        node->father = rightSon;

        node->numberOfSons = getNumOfSons(node->right) + getNumOfSons(node->left) + 1;
        //node->gradeOfSubtree = getGradeOfSubTree(node->right) + getGradeOfSubTree(node->left) + node->data->grade;

        rightSon->numberOfSons = getNumOfSons(rightSon->right) + getNumOfSons(rightSon->left) + 1;
        //rightSon->gradeOfSubtree = getGradeOfSubTree(rightSon->right) + getGradeOfSubTree(rightSon->left) + rightSon->data->grade;


        node->height = maxInt(getHeight(node->left), getHeight(node->right)) + 1;
    }

    void rotateLR(shared_ptr<TreeNode> node) {
        rotateRR(node->left);
        rotateLL(node);
    }

    void rotateRL(shared_ptr<TreeNode> node) {
        rotateLL(node->right);
        rotateRR(node);
    }


    // aux
    long maxInt(long x1, long x2) const {
        return x1 > x2 ? x1 : x2;
    }

    shared_ptr<TreeNode> findFirstBiggerThanAux(shared_ptr<T> data){
        if(!max || predicate(max->data , data)){
            return nullptr;
        }
        shared_ptr<TreeNode> node = findLastNodeInSearch(root , nullptr , data);
//        if(node->data == data){
//            return node;
//        }
        if(!predicate(node->data , data)){
            return node;
        }
        while(node->father && node->father->right == node){
            node = node->father;
        }
        return node->father;
    }

    void mergeSortedArrays(shared_ptr<T>* toMergeTo , shared_ptr<T>* toMerge1 , shared_ptr<T>* toMerge2
                           , long n1 , long n2) const{
        long i1 = 0 , i2 = 0 , iTo = 0;
        while(i1 < n1 && i2 < n2){
            if(predicate(toMerge1[i1] , toMerge2[i2])){
                toMergeTo[iTo] = toMerge1[i1++];
            }else{
                toMergeTo[iTo] = toMerge2[i2++];
            }
            iTo++;
        }
        for(; i1 < n1 ; i1++ , iTo++){
            toMergeTo[iTo] = toMerge1[i1];
        }
        for(; i2 < n2 ; i2++ , iTo++){
            toMergeTo[iTo] = toMerge2[i2];
        }
    }

    shared_ptr<TreeNode> fromArrayToTree(shared_ptr<T>* toBuild, long start, long end, shared_ptr<TreeNode> father) const{
        if (start > end) {
            return nullptr;
        }

        long mid = (start + end) / 2;

        shared_ptr<TreeNode> node = initNode(toBuild[mid]);
        node->father = father;
        node->left = fromArrayToTree(toBuild, start, mid - 1, node);
        node->right = fromArrayToTree(toBuild, mid + 1, end, node);

        node->height = maxInt(getHeight(node->left), getHeight(node->right)) + 1;
        node->numberOfSons = getNumOfSons(node->right) + getNumOfSons(node->left) + 1;
        node->gradeOfSubtree = getGradeOfSubTree(node->right) + getGradeOfSubTree(node->left) + node->data->grade;

        return node;
    }

    void fromTreeToArray(shared_ptr<T>* toBuildTo) const {
        long i = 0;
        for (AVLIter avlIter = begin(); avlIter != end() ; ++avlIter) {
            toBuildTo[i++] = *avlIter;
        }
    }
public:
    RankTree() : size(0) {}

    ~RankTree() {
        empty();
    }

    class AVLIter;
    // exceptions
    class AlreadyExists : public std::exception {};
    class DoesntExist : public std::exception {};

    // throws exception AlreadyExists if already exists
    void insert(shared_ptr<T> data) {
        if (!root) {
            root = initNode(data);
        } else {
            if (findNode(root, data)) {
                throw AlreadyExists();
            }
            insertNode(initNode(data), root, nullptr);
        }
        ++size;
        fixMax();
        fixMin();
    }

    // returns nullptr if not found
    shared_ptr<T> find(shared_ptr<T> data) const {
        shared_ptr<TreeNode> toFind = findNode(root, data);
        return !toFind ? nullptr : toFind->data;
    }

    // throws DoesntExist
    void remove(shared_ptr<T> data) {
        shared_ptr<TreeNode> toRemove = findNode(root, data);
        if (!toRemove) {
            throw DoesntExist();
        }
        removeNode(toRemove);
        size--;
        fixMax();
        fixMin();
    }

    shared_ptr<T> getMax() const {
        return this->max->data;
    }

    long getSize() const {
        return this->size;
    }

    bool isEmpty() const {
        return this->size == 0;
    }

    shared_ptr<TreeNode> findLastNodeInSearch(shared_ptr<TreeNode> treeNode , shared_ptr<TreeNode> father , shared_ptr<T> toFind){
        if(treeNode == nullptr){
            return father;
        }

        if (*treeNode->data == *toFind) {
            return treeNode;
        }

        if (predicate(toFind, treeNode->data)) {
            return findLastNodeInSearch(treeNode->left,treeNode ,  toFind);
        }
        return findLastNodeInSearch(treeNode->right, treeNode ,toFind);
    }

    RankTree::AVLIter findFirstBiggerThan(shared_ptr<T> data){
        return AVLIter(findFirstBiggerThanAux(data) , max);
    }

    void merge(RankTree<T, Pred>& toMergeFrom) {
        if (this->size == 0) {
            this->root = toMergeFrom.root;
            this->size = toMergeFrom.size;
            fixMax();
            fixMin();
            toMergeFrom.root = nullptr;
            toMergeFrom.size = 0;
            toMergeFrom.max = nullptr;
            toMergeFrom.min = nullptr;
            return;
        }
        if (toMergeFrom.size == 0) {
            return;
        }

        shared_ptr<T>* array1 = new shared_ptr<T>[this->size];
        shared_ptr<T>* array2 = new shared_ptr<T>[toMergeFrom.size];
        shared_ptr<T>* array3 =
                new shared_ptr<T>[this->size + toMergeFrom.size];

        this->fromTreeToArray(array1);
        toMergeFrom.fromTreeToArray(array2);

        mergeSortedArrays(array3, array1, array2, this->size, toMergeFrom.size);

        shared_ptr<TreeNode> newRoot = fromArrayToTree(array3, 0, this->size + toMergeFrom.size - 1, nullptr);

        long oldSize = this->size;
        empty();

        this->root = newRoot;
        fixMax();
        fixMin();
        this->size = oldSize + toMergeFrom.size;

        toMergeFrom.empty();

        delete[] array1;
        delete[] array2;
        delete[] array3;
    }

    long long sumOfGradeBestM(long long m) {
        shared_ptr<TreeNode> temp = root;
        long long sumOfGrades = 0;
        long long numberOfHigherEmployeesLeft = m;
        while (temp != nullptr) {
//            if (numberOfHigherEmployeesLeft == 0) {
//                break;
//            }
            if (getNumOfSons(temp->right) > numberOfHigherEmployeesLeft) {
                temp = temp->right;
            } else if (getNumOfSons(temp->right) < numberOfHigherEmployeesLeft){
                sumOfGrades += temp->data->grade + getGradeOfSubTree(temp->right);
                numberOfHigherEmployeesLeft -= 1 + getNumOfSons(temp->right);
                temp = temp->left;
            } else {
                sumOfGrades += getGradeOfSubTree(temp->right);
                break;
            }
        }
        return sumOfGrades;
    }

    long long getSumGradeUntilKth(shared_ptr<T> kthEmp) {
        long long sumGrade = 0;
        shared_ptr<TreeNode> temp = root;

        while (temp != nullptr) {
            if (temp->data->salary == kthEmp->salary) {
                sumGrade += temp->data->grade + getGradeOfSubTree(temp->left);
                temp = temp->right;
            } else if (predicate(kthEmp, temp->data)) {
                temp = temp->left;
            } else {
                sumGrade += temp->data->grade + getGradeOfSubTree(temp->left);
                temp = temp->right;
            }
        }
        return sumGrade;
    }

    shared_ptr<TreeNode> getRoot(){
        return root;
    }

    shared_ptr<T> select(shared_ptr<TreeNode> root, long long rank) {
        if (root == nullptr) {
            return nullptr;
        }
        if(root->left) {
            if (getNumOfSons(root->left) + 1 == rank) {
                return root->data;
            }
            if (getNumOfSons(root->left) + 1 > rank) {
                if (!root->left) {
                    return root->data;
                }
                return select(root->left, rank);
            }
            return select(root->right, rank - getNumOfSons(root->left) - 1);
        }
        else{
            if(rank == 1){
                return root->data;
            }
            return select(root->right, rank - 1);
        }
    }


    long long getSumGradeUntilKthNotIncluding(shared_ptr<T> kthEmp) {
        long long sumGrade = 0;
        shared_ptr<TreeNode> temp = root;

        while (temp != nullptr) {
            if (predicate(kthEmp, temp->data) || temp->data->salary == kthEmp->salary) {
                temp = temp->left;
            }else {
                sumGrade += temp->data->grade + getGradeOfSubTree(temp->left);
                temp = temp->right;
            }
        }
        return sumGrade;
    }

    long long getNumOfNodesUntilKth(shared_ptr<T> kthEmp) {
        long long numOfNodes = 0;
        shared_ptr<TreeNode> temp = root;

        while (temp != nullptr) {
            if (temp->data->salary == kthEmp->salary) {
                numOfNodes += 1 + getNumOfSons(temp->left);
                temp = temp->right;
            }
            else if (predicate(kthEmp, temp->data)) {
                temp = temp->left;
            } else {
                numOfNodes += 1 + getNumOfSons(temp->left);
                temp = temp->right;
            }
        }
        return numOfNodes;
    }

    long long getNumOfNodesUntilKthNotInclude(shared_ptr<T> kthEmp) {
        long long numOfNodes = 0;
        shared_ptr<TreeNode> temp = root;

        while (temp != nullptr) {
            if (predicate(kthEmp, temp->data) || temp->data->salary == kthEmp->salary) {
                temp = temp->left;
            } else {
                numOfNodes += 1 + getNumOfSons(temp->left);
                temp = temp->right;
            }
        }
        return numOfNodes;
    }

    // iterator and merge
    class AVLIter {
        shared_ptr<TreeNode> current;
        shared_ptr<TreeNode> finish;
    public:
        AVLIter(shared_ptr<TreeNode> current, shared_ptr<TreeNode> finish) : current(current), finish(finish) {}
        AVLIter() {
            current = nullptr;
            finish = nullptr;
        }

        bool operator==(const AVLIter& iter) {
            return this->current == iter.current;
        }

        bool operator!=(const AVLIter& iter) {
            return this->current != iter.current;
        }

        AVLIter& operator++() {
            if (current == finish) {
                current = nullptr;
            } else {
                if (current->right) {
                    current = current->right;
                    while (current->left) {
                        current = current->left;
                    }
                } else if (!current->father) {
                    current = nullptr;
                } else if (current->father->left == current) {
                        current = current->father;
                    } else {
                        while (current->father->right == current) {
                            current = current->father;
                        }
                        current = current->father;
                    }
                }
            return *this;
        }

        shared_ptr<T> operator*(){
            if (!current) {
                return nullptr;
            }
            return current->data;
        }

        friend class RankTree<T, Pred>;
    };

    AVLIter begin() const {
        return AVLIter(this->min, this->max);
    }

    AVLIter end() const {
        return AVLIter(nullptr , nullptr);
    }
};




#endif //WET1_RankTree_H
