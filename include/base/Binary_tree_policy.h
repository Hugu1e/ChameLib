#ifndef CHAMELIB_BINARY_TREE_POLICY_H
#define CHAMELIB_BINARY_TREE_POLICY_H

#include "Element_t_vector.h"
#include <iostream>
#include <queue>

class Binary_tree_policy_node
{
public:
    enum node_type
    {
        AND,
        OR,
        LEAF
    };

private:
    Binary_tree_policy_node::node_type type;

    std::string name;
    Element_t_vector *value;

    Binary_tree_policy_node *parent;
    Binary_tree_policy_node *left_child;
    Binary_tree_policy_node *right_child;

public:
    Binary_tree_policy_node();
    Binary_tree_policy_node(element_s *sample_element);
    Binary_tree_policy_node(Binary_tree_policy_node::node_type type, std::string name, element_s *sample_element);

    Binary_tree_policy_node::node_type getType();
    void setType(Binary_tree_policy_node::node_type type);

    std::string getName();
    void setName(std::string name);

    Element_t_vector *getValue();
    void setValue(Element_t_vector *value);
    void pushBackValue(element_s *elem);
    element_s *getValueElement(signed long int i);
    void setValueElement(signed long int i, element_s *elem);
    void resizeValue(signed long int i);

    Binary_tree_policy_node *getParent();
    void setParent(Binary_tree_policy_node *parent);
    Binary_tree_policy_node *getLeftChild();
    void setLeftChild(Binary_tree_policy_node *left_child);
    Binary_tree_policy_node *getRightChild();
    void setRightChild(Binary_tree_policy_node *right_child);

    ~Binary_tree_policy_node();
};

class Binary_tree_policy
{
private:
    Binary_tree_policy_node *root;

    void visitNode(Binary_tree_policy_node *node);

    void inOrderTraversalRoughly(Binary_tree_policy_node *node);

    void freeTree(Binary_tree_policy_node *root);

public:
    Binary_tree_policy(element_s *sample_element);
    Binary_tree_policy(Binary_tree_policy_node *root);

    Binary_tree_policy_node *getRoot();

    void levelTraversal();
    void inOrderTraversal();

    ~Binary_tree_policy();
};

#endif // CHAMELIB_BINARY_TREE_POLICY_H
