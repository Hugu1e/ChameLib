#ifndef CHAMELIB_POLICY_GENERATION_H
#define CHAMELIB_POLICY_GENERATION_H

#include "../base/Binary_tree_policy.h"
#include "../base/Element_t_matrix.h"
#include <map>
#include <queue>

class Policy_generation{
    private:
        static void assignAccessToChildInMatrixForm(Binary_tree_policy_node *node, signed long int *m);

        static void assignAccessToChildInBinaryTreeForm(Binary_tree_policy_node *node);

        static void generatePolicyInMatrixForm(Binary_tree_policy *tree);

    public:
        
        static Element_t_matrix *getPolicyInMatrixFormFromTree(Binary_tree_policy *tree);

        // static std::map<signed long int, std::string> *getRhoFromTree(Binary_tree_policy *tree);

        // static void generatePolicyInBinaryTreeForm(Binary_tree_policy *tree, element_s *root_secret);

        // static Element_t_vector *getPolicyInBinaryTreeFormFromTree(Binary_tree_policy *tree);
};

#endif  // CHAMELIB_POLICY_GENERATION_H
