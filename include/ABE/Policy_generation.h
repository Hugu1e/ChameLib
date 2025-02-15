#ifndef CHAMELIB_POLICY_GENERATION_H
#define CHAMELIB_POLICY_GENERATION_H

#include "../base/Binary_tree_policy.h"
#include "../base/Element_t_matrix.h"
#include <map>
#include <queue>

class Policy_generation
{
private:
    void assignAccessToChildInMatrixForm(Binary_tree_policy_node *node, signed long int *m);

    void assignAccessToChildInBinaryTreeForm(Binary_tree_policy_node *node);

    // void assignAccessToChildInMultiwayTreeForm(multiway_tree_node *node);

public:
    void generatePolicyInMatrixForm(Binary_tree_policy *tree);
    Element_t_matrix *getPolicyInMatrixFormFromTree(Binary_tree_policy *tree);
    std::map<signed long int, std::string> *getRhoFromTree(Binary_tree_policy *tree);

    void generatePolicyInBinaryTreeForm(Binary_tree_policy *tree, element_s *root_secret);
    Element_t_vector *getPolicyInBinaryTreeFormFromTree(Binary_tree_policy *tree);

    // void generatePolicyInMultiwayTreeForm(multiway_tree *tree, element_s *root_secret);
    // std::map<std::string, element_s *> *getSharesFromTree(multiway_tree *tree);

    //    Element_t_matrix* generateLSSSMatrixFromThresholdExpression(std::string threshold_expression, element_s *sample_element);
    // Element_t_matrix *generateLSSSMatrixFromMultiwayTree(multiway_tree *mt, element_s *sample_element);
};

#endif  // CHAMELIB_POLICY_GENERATION_H
