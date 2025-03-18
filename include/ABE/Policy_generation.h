#ifndef CHAMELIB_POLICY_GENERATION_H
#define CHAMELIB_POLICY_GENERATION_H

#include "../base/Binary_tree_policy.h"
#include "../base/Element_t_matrix.h"
#include <map>
#include <queue>

class Policy_generation{
    private:
        void assignAccessToChildInMatrixForm(Binary_tree_policy_node *node, signed long int *m);

        void assignAccessToChildInBinaryTreeForm(Binary_tree_policy_node *node);

    public:
        void generatePolicyInMatrixForm(Binary_tree_policy *tree);
        Element_t_matrix *getPolicyInMatrixFormFromTree(Binary_tree_policy *tree);
        std::map<signed long int, std::string> *getRhoFromTree(Binary_tree_policy *tree);

        void generatePolicyInBinaryTreeForm(Binary_tree_policy *tree, element_s *root_secret);
        Element_t_vector *getPolicyInBinaryTreeFormFromTree(Binary_tree_policy *tree);
};

#endif  // CHAMELIB_POLICY_GENERATION_H
