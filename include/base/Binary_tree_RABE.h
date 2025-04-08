#ifndef CHAMELIB_BINARY_TREE_RABE_H
#define CHAMELIB_BINARY_TREE_RABE_H

#include <pbc/pbc.h>
#include <vector>
#include <stdexcept>
#include <math.h>
#include <queue>
#include <algorithm>

class Binary_tree_RABE_node {
    public:
        enum node_type{ INTERNAL, LEAF };
    private:
        Binary_tree_RABE_node::node_type type;
        element_t gtheta;  // gtheta
        // leaf node
        element_t id;
        time_t time;

        Binary_tree_RABE_node *parent;
        Binary_tree_RABE_node *left_child;
        Binary_tree_RABE_node *right_child;
    public:
        Binary_tree_RABE_node(Binary_tree_RABE_node::node_type type, element_t _G, element_t _Zn);

        Binary_tree_RABE_node::node_type getType();
        void setType(Binary_tree_RABE_node::node_type);
        element_s *getGtheta();
        void setGtheta(element_t gtheta);
        element_s *getId();
        void setId(element_t id);
        time_t getTime();
        void setTime(time_t time);
       

        Binary_tree_RABE_node* getParent();
        void setParent(Binary_tree_RABE_node *parent);
        Binary_tree_RABE_node* getLeftChild();
        void setLeftChild(Binary_tree_RABE_node *left_child);
        Binary_tree_RABE_node* getRightChild();
        void setRightChild(Binary_tree_RABE_node *right_child);

        bool isEmpty();

        ~Binary_tree_RABE_node();
};

class Binary_tree_RABE {
    private:
        int n;  // number of leaf nodes
        Binary_tree_RABE_node *root;

        void freeTree(Binary_tree_RABE_node *root);

    public:
        Binary_tree_RABE();
        Binary_tree_RABE(int n, element_t _G, element_t _Zn);

        void buildTree(int n, element_t _G, element_t _Zn);

        void printTree();

        Binary_tree_RABE_node* setLeafNode(element_t id, time_t time);

        std::vector<Binary_tree_RABE_node *> KUNodes(std::vector<element_s *> rl_ids, time_t t);

        ~Binary_tree_RABE();
};


#endif  // CHAMELIB_BINARY_TREE_RABE_H
