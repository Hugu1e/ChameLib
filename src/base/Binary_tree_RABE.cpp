#include <base/Binary_tree_RABE.h>

Binary_tree_RABE_node::Binary_tree_RABE_node(Binary_tree_RABE_node::node_type type, element_t _G, element_t _Zn){
    this->type = type;
    element_init_same_as(gtheta, _G);
    element_init_same_as(id, _Zn);
    parent = NULL;
    left_child = NULL;
    right_child = NULL;
}

Binary_tree_RABE_node* Binary_tree_RABE_node::getLeftChild(){
    return left_child;
}

void Binary_tree_RABE_node::setLeftChild(Binary_tree_RABE_node *left_child){
    this->left_child = left_child;
}

Binary_tree_RABE_node* Binary_tree_RABE_node::getRightChild(){
    return right_child;
}

void Binary_tree_RABE_node::setRightChild(Binary_tree_RABE_node *right_child){
    this->right_child = right_child;
}

bool Binary_tree_RABE_node::isEmpty(){
    if(type == Binary_tree_RABE_node::LEAF){
        if(element_is0(id)){
            return true;
        }else{
            return false;
        }
    }else if(type == Binary_tree_RABE_node::INTERNAL){
        if(element_is0(gtheta)){
            return true;
        }else{
            return false;
        }
    }
}

void Binary_tree_RABE_node::setParent(Binary_tree_RABE_node *parent)
{
    this->parent = parent;
}

Binary_tree_RABE_node::node_type Binary_tree_RABE_node::getType(){
    return type;
}

void Binary_tree_RABE_node::setType(Binary_tree_RABE_node::node_type type){
    this->type = type;
}

Binary_tree_RABE_node* Binary_tree_RABE_node::getParent(){
    return parent;
}

element_s *Binary_tree_RABE_node::getGtheta(){
    return gtheta;
}

void Binary_tree_RABE_node::setGtheta(element_t gtheta)
{
    element_set(this->gtheta, gtheta);
}
element_s *Binary_tree_RABE_node::getId()
{
    return id;
}

void Binary_tree_RABE_node::setId(element_t id)
{
    element_set(this->id, id);
}

time_t Binary_tree_RABE_node::getTime(){
    return time;
}

void Binary_tree_RABE_node::setTime(time_t time)
{
    this->time = time;
}

void ConstructTree(int curDepth, int totalDepth, Binary_tree_RABE_node *node, Binary_tree_RABE_node *parent_node, element_t _G, element_t _Zn){
    if(parent_node != nullptr){
        node->setParent(parent_node);
    }

    if(curDepth == totalDepth){
        // leaf node
        node->setType(Binary_tree_RABE_node::LEAF);
        return;
    }
    // internal node
    Binary_tree_RABE_node *left = new Binary_tree_RABE_node(Binary_tree_RABE_node::INTERNAL, _G, _Zn);
    node->setLeftChild(left);
    ConstructTree(curDepth+1, totalDepth, left, node, _G, _Zn);
    Binary_tree_RABE_node *right = new Binary_tree_RABE_node(Binary_tree_RABE_node::INTERNAL, _G, _Zn);
    node->setRightChild(right);
    ConstructTree(curDepth+1, totalDepth, right, node, _G, _Zn);
}

Binary_tree_RABE::Binary_tree_RABE(){}

Binary_tree_RABE::Binary_tree_RABE(int n, element_t _G, element_t _Zn){
    buildTree(n, _G, _Zn);
}

void Binary_tree_RABE::buildTree(int n, element_t _G, element_t _Zn){
    // 构造一个有n个叶子节点的二叉树
    if((n & (n-1)) != 0){
        throw std::invalid_argument("n is not a power of 2");
        return;
    }
    if(n < 2){
        throw std::invalid_argument("n is less than 2");
        return;
    }
    int totalDepth = log2(n) + 1;
    this->root = new Binary_tree_RABE_node(Binary_tree_RABE_node::INTERNAL, _G, _Zn);
    ConstructTree(1, totalDepth, this->root, nullptr, _G, _Zn);
}

void PrintTree(Binary_tree_RABE_node* node, int depth = 0) {
    if (node == nullptr) {
        return;
    }

    // Print node information
    if (node->getType() == Binary_tree_RABE_node::INTERNAL) {
        printf("I");
        // PrintElement("",*node->getGtheta());
    }else if(node->getType() == Binary_tree_RABE_node::LEAF){
        printf("L");
    }
    PrintTree(node->getLeftChild(), depth + 1);
    PrintTree(node->getRightChild(), depth + 1);
}

void Binary_tree_RABE::printTree(){
    printf("print tree\n");
    PrintTree(this->root);
    printf("\nend of tree\n");
}

Binary_tree_RABE_node* Binary_tree_RABE::setLeafNode(element_t id, time_t time){
    // find the first leaf node that is not set
    Binary_tree_RABE_node *res = nullptr;
    std::queue<Binary_tree_RABE_node *> q;
    q.push(this->root);
    while(!q.empty()){
        Binary_tree_RABE_node *node = q.front();
        if(node->getType() == Binary_tree_RABE_node::LEAF && node->isEmpty()){
            res = node;
            break;
        }
        q.pop();
        if(node->getType() == Binary_tree_RABE_node::INTERNAL){
            q.push(node->getLeftChild());
            q.push(node->getRightChild());
        }
    }
    if(res != nullptr){
        res->setId(id);
        res->setTime(time);
    }
    return res;
}
std::vector<Binary_tree_RABE_node *> Binary_tree_RABE::KUNodes(std::vector<element_s *> rl_ids, time_t t)
{
    std::vector<Binary_tree_RABE_node *> res;
    std::vector<Binary_tree_RABE_node *> rNodes;  // revoked nodes
    std::vector<Binary_tree_RABE_node *> rNodesWithParents;  // revoked nodes with parents
    std::queue<Binary_tree_RABE_node *> q;
    q.push(this->root);
    while(!q.empty()){
        Binary_tree_RABE_node *node = q.front();
        if(node->getType() == Binary_tree_RABE_node::LEAF && !node->isEmpty()){
            // check if the node is in rl_ids and the time is less than t
            for(int i = 0;i < rl_ids.size();i++){
                if(element_cmp(node->getId(), rl_ids[i]) == 0 && node->getTime() < t){
                    rNodes.push_back(node);
                    break;
                }
            }
        }
        q.pop();
        if(node->getType() == Binary_tree_RABE_node::INTERNAL){
            q.push(node->getLeftChild());
            q.push(node->getRightChild());
        }
    }

    // find the parents of revoked nodes
    for(int i = 0;i < rNodes.size();i++){
        rNodesWithParents.push_back(rNodes[i]);
        Binary_tree_RABE_node *node = rNodes[i]->getParent();
        while(node != nullptr){
            rNodesWithParents.push_back(node);
            node = node->getParent();
        }
    }

    std::queue<Binary_tree_RABE_node *> p;
    p.push(this->root);
    while(!p.empty()){
        Binary_tree_RABE_node *node = p.front();
        p.pop();
        // if node not in rNodesWithParents, add it to res
        if(rNodesWithParents.end() == find(rNodesWithParents.begin(), rNodesWithParents.end(), node)){
            res.push_back(node);
            continue;
        }
        if(node->getType() == Binary_tree_RABE_node::INTERNAL){
            p.push(node->getLeftChild());
            p.push(node->getRightChild());
        }
    }
    return res;
}