#include <base/Binary_tree_policy.h>

Binary_tree_policy_node::Binary_tree_policy_node() {}
Binary_tree_policy_node::Binary_tree_policy_node(element_s *sample_element) {
    this->name.assign("");

    value = new Element_t_vector(1, sample_element);
    element_set1(value->getElement(0));

    parent = NULL;
    left_child = NULL;
    right_child = NULL;
}
Binary_tree_policy_node::Binary_tree_policy_node(Binary_tree_policy_node::node_type type, std::string name, element_s *sample_element) {
    this->type = type;

    this->name.assign(name);

    value = new Element_t_vector(1, sample_element);
    element_set1(value->getElement(0));

    parent = NULL;
    left_child = NULL;
    right_child = NULL;
}

/**
 * @brief get the type of the node
 * 
 * @return node type
 */
Binary_tree_policy_node::node_type Binary_tree_policy_node::getType() {
    return type;
}

/**
 * @brief set the type of the node
 * 
 * @param type node type
 */
void Binary_tree_policy_node::setType(Binary_tree_policy_node::node_type type) {
    this->type = type;
}

/**
 * @brief get the name of the node
 * 
 * @return node name
 */
std::string Binary_tree_policy_node::getName() {
    return name;
}

/**
 * @brief set the name of the node
 * 
 * @param name node name
 */
void Binary_tree_policy_node::setName(std::string name) {
    this->name.assign(name);
}

/**
 * @brief get the value of the node
 * 
 * @return node value
 */
Element_t_vector* Binary_tree_policy_node::getValue() {
    return value;
}

/**
 * @brief set the value of the node
 * 
 * @param value node value
 */
void Binary_tree_policy_node::setValue(Element_t_vector *value) {
    *(this->value) = *value;
}

/**
 * @brief add 'elem' to the end of the value vector
 * 
 * @param elem 'elem'
 */
void Binary_tree_policy_node::pushBackValue(element_s *elem) {
    value->pushBack(elem);
}

/**
 * @brief get the element with the index 'i'
 * 
 * @param i the index
 * @return the element
 */
element_s* Binary_tree_policy_node::getValueElement(signed long int i) {
    return value->getElement(i);
}
void Binary_tree_policy_node::setValueElement(signed long int i, element_s *elem) {
    value->setElement(i, elem);
}

/**
 * @brief resize value
 * 
 * @param i the new size
 */
void Binary_tree_policy_node::resizeValue(signed long int i) {
    value->resizeValue(i);
}

/**
 * @brief get parent
 * 
 * @return parent
 */
Binary_tree_policy_node* Binary_tree_policy_node::getParent() {
    return parent;
}
/**
 * @brief set parent
 * 
 * @param parent parent
 */
void Binary_tree_policy_node::setParent(Binary_tree_policy_node *parent) {
    this->parent = parent;
}
/**
 * @brief get left child
 * 
 * @return left child
 */
Binary_tree_policy_node* Binary_tree_policy_node::getLeftChild() {
    return left_child;
}
/**
 * @brief set left child
 * 
 * @param left_child left child
 */
void Binary_tree_policy_node::setLeftChild(Binary_tree_policy_node *left_child) {
    this->left_child = left_child;
}
/**
 * @brief get right child
 * 
 * @return right child
 */
Binary_tree_policy_node* Binary_tree_policy_node::getRightChild() {
    return right_child;
}
/**
 * @brief set right child
 * 
 * @param right_child right child
 */
void Binary_tree_policy_node::setRightChild(Binary_tree_policy_node *right_child) {
    this->right_child = right_child;
}


Binary_tree_policy::Binary_tree_policy(element_s *sample_element) {
    root = new Binary_tree_policy_node(sample_element);
}
Binary_tree_policy::Binary_tree_policy(Binary_tree_policy_node *root) {
    this->root = root;
}

/**
 * @brief get root
 * 
 * @return root
 */
Binary_tree_policy_node* Binary_tree_policy::getRoot() {
    return root;
}

/**
 * @brief print node
 * 
 * @param node the node
 */
void Binary_tree_policy::visitNode(Binary_tree_policy_node *node) {
    if (node->getType() == Binary_tree_policy_node::LEAF) {
        std::cout << node->getName();
    } else if (node->getType() == Binary_tree_policy_node::AND) {
        std::cout << "AND";
    } else if (node->getType() == Binary_tree_policy_node::OR) {
        std::cout << "OR";
    }
}

/**
 * @brief in order recursive traversal
 * 
 * @param node the current node
 */
void Binary_tree_policy::inOrderTraversalRoughly(Binary_tree_policy_node *node) {
    if (node == NULL) {
        return;
    }
    inOrderTraversalRoughly(node->getLeftChild());
    visitNode(node);
    std::cout << "  ";
    inOrderTraversalRoughly(node->getRightChild());
}

/**
 * @brief level traversal, print by layer newline
 */
void Binary_tree_policy::levelTraversal() {
    std::queue<Binary_tree_policy_node*> q;

    Binary_tree_policy_node *last = root;
    Binary_tree_policy_node *nlast = root;

    q.push(root);

    while (!q.empty()) {
        visitNode(q.front());
        if (q.front()->getLeftChild() != NULL) {
            q.push(q.front()->getLeftChild());
            nlast = q.front()->getLeftChild();
        }
        if (q.front()->getRightChild() != NULL) {
            q.push(q.front()->getRightChild());
            nlast = q.front()->getRightChild();
        }
        if (q.front() == last) {
            std::cout << std::endl;
            last = nlast;
        } else {
            std::cout << "  ";
        }
        q.pop();
    }
}
/**
 * @brief in order traversal
 */
void Binary_tree_policy::inOrderTraversal() {
    inOrderTraversalRoughly(root);
    std::cout << std::endl;
}