
#ifndef WET1_AVLTREE_H
#define WET1_AVLTREE_H

#include <stdexcept>
#include <cassert>

enum struct TreeStatusType {
    TREE_SUCCESS = 0,
    TREE_FAILURE = -1,
    TREE_ALLOCATION_ERROR = -2,
    TREE_INVALID_INPUT = -3
} ;

template <class KeyType, class ValueType>
struct Node {
    KeyType* key;
    ValueType* value;
    Node* parent;
    Node* left;
    Node* right;
    int height;
};

template <class KeyType, class ValueType>
class AvlTree {
    // Real root is right son of root
    Node<KeyType, ValueType>* root;
    int size;

    // Balances a tree when given the problematic node
    void balance_tree(Node<KeyType, ValueType>* currParentNode);

    //Calculates the balance factor of a node
    int balance_factor(const Node<KeyType, ValueType>* root) const;

    // Does a left left roll
    void left_left_roll(Node<KeyType, ValueType>* root);
    
    // Does a left right roll
    void left_right_roll(Node<KeyType, ValueType>* root);
    
    // Does a right left roll
    void right_left_roll(Node<KeyType, ValueType>* root);
    
    // Does a right right roll
    void right_right_roll(Node<KeyType, ValueType>* root);
    
    // Swaps between two nodes
    void swap_nodes(Node<KeyType, ValueType>* node1, Node<KeyType, ValueType>* node2);
    
    // Updates height of a specific node
    void update_height(Node<KeyType, ValueType>* node);
    
    // Gets nodes into given array in order
    void in_order(Node<KeyType, ValueType>** array,
        Node<KeyType, ValueType>* node, int* counter)const;
    
    // Gets nodes' keys into given array in order
    void keys_in_order(KeyType* array, Node<KeyType, ValueType>* const node, int* counter)const;
    
    // Gets nodes' value into given array in order (by keys)
    void values_in_order(ValueType* array, Node<KeyType, ValueType>* const node, int* counter)const;
    
    // Gets valid nodes' values into given array in ranged order(by keys), for keys between minKey and maxKey
    void values_ranged_in_order(ValueType* array, Node<KeyType, ValueType>* const node, int* counter, KeyType minKey, KeyType maxKey, bool (*validationFunc)(ValueType value))const;
    
    // Gets number of valid nodes' values in ranged order(by keys), for keys between minKey and maxKey
    void num_of_values_ranged_in_order(Node<KeyType, ValueType>* const node, int* counter, KeyType minKey, KeyType maxKey, bool (*validationFunc)(ValueType value))const;
    
    // Gets nodes into given array in pre order
    void pre_order(Node<KeyType, ValueType>** array,
        Node<KeyType, ValueType>* node, int* counter);
    
    // Deletes all tree nodes (except dummy tree)
    void delete_tree_nodes(Node<KeyType, ValueType>* node);
    
     // Copies the nodes of a tree, without dummy root
    void copy_aux(Node<KeyType, ValueType>* copyToParent,
        Node<KeyType, ValueType>* copyTo,
        Node<KeyType, ValueType>** InOrder,
        Node<KeyType, ValueType>** PreOrder, int treeSize);

    // Fills an empty tree with a copy of another tree
    void copy(AvlTree<KeyType, ValueType> const& toCopy,
        AvlTree<KeyType, ValueType>& copyTo);

     // Inserts a given node to the tree in the right place and updates the tree stats
    TreeStatusType place_node(Node<KeyType, ValueType>* toPlace);
 
    // Finds a node with the given key in the tree
    Node<KeyType, ValueType>* find_node_by_key(KeyType const& key) const;
    
    // Removes the root from the tree if its the only node in it
    void remove_root();

    // Removes the leaf from the tree.
    void remove_leaf(Node<KeyType, ValueType>* toRemove);

    // Removes the node from the tree when there is one child
    void remove_one_child(Node<KeyType, ValueType>* toRemove);

    // Removes the node from the tree when there are two children
    void remove_two_children(Node<KeyType, ValueType>* toRemove);

     // Finds the next node inorder from the tree
    Node<KeyType, ValueType>* get_next_in_order(Node<KeyType, ValueType>* node);

public:
    AvlTree<KeyType, ValueType>();
    AvlTree<KeyType, ValueType>(AvlTree<KeyType, ValueType>& tree);
    ~AvlTree();
    AvlTree& operator=(AvlTree<KeyType, ValueType> const& tree);

    TreeStatusType insert(KeyType& key, ValueType& value);
    TreeStatusType find(const KeyType& key, ValueType* value) const;
    TreeStatusType remove(const KeyType& key);
    TreeStatusType remove_by_pointer(Node<KeyType, ValueType>* toDelete);
    TreeStatusType get_size(int* n) const;
    TreeStatusType create_tree_from_sorted_array(KeyType* sortedKeyArray, ValueType* sortedValueArray, int length);
    Node<KeyType, ValueType>* get_root_from_sorted_array(KeyType* sortedKeyArray, ValueType* sortedValueArray, int start, int end, int height, Node<KeyType, ValueType>* parent);
    KeyType* find_max()const;
    KeyType* find_min()const;
    void get_tree_keys_in_order(KeyType* const array)const;
    void get_tree_values_in_order(ValueType* const array)const;
    ValueType* get_tree_values_ranged_in_order(int* counter, KeyType minKey, KeyType maxKey, bool (*validationFunc)(ValueType value))const;
    void get_tree_in_order(Node<KeyType, ValueType>** const array);
    KeyType* get_closest_key(Node<KeyType, ValueType>* node, KeyType* key, KeyType* closestKey, int (*compareFunc)(KeyType* key1, KeyType* key2,KeyType* refKey), bool closestKeyValid) const;
    KeyType* find_closest_key(KeyType* key, int (*compareFunc)(KeyType* key1, KeyType* key2, KeyType* refKey))const;
    
};

// Utitlity Methods
static int max(int num1, int num2) {
    if (num1 > num2) {
        return num1;
    }
    return num2;
}

/****************************************************************************/

// tree balancing funcs
template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::balance_tree(Node<KeyType, ValueType>* currParentNode) {
    int balanceFactor = this->balance_factor(currParentNode);
    if (balanceFactor == 2) {
        if (this->balance_factor(currParentNode->left) == -1) {
            this->left_right_roll(currParentNode);
        }
        else {
            this->left_left_roll(currParentNode);
        }
    }
    if (balanceFactor == -2) {
        if (this->balance_factor(currParentNode->right) == 1) {
            this->right_left_roll(currParentNode);
        }
        else {
            this->right_right_roll(currParentNode);
        }
    }
}

template <class KeyType, class ValueType>
int AvlTree<KeyType, ValueType>::balance_factor(const Node<KeyType, ValueType>* const root) const {
    assert(root != NULL);
    if (root->right == NULL && root->left == NULL) {
        return 0;
    }
    if (root->right == NULL) {
        return root->left->height + 1;
    }
    else if (root->left == NULL) {
        return -1 - root->right->height;
    }
    return root->left->height - root->right->height;
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::left_left_roll(Node<KeyType, ValueType>* root) {
    Node<KeyType, ValueType>* node1 = root;
    Node<KeyType, ValueType>* node2 = root->left;

    if (node1->parent == NULL) {
        this->root->right = node2;
    }
    else {
        bool isRight = (node1 == node1->parent->right);
        if (isRight) {
            node1->parent->right = node2;
        }
        else {
            node1->parent->left = node2;
        }
    }

    node2->parent = node1->parent;
    node1->parent = node2;

    node1->left = node2->right;
    if (node2->right != NULL) {
        node2->right->parent = node1;
    }
    node2->right = node1;

    update_height(node1);
    update_height(node2);
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::right_right_roll(Node<KeyType, ValueType>* const root) {
    Node<KeyType, ValueType>* node1 = root;
    Node<KeyType, ValueType>* node2 = root->right;

    if (node1->parent == NULL) {
        this->root->right = node2;
    }
    else {
        bool isRight = (node1 == node1->parent->right);
        if (isRight) {
            node1->parent->right = node2;
        }
        else {
            node1->parent->left = node2;
        }
    }

    node2->parent = node1->parent;
    node1->parent = node2;


    node1->right = node2->left;
    if (node2->left != NULL) {
        node2->left->parent = node1;
    }
    node2->left = node1;

    update_height(node1);
    update_height(node2);
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::left_right_roll(Node<KeyType, ValueType>* const root) {
    right_right_roll(root->left);
    left_left_roll(root);
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::right_left_roll(Node<KeyType, ValueType>* const root) {
    left_left_roll(root->right);
    right_right_roll(root);
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::swap_nodes(Node<KeyType, ValueType>* const node1, Node<KeyType, ValueType>* const node2) {
    assert(node1 != NULL && node2 != NULL);

    if (!node1->parent) { // True root is right son of tree root
        this->root->right = node2;
    }
    else if (node1->parent->right == node1) {
        node1->parent->right = node2;
    }
    else {
        node1->parent->left = node2;
    }

    if (node2->parent == node1) {
        node2->parent = node1->parent;
        node1->parent = node2;

        node1->right = node2->right;
        node2->right = node1;

        node2->left = node1->left;
        node1->left = NULL;

        int tempH = node1->height;
        node1->height = node2->height;
        node2->height = tempH;

        node2->left->parent = node2;

        if (node1->right) {
            node1->right->parent = node1;
        }
    }
    else {
        // swap_nodes the nodes sons and parent
        Node<KeyType, ValueType>* currParentNode = node1->parent;
        Node<KeyType, ValueType>* tempRight = node1->right;
        Node<KeyType, ValueType>* tempLeft = node1->left;
        int tempHeight = node1->height;

        node1->parent = node2->parent;
        node1->height = node2->height;
        node1->right = node2->right;
        node1->left = node2->left;

        node2->parent = currParentNode;
        node2->height = tempHeight;
        node2->right = tempRight;
        node2->left = tempLeft;

        node2->right->parent = node2;
        node2->left->parent = node2;

        if (node1->right) {
            node1->right->parent = node1;
        }
    }
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::update_height(Node<KeyType, ValueType>* const node) {
    assert(node != NULL);
    if (node->right == NULL && node->left == NULL) {
        node->height = 0;
    }
    else if (node->right == NULL) {
        node->height = 1 + node->left->height;
    }
    else if (node->left == NULL) {
        node->height = 1 + node->right->height;
    }
    else {
        node->height = 1 + max(node->right->height, node->left->height);
    }
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::get_tree_in_order(
    Node<KeyType, ValueType>** const array) {

    int counter = 0;
    in_order(array, this->root->right, &counter);
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::get_tree_keys_in_order(KeyType* const array)const {
    int counter = 0;
    keys_in_order(array, root->right, &counter);
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::get_tree_values_in_order(ValueType* const array)const {
    int counter = 0;
    values_in_order(array, root->right, &counter);
}

template <class KeyType, class ValueType>
ValueType* AvlTree<KeyType, ValueType>::get_tree_values_ranged_in_order(int* counter, KeyType minKey, KeyType maxKey, bool (*validationFunc)(ValueType value))const {
    int arraySize = 0;
    num_of_values_ranged_in_order(root->right, &arraySize, minKey, maxKey, validationFunc);
    ValueType* array = new ValueType[arraySize];
    values_ranged_in_order(array, root->right, counter, minKey, maxKey, validationFunc);
    return array;
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::keys_in_order(KeyType* array, Node<KeyType, ValueType>* const node, int* counter)const {
    if (node == NULL) {
        return;
    }
    keys_in_order(array, node->left, counter);
    KeyType* cpyKey = new KeyType(*node->key);
    array[*counter] = *cpyKey;
    (*counter)++;
    keys_in_order(array, node->right, counter);
}


template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::values_in_order(ValueType* array, Node<KeyType, ValueType>* const node, int* counter)const {
    if (node == NULL) {
        return;
    }
    values_in_order(array, node->left, counter);
    ValueType* cpyValue = new ValueType(*node->value);
    array[*counter] = *cpyValue;
    (*counter)++;
    values_in_order(array, node->right, counter);
}


template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::values_ranged_in_order(ValueType* array, Node<KeyType, ValueType>* const node, int* counter, KeyType minKey, KeyType maxKey, bool (*validationFunc)(ValueType value))const {
    if (node == NULL) {
        return;
    }
    if ((*node->key) > minKey) {
        values_ranged_in_order(array, node->left, counter, minKey, maxKey, validationFunc);
    }
    if ((*node->key) >= minKey && (*node->key) <= maxKey && validationFunc(*(node->value))) {
        array[*counter] = *node->value;
        (*counter)++;
    }
    if ((*node->key) < maxKey) {
        values_ranged_in_order(array, node->right, counter, minKey, maxKey, validationFunc);
    }
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::num_of_values_ranged_in_order(Node<KeyType, ValueType>* const node, int* counter, KeyType minKey, KeyType maxKey, bool (*validationFunc)(ValueType value))const {
    if (node == NULL) {
        return;
    }
    if ((*node->key) > minKey) {
        num_of_values_ranged_in_order(node->left, counter, minKey, maxKey, validationFunc);
    }
    if ((*node->key) >= minKey && (*node->key) <= maxKey && validationFunc(*(node->value))) {
        (*counter)++;
    }
    if ((*node->key) < maxKey) {
        num_of_values_ranged_in_order(node->right, counter, minKey, maxKey, validationFunc);
    }
}


template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::in_order(Node<KeyType, ValueType>** const array,
    Node<KeyType, ValueType>* const node,
    int* counter) const {
    if (node == NULL) {
        return;
    }
    in_order(array, node->left, counter);
    array[*counter] = node;
    (*counter)++;
    in_order(array, node->right, counter);
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::pre_order(Node<KeyType, ValueType>** array,
    Node<KeyType, ValueType>* const node,
    int* counter) {
    if (node == NULL) {
        return;
    }
    array[*counter] = node;
    (*counter)++;
    pre_order(array, node->left, counter);
    pre_order(array, node->right, counter);
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::delete_tree_nodes(Node<KeyType, ValueType>* node) {
    if (node == NULL) {
        return;
    }
    delete_tree_nodes(node->left);
    delete_tree_nodes(node->right);
    delete node->value;
    delete node->key;
    delete node;
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::copy_aux(Node<KeyType, ValueType>* copyToParent,
    Node<KeyType, ValueType>* copyTo,
    Node<KeyType, ValueType>** const InOrder,
    Node<KeyType, ValueType>** const PreOrder,
    int treeSize) {
    if (treeSize == 0) {
        return;
    }

    copyTo = new Node<KeyType, ValueType>;
    copyTo->key = new KeyType(*(PreOrder[0]->key));
    copyTo->value = new ValueType(*(PreOrder[0]->value));
    copyTo->parent = copyToParent;
    copyTo->height = PreOrder[0]->height;

    int currIndex = -1;
    for (int i = 0; i < treeSize; i++) {
        if (PreOrder[0]->key == InOrder[i]->key) {
            currIndex = i;
            break;
        }
    }
    copy_aux(copyTo, copyTo->left, InOrder, PreOrder + 1, currIndex);
    copy_aux(copyTo, copyTo->right, InOrder + currIndex + 1, PreOrder + currIndex + 1,
        treeSize - currIndex - 1);
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::copy(AvlTree<KeyType, ValueType> const& toCopy,
    AvlTree<KeyType, ValueType>& copyTo) {
    Node<KeyType, ValueType>** treeInOrder = new Node<KeyType, ValueType>*[toCopy.size];
    int in_counter = 0;
    in_order(treeInOrder, toCopy.root->right, &in_counter);

    int pre_counter = 0;
    Node<KeyType, ValueType>** treePreOrder = new Node<KeyType, ValueType>*[toCopy.size];
    pre_order(treePreOrder, toCopy.root->right, &pre_counter);

    copy_aux(copyTo.root, copyTo.root->right, treeInOrder, treePreOrder, toCopy.size);

    delete[] treeInOrder;
    delete[] treePreOrder;
}

template <class KeyType, class ValueType>
TreeStatusType AvlTree<KeyType, ValueType>::place_node(Node<KeyType, ValueType>* toPlace) {
    //if the tree is empty
    if (this->size == 0) {
        this->size++;
        toPlace->parent = NULL;
        this->root->right = toPlace;
        return TreeStatusType::TREE_SUCCESS;
    }
    
    Node<KeyType, ValueType>* currParentNode = this->root;
    Node<KeyType, ValueType>* temp = this->root->right;

    while (temp != NULL) {
        if (*(temp->key) == *(toPlace->key)) {
            return TreeStatusType::TREE_FAILURE;
        }
        else if (*(temp->key) < *(toPlace->key)) {
            currParentNode = temp;
            temp = temp->right;
        }
        else if (*(temp->key) > *(toPlace->key)) {
            currParentNode = temp;
            temp = temp->left;
        }
    }

    // Place node
    toPlace->parent = currParentNode;
    this->size++;
    if (*(currParentNode->key) < *(toPlace->key)) {
        currParentNode->right = toPlace;
    }
    else if (*(currParentNode->key) > *(toPlace->key)) {
        currParentNode->left = toPlace;
    }

    while (currParentNode != NULL) {
        update_height(currParentNode);
        balance_tree(currParentNode);
        currParentNode = currParentNode->parent;
    }
    return TreeStatusType::TREE_SUCCESS;
}

template <class KeyType, class ValueType>
Node<KeyType, ValueType>* AvlTree<KeyType, ValueType>::find_node_by_key(KeyType const& key) const {
    Node<KeyType, ValueType>* temp = this->root->right;

    while (temp != NULL) {
        if (*(temp->key) == key) {
            return temp;
        }
        else if (*(temp->key) < key) {
            temp = temp->right;
        }
        else if (*(temp->key) > key) {
            temp = temp->left;
        }
    }
    return NULL;
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::remove_root() {
    this->root->right = NULL;
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::remove_leaf(Node<KeyType, ValueType>* toRemove) {
    assert(toRemove != NULL);
    bool isRightSon = (toRemove == toRemove->parent->right);
    if (isRightSon) {
        toRemove->parent->right = NULL;
    }
    else {
        toRemove->parent->left = NULL;
    }
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::remove_one_child(Node<KeyType, ValueType>* toRemove) {
    assert(toRemove != NULL);
    bool isRoot = toRemove->parent == NULL;
    bool isRightSon = !isRoot && toRemove == toRemove->parent->right;

    bool hasRightSon = toRemove->right != NULL;

    if (isRoot) {
        if (hasRightSon) {
            this->root->right = toRemove->right;
            toRemove->right->parent = NULL;
        }
        else {
            this->root->right = toRemove->left;
            toRemove->left->parent = NULL;
        }
    }
    else if (isRightSon) {
        if (hasRightSon) {
            toRemove->parent->right = toRemove->right;
            toRemove->right->parent = toRemove->parent;
        }
        else {
            toRemove->parent->right = toRemove->left;
            toRemove->left->parent = toRemove->parent;
        }
    }
    else {
        if (hasRightSon) {
            toRemove->parent->left = toRemove->right;
            toRemove->right->parent = toRemove->parent;
        }
        else {
            toRemove->parent->left = toRemove->left;
            toRemove->left->parent = toRemove->parent;
        }
    }
}

template <class KeyType, class ValueType>
void AvlTree<KeyType, ValueType>::remove_two_children(Node<KeyType, ValueType>* toRemove) {
    assert(toRemove != NULL);
    Node<KeyType, ValueType>* next = get_next_in_order(toRemove->right);
    swap_nodes(toRemove, next);

    if (toRemove->right == NULL && toRemove->left == NULL) {
        remove_leaf(toRemove);
    }
    else {
        remove_one_child(toRemove);
    }
}

template <class KeyType, class ValueType>
Node<KeyType, ValueType>* AvlTree<KeyType, ValueType>::get_next_in_order(Node<KeyType, ValueType>* node) {
    assert(node != NULL);
    if (node->left != NULL) {
        return get_next_in_order(node->left);
    }
    else {
        return node;
    }
}

// AvlTree basic funcs
template <class KeyType, class ValueType>
AvlTree<KeyType, ValueType>::AvlTree() {
    this->root = new Node<KeyType, ValueType>;
    this->root->key = NULL;
    this->root->right = NULL;
    this->root->left = NULL;
    this->root->parent = NULL;
    this->root->height = -1;
    this->size = 0;
}

template <class KeyType, class ValueType>
AvlTree<KeyType, ValueType>::AvlTree(AvlTree<KeyType, ValueType>& tree) {

    this->size = tree.size;
    this->root->left = NULL;
    copy(tree, *this);
}

template <class KeyType, class ValueType>
AvlTree<KeyType, ValueType>& AvlTree<KeyType, ValueType>::operator=(AvlTree<KeyType,
    ValueType> const& tree) {
    if (this == &tree) {
        return *this;
    }

    // Remove the old tree
    Node<KeyType, ValueType>** thisInOrder = new Node<KeyType, ValueType>*[this->size];
    get_tree_in_order(thisInOrder);
    for (int i = 0; i < this->size; i++) {
        delete thisInOrder[i]->key;
        delete thisInOrder[i]->value;
        delete thisInOrder[i];
    }
    delete[] thisInOrder;

    // Copy the new tree
    this->size = tree.size;
    this->root->left = NULL;
    copy(tree, *this);

    return *this;
}

template <class KeyType, class ValueType>
AvlTree<KeyType, ValueType>::~AvlTree() {
    delete_tree_nodes(this->root->right);
    delete this->root;
}


template <class KeyType, class ValueType>
TreeStatusType AvlTree<KeyType, ValueType>::insert(KeyType& key, ValueType& value) {
    
    // Create the node
    Node<KeyType, ValueType>* newNode;
    try {
        newNode = new Node<KeyType, ValueType>;
    }
    catch (std::bad_alloc& ba) {
        return TreeStatusType::TREE_ALLOCATION_ERROR;
    }

    // Fill the parameters
    newNode->height = 0;
    newNode->right = NULL;
    newNode->left = NULL;
    newNode->key = new KeyType(key);
    newNode->value = new ValueType(value);

    // Place the node
    if (place_node(newNode) == TreeStatusType::TREE_FAILURE) {
        // Sanity check
        delete newNode->key;
        delete newNode->value;
        delete newNode;
        newNode = NULL;

        return TreeStatusType::TREE_FAILURE;
    }

    return TreeStatusType::TREE_SUCCESS;
}

template <class KeyType, class ValueType>
TreeStatusType AvlTree<KeyType, ValueType>::find(const KeyType& key,
    ValueType* value) const {
    if (value == NULL) {
        return TreeStatusType::TREE_INVALID_INPUT;
    }
    Node<KeyType, ValueType>* found = find_node_by_key(key);

    if (found == NULL) {
        return TreeStatusType::TREE_FAILURE;
    }
    else {
        *value = *(found->value);
        return TreeStatusType::TREE_SUCCESS;
    }
}

template <class KeyType, class ValueType>
TreeStatusType AvlTree<KeyType, ValueType>::remove(const KeyType& key) {
    Node<KeyType, ValueType>* toDelete = find_node_by_key(key);
    if (toDelete == NULL) {
        return TreeStatusType::TREE_FAILURE;
    }
    return remove_by_pointer(toDelete);
}

template <class KeyType, class ValueType>
KeyType* AvlTree<KeyType, ValueType>::find_max()const {
    Node<KeyType, ValueType>* currNode = this->root->right;

    if (currNode == NULL) {
        return nullptr;
    }

    while (currNode->right != NULL) {
        currNode = currNode->right;
    }
    return currNode->key;
}

template <class KeyType, class ValueType>
KeyType* AvlTree<KeyType, ValueType>::find_min()const {
    Node<KeyType, ValueType>* currNode = this->root->right;

    if (currNode == NULL) {
        return nullptr;
    }

    while (currNode->left != NULL) {
        currNode = currNode->left;
    }
    return currNode->key;
}


template <class KeyType, class ValueType>
TreeStatusType AvlTree<KeyType, ValueType>::remove_by_pointer(Node<KeyType, ValueType>*
    toDelete) {
    if (toDelete == NULL) {
        return TreeStatusType::TREE_INVALID_INPUT;
    }
    // Remove node from the tree
    if (this->size == 1) {
        remove_root();
    }
    else if (toDelete->right == NULL && toDelete->left == NULL) {
        remove_leaf(toDelete);
    }
    else if (toDelete->right == NULL || toDelete->left == NULL) {
        remove_one_child(toDelete);
    }
    else {
        remove_two_children(toDelete);
    }

    // Update heights and check balance factor for the parents
    Node<KeyType, ValueType>* temp = toDelete->parent;
    while (temp != NULL) {
        update_height(temp);
        balance_tree(temp);
        temp = temp->parent;
    }
    // Delete the node's data
    delete toDelete->value;
    delete toDelete->key;
    delete toDelete;
    this->size--;
    return TreeStatusType::TREE_SUCCESS;
}

template <class KeyType, class ValueType>
TreeStatusType AvlTree<KeyType, ValueType>::get_size(int* n) const {
    if (n == NULL) {
        return TreeStatusType::TREE_INVALID_INPUT;
    }

    *n = this->size;
    return TreeStatusType::TREE_SUCCESS;
}

template <class KeyType, class ValueType>
KeyType* AvlTree<KeyType, ValueType>::get_closest_key(Node<KeyType, ValueType>* node, KeyType* key, KeyType* closestKey, int (*compareFunc)(KeyType* key1, KeyType* key2, KeyType* refKey), bool closestKeyValid) const {
    if (node == NULL) {
        return closestKey;
    }

    bool valid = closestKeyValid;    
    if (!valid) {
        KeyType* newClosestKey = node->key;
        valid = true;
        // Traverse according to following rule: left if refrence key is smaller than current node key, right otherwise
        if ((*key) < (*node->key))
            return get_closest_key(node->left, key, newClosestKey, compareFunc, valid);
        else
            return get_closest_key(node->right, key, newClosestKey, compareFunc, valid);
    }
    else if (compareFunc(node->key, closestKey, key) > 0) {  // If current node key is closser to refrence key (key), compare function returns positive value
       closestKey = node->key;  // Update closest key
    }
    // Traverse according to following rule: left if refrence key is smaller than current node key, right otherwise
    if ((*key) < (*node->key))
        return get_closest_key(node->left, key, closestKey, compareFunc, valid);
    else
        return get_closest_key(node->right, key, closestKey, compareFunc, valid);
}

template <class KeyType, class ValueType>
KeyType* AvlTree<KeyType, ValueType>::find_closest_key(KeyType* key, int (*compareFunc)(KeyType* key1, KeyType* key2, KeyType* refKey))const {
    return get_closest_key(root->right, key, NULL, compareFunc, false);
}

template <class KeyType, class ValueType>
Node<KeyType, ValueType>* AvlTree<KeyType, ValueType>::get_root_from_sorted_array(KeyType* sortedKeyArray, ValueType* sortedValueArray, int start, int end, int height, Node<KeyType, ValueType>* parent) {
    if (start > end) {
        return NULL;
    }
    int middle = (start + end) / 2;
    Node<KeyType, ValueType>* tempRoot = new Node<KeyType, ValueType>;
    tempRoot->height = height;
    tempRoot->key = new KeyType(sortedKeyArray[middle]);
    tempRoot->value = new ValueType(sortedValueArray[middle]);
    tempRoot->parent = parent;
    tempRoot->left = get_root_from_sorted_array(sortedKeyArray, sortedValueArray, start, middle - 1, height + 1, tempRoot);
    tempRoot->right = get_root_from_sorted_array(sortedKeyArray, sortedValueArray, middle + 1, end, height + 1, tempRoot);
    return tempRoot;
}

template <class KeyType, class ValueType>
TreeStatusType AvlTree<KeyType, ValueType>::create_tree_from_sorted_array(KeyType* sortedKeyArray, ValueType* sortedValueArray, int length) {
    if (root->right != NULL) {
        return TreeStatusType::TREE_FAILURE;
    }
    root->right = get_root_from_sorted_array(sortedKeyArray, sortedValueArray, 0, length - 1, 0, NULL);
    size = length;
    if (root->right == NULL) {
        return TreeStatusType::TREE_FAILURE;
    }
    return TreeStatusType::TREE_SUCCESS;
}

#endif //DATASTRUCTURESWORLDCUP_AVLTREE_H
