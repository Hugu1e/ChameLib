#include "ABE/Policy_generation.h"

void Policy_generation::assignAccessToChildInMatrixForm(Binary_tree_policy_node *node, signed long int *m) {
    if (Binary_tree_policy_node::LEAF == node->getType()) {
        return;
    }

    Binary_tree_policy_node* lc = node->getLeftChild();
    Binary_tree_policy_node* rc = node->getRightChild();

    if (Binary_tree_policy_node::OR == node->getType()) {
        lc->setValue(node->getValue());
        rc->setValue(node->getValue());
        return;
    }

    element_t zero;
    element_t one;
    element_t negOne;
    element_init_same_as(zero, node->getValueElement(0));
    element_init_same_as(one, zero);
    element_init_same_as(negOne, zero);
    element_set0(zero);
    element_set1(one);
    element_set_si(negOne, -1);
    signed long int len = node->getValue()->length();

    if (Binary_tree_policy_node::AND == node->getType()) {
        if (len < *m) {
            for (signed long int i = 0; i < (*m) - len; ++i) {
                node->pushBackValue(zero);
            }
        }
        lc->setValue(node->getValue());
        rc->resizeValue(0);
        for (signed long int i = 0; i < *m; ++i) {
            rc->pushBackValue(zero);
        }
        lc->pushBackValue(one);
        rc->pushBackValue(negOne);
        (*m)++;
        return;
    }
}

void Policy_generation::assignAccessToChildInBinaryTreeForm(Binary_tree_policy_node *node) {
    if (Binary_tree_policy_node::LEAF == node->getType()) {
        return;
    }

    Binary_tree_policy_node* lc = node->getLeftChild();
    Binary_tree_policy_node* rc = node->getRightChild();

    if (Binary_tree_policy_node::OR == node->getType()) {
        lc->setValueElement(0, node->getValueElement(0));
        rc->setValueElement(0, node->getValueElement(0));
        return;
    }

    element_t node_secret;
    element_init_same_as(node_secret, node->getValueElement(0));
    element_set(node_secret, node->getValueElement(0));

    element_t one;
    element_init_same_as(one, node_secret);
    element_set1(one);
    element_t two;
    element_init_same_as(two, node_secret);
    element_set_si(two, 2);

    element_t res1;
    element_init_same_as(res1, node_secret);
    element_set(res1, node_secret);
    element_t res2;
    element_init_same_as(res2, node_secret);
    element_set(res2, node_secret);

    element_t r;
    element_init_same_as(r, node_secret);
    while (element_is0(r)) {
        element_random(r);
    }

    element_printf("the coefficient is %B\n", r);

    element_t mul1;
    element_init_same_as(mul1, node_secret);
    element_mul(mul1, r, one);
    element_add(res1, res1, mul1);

    element_t mul2;
    element_init_same_as(mul2, node_secret);
    element_mul(mul2, r, two);
    element_add(res2, res2, mul2);

    if (Binary_tree_policy_node::AND == node->getType()) {
        lc->setValueElement(0, res1);
        rc->setValueElement(0, res2);
        return;
    }
}

void Policy_generation::generatePolicyInMatrixForm(Binary_tree_policy *tree) {
    signed long int *m = new signed long int;
    *m = 1;

    std::queue<Binary_tree_policy_node*> q;

    q.push(tree->getRoot());

    while (!q.empty()) {
        assignAccessToChildInMatrixForm(q.front(), m);
        if (q.front()->getLeftChild() != NULL) {
            q.push(q.front()->getLeftChild());
        }
        if (q.front()->getRightChild() != NULL) {
            q.push(q.front()->getRightChild());
        }
        q.pop();
    }
}

Element_t_matrix* Policy_generation::getPolicyInMatrixFormFromTree(Binary_tree_policy *tree) {
    Element_t_matrix* res = new Element_t_matrix();

    std::queue<Binary_tree_policy_node*> q;

    q.push(tree->getRoot());

    while (!q.empty()) {
        if (q.front()->getType() == Binary_tree_policy_node::LEAF) {
            Element_t_vector v(*(q.front()->getValue()));
            res->pushBack(&v);
            res->pushBackName(q.front()->getName());
        }
        if (q.front()->getLeftChild() != NULL) {
            q.push(q.front()->getLeftChild());
        }
        if (q.front()->getRightChild() != NULL) {
            q.push(q.front()->getRightChild());
        }
        q.pop();
    }

    return res;
}

std::map<signed long int, std::string>* Policy_generation::getRhoFromTree(Binary_tree_policy *tree) {
    std::map<signed long int, std::string> *res = new std::map<signed long int, std::string>;

    signed long int kk = 0;

    std::queue<Binary_tree_policy_node*> q;

    q.push(tree->getRoot());

    while (!q.empty()) {
        if (q.front()->getType() == Binary_tree_policy_node::LEAF) {
            signed long int k = kk;
            std::string v = "";
            v.assign(q.front()->getName());
            res->insert(std::pair<signed long int, std::string>(k, v));
            ++kk;
        }
        if (q.front()->getLeftChild() != NULL) {
            q.push(q.front()->getLeftChild());
        }
        if (q.front()->getRightChild() != NULL) {
            q.push(q.front()->getRightChild());
        }
        q.pop();
    }

    return res;
}

void Policy_generation::generatePolicyInBinaryTreeForm(Binary_tree_policy *tree, element_s *root_secret) {
    tree->getRoot()->setValueElement(0, root_secret);

    std::queue<Binary_tree_policy_node*> q;

    q.push(tree->getRoot());

    while (!q.empty()) {
        assignAccessToChildInBinaryTreeForm(q.front());
        if (q.front()->getLeftChild() != NULL) {
            q.push(q.front()->getLeftChild());
        }
        if (q.front()->getRightChild() != NULL) {
            q.push(q.front()->getRightChild());
        }
        q.pop();
    }
}

Element_t_vector* Policy_generation::getPolicyInBinaryTreeFormFromTree(Binary_tree_policy *tree) {
    Element_t_vector* res = new Element_t_vector();

    std::queue<Binary_tree_policy_node*> q;

    q.push(tree->getRoot());

    while (!q.empty()) {
        if (q.front()->getType() == Binary_tree_policy_node::LEAF) {
            res->pushBack(q.front()->getValueElement(0));
        }
        if (q.front()->getLeftChild() != NULL) {
            q.push(q.front()->getLeftChild());
        }
        if (q.front()->getRightChild() != NULL) {
            q.push(q.front()->getRightChild());
        }
        q.pop();
    }

    return res;
}