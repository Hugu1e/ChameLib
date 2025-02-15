#ifndef CHAMELIB_POLICY_RESOLUTION_H
#define CHAMELIB_POLICY_RESOLUTION_H

#include "../base/Binary_tree_policy.h"
#include <stack>

class Policy_resolution
{
public:
    std::vector<std::string> *infixToPostfix(std::string infix);
    Binary_tree_policy *postfixToBinaryTree(std::vector<std::string> *postfix, element_s *sample_element);

    // multiway_tree *ThresholdExpressionToMultiwayTree(string threshold_expression, element_s *sample_element);
};

#endif // CHAMELIB_POLICY_RESOLUTION_H
