#include "ABE/Policy_resolution.h"

std::vector<std::string>* Policy_resolution::infixToPostfix(std::string infix) {
    std::vector<std::string>* res = new std::vector<std::string>;

    signed long int len = infix.length();

    std::stack<char> st;

    std::string s = "";

    char oc;
    std::string os = "";

    for (signed long int i = 0; i < len; ++i) {
        char ch = infix.at(i);
        if ('&' == ch || '|' == ch || '(' == ch || ')' == ch) {
            if (0 != s.size()) {
                res->push_back(s);
                s.assign("");
            }
        } else if (len - 1 == i) {
            s.append(1, ch);
            res->push_back(s);
            break;
        }

        if ('&' == ch || '|' == ch || '(' == ch || ')' == ch) {
            if (st.empty() || '(' == ch || ('&' == ch && '|' == st.top())) {
                st.push(ch);
            } else if (')' == ch) {
                oc = st.top();
                st.pop();
                while ('(' != oc) {
                    os.assign(1, oc);
                    res->push_back(os);
                    oc = st.top();
                    st.pop();
                }
            } else {
                oc = st.top();
                while ((!('&' == ch && '|' == oc)) && '(' != oc) {
                    os.assign(1, oc);
                    res->push_back(os);
                    st.pop();
                    if (st.empty()) {
                        break;
                    }
                    oc = st.top();
                }
                st.push(ch);
            }
        } else {
            s.append(1, ch);
        }
    }

    while (!st.empty()) {
        oc = st.top();
        st.pop();
        os.assign(1, oc);
        res->push_back(os);
    }

    return res;
}

Binary_tree_policy* Policy_resolution::postfixToBinaryTree(std::vector<std::string> *postfix, element_s *sample_element) {
    std::stack<Binary_tree_policy_node*> st;

    signed long int len = postfix->size();

    for (signed long int i = 0; i < len; ++i) {
        if ('&' == (*postfix)[i].at(0) || '|' == (*postfix)[i].at(0)) {
            Binary_tree_policy_node* node;
            if ('&' == (*postfix)[i].at(0)) {
                node = new Binary_tree_policy_node(Binary_tree_policy_node::AND, "", sample_element);
            } else {
                node = new Binary_tree_policy_node(Binary_tree_policy_node::OR, "", sample_element);
            }
            node->setRightChild(st.top());
            st.top()->setParent(node);
            st.pop();
            node->setLeftChild(st.top());
            st.top()->setParent(node);
            st.pop();
            st.push(node);
        } else {
            Binary_tree_policy_node* node = new Binary_tree_policy_node(Binary_tree_policy_node::LEAF, (*postfix)[i], sample_element);
            st.push(node);
        }
    }

    Binary_tree_policy* tree = new Binary_tree_policy(st.top());
    return tree;
}