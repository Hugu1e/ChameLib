#include <ABE/RABE_XNM.h>

RABE_XNM::RABE_XNM(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn): PbcScheme(_G1, _G2, _GT, _Zn){
    element_init_same_as(this->d1, Zn);
    element_init_same_as(this->d2, Zn);
    element_init_same_as(this->d3, Zn);

    element_init_same_as(this->r1, Zn);
    element_init_same_as(this->r2, Zn);

    element_init_same_as(this->b1r1a1, Zn);
    element_init_same_as(this->b1r1a2, Zn);
    element_init_same_as(this->b2r2a1, Zn);
    element_init_same_as(this->b2r2a2, Zn);
    element_init_same_as(this->r1r2a1, Zn);
    element_init_same_as(this->r1r2a2, Zn);

    element_init_same_as(this->s1, Zn);
    element_init_same_as(this->s2, Zn);
}


/**
 * hash function {0,1}* -> G
 * input: m
 * output: res
 */
void RABE_XNM::Hash(element_t res, std::string m){
    HASH::hash(res, m);
}

/**
 * input: n
 * output: mpk, msk, st, rl
 */
void RABE_XNM::Setup(RABE_XNM_mpk *mpk, RABE_XNM_msk *msk, std::vector<RABE_XNM_revokedPreson *> *rl, Binary_tree_RABE *&st, int n){
    element_random(tmp_G);
    msk->insertElement("g", "G1", tmp_G);
    element_random(tmp_H);
    msk->insertElement("h", "G2", tmp_H);
    element_random(tmp_Zn);
    msk->insertElement("a1", "Zn", tmp_Zn);
    element_random(tmp_Zn);
    msk->insertElement("a2", "Zn", tmp_Zn);
    element_random(tmp_Zn);
    msk->insertElement("b1", "Zn", tmp_Zn);
    element_random(tmp_Zn);
    msk->insertElement("b2", "Zn", tmp_Zn);

    element_random(this->d1);
    element_random(this->d2);
    element_random(this->d3);

    // g^d1, g^d2, g^d3
    element_pow_zn(tmp_G, msk->getElement("g"), this->d1);
    msk->insertElement("g_pow_d1", "G1", tmp_G);
    element_pow_zn(tmp_G, msk->getElement("g"), this->d2);
    msk->insertElement("g_pow_d2", "G1", tmp_G);
    element_pow_zn(tmp_G, msk->getElement("g"), this->d3);
    msk->insertElement("g_pow_d3", "G1", tmp_G);

    mpk->insertElement("h", "G2", msk->getElement("h"));
    element_pow_zn(tmp_H, msk->getElement("h"), msk->getElement("a1"));
    mpk->insertElement("H1", "G2", tmp_H);
    element_pow_zn(tmp_H, msk->getElement("h"), msk->getElement("a2"));
    mpk->insertElement("H2", "G2", tmp_H);

    // e(g,h)^(d1a1+d3)
    element_mul(this->tmp_Zn, this->d1, msk->getElement("a1"));
    element_add(this->tmp_Zn, this->tmp_Zn, this->d3);
    element_pairing(this->tmp_GT, msk->getElement("g"), msk->getElement("h"));
    element_pow_zn(tmp_GT, this->tmp_GT, this->tmp_Zn);
    mpk->insertElement("T1", "GT", tmp_GT);
    // e(g,h)^(d2a2+d3)
    element_mul(this->tmp_Zn, this->d2, msk->getElement("a2"));
    element_add(this->tmp_Zn, this->tmp_Zn, this->d3);
    element_pairing(this->tmp_GT, msk->getElement("g"), msk->getElement("h"));
    element_pow_zn(tmp_GT, this->tmp_GT, this->tmp_Zn);
    mpk->insertElement("T2", "GT", tmp_GT);

    // initialize rl
    rl->clear();

    // initialize st
    st = new Binary_tree_RABE(n, this->G1, this->Zn);
}

/**
 * input: mpk, msk, st, id, attr_list
 * output: skid, st
 */
void RABE_XNM::KGen(RABE_XNM_skid *skid, Binary_tree_RABE *st, RABE_XNM_mpk *mpk, RABE_XNM_msk *msk, element_t id, std::vector<std::string> *attr_list){
    element_random(this->r1);
    element_random(this->r2);
    // sk0 = (h^(b1r1), h^(b2r2), h^(r1+r2))
    element_mul(this->tmp_Zn, msk->getElement("b1"), this->r1);
    element_pow_zn(tmp_H, mpk->getElement("h"), this->tmp_Zn);
    skid->get_sk0()->insertElement("sk0_1", "G2", tmp_H);
    // (b1 * r1) / a1
    element_div(this->b1r1a1, this->tmp_Zn, msk->getElement("a1"));
    // (b1 * r1) / a2
    element_div(this->b1r1a2, this->tmp_Zn, msk->getElement("a2"));
    element_mul(this->tmp_Zn, msk->getElement("b2"), this->r2);
    element_pow_zn(tmp_H, mpk->getElement("h"), this->tmp_Zn);
    skid->get_sk0()->insertElement("sk0_2", "G2", tmp_H);
    // (b2 * r2) / a1
    element_div(this->b2r2a1, this->tmp_Zn, msk->getElement("a1"));
    // (b2 * r2) / a2
    element_div(this->b2r2a2, this->tmp_Zn, msk->getElement("a2"));
    element_add(this->tmp_Zn, this->r1, this->r2);
    element_pow_zn(tmp_H, mpk->getElement("h"), this->tmp_Zn);
    skid->get_sk0()->insertElement("sk0_3", "G2", tmp_H);
    // (r1 + r2) / a1
    element_div(this->r1r2a1, this->tmp_Zn, msk->getElement("a1"));
    // (r1 + r2) / a2
    element_div(this->r1r2a2, this->tmp_Zn, msk->getElement("a2"));

    // compute sk_y
    for(int i = 0;i < attr_list->size();i++){
        attr_map[attr_list->at(i)] = i;
        // sigma_y
        element_random(this->tmp_Zn);
        // t = 1
        // H(y11)^b1r1a1
        std::string y11 = attr_list->at(i) + "1" + "1";
        this->Hash(this->tmp_G, y11);
        element_pow_zn(this->tmp_G, this->tmp_G, this->b1r1a1);
        // H(y21)^b2r2a1
        std::string y21 = attr_list->at(i) + "2" + "1";
        this->Hash(this->tmp_G_2, y21);
        element_pow_zn(this->tmp_G_2, this->tmp_G_2, this->b2r2a1);
        // H(y31)^r1r2a1
        std::string y31 = attr_list->at(i) + "3" + "1";
        this->Hash(this->tmp_G_3, y31);
        element_pow_zn(this->tmp_G_3, this->tmp_G_3, this->r1r2a1);
        // g^(sigma_y / a1)
        element_div(this->tmp_Zn_2, this->tmp_Zn, msk->getElement("a1"));
        element_pow_zn(this->tmp_G_4, msk->getElement("g"), this->tmp_Zn_2);
        // sky1
        element_mul(tmp_G, this->tmp_G, this->tmp_G_2);
        element_mul(tmp_G, tmp_G, this->tmp_G_3);
        element_mul(tmp_G, tmp_G, this->tmp_G_4);

        PbcElements sk_y;
        sk_y.insertElement("sk_1", "G1", tmp_G);

        // t = 2
        // H(y12)^b1r1a2
        std::string y12 = attr_list->at(i) + "1" + "2";
        this->Hash(this->tmp_G, y12);
        element_pow_zn(this->tmp_G, this->tmp_G, this->b1r1a2);
        // H(y22)^b2r2a2
        std::string y22 = attr_list->at(i) + "2" + "2";
        this->Hash(this->tmp_G_2, y22);
        element_pow_zn(this->tmp_G_2, this->tmp_G_2, this->b2r2a2);
        // H(y32)^r1r2a2
        std::string y32 = attr_list->at(i) + "3" + "2";
        this->Hash(this->tmp_G_3, y32);
        element_pow_zn(this->tmp_G_3, this->tmp_G_3, this->r1r2a2);
        // g^(sigma_y / a2)
        element_div(this->tmp_Zn_2, this->tmp_Zn, msk->getElement("a2"));
        element_pow_zn(this->tmp_G_4, msk->getElement("g"), this->tmp_Zn_2);
        // sky2
        element_mul(tmp_G, this->tmp_G, this->tmp_G_2);
        element_mul(tmp_G, tmp_G, this->tmp_G_3);
        element_mul(tmp_G, tmp_G, this->tmp_G_4);
        sk_y.insertElement("sk_2", "G1", tmp_G);

        // sky3 = g^(-sigma_y)
        element_neg(this->tmp_Zn, this->tmp_Zn);
        element_pow_zn(tmp_G, msk->getElement("g"), this->tmp_Zn);
        sk_y.insertElement("sk_3", "G1", tmp_G);

        skid->get_sk_y()->push_back(sk_y);
    }

    // sk_prime
    // sigma_prime
    element_random(this->tmp_Zn);
    // t = 1
    // H(0111)^b1r1a1
    std::string y0111 = "0111";
    this->Hash(this->tmp_G, y0111);
    element_pow_zn(this->tmp_G, this->tmp_G, this->b1r1a1);
    // H(0121)^b2r2a1
    std::string y0121 = "0121";
    this->Hash(this->tmp_G_2, y0121);
    element_pow_zn(this->tmp_G_2, this->tmp_G_2, this->b2r2a1);
    // H(0131)^r1r2a1
    std::string y0131 = "0131";
    this->Hash(this->tmp_G_3, y0131);
    element_pow_zn(this->tmp_G_3, this->tmp_G_3, this->r1r2a1);
    // g^(sigma_prime / a1)
    element_div(this->tmp_Zn_2, this->tmp_Zn, msk->getElement("a1"));
    element_pow_zn(this->tmp_G_4, msk->getElement("g"), this->tmp_Zn_2);
    // sk_prime1
    element_mul(tmp_G, msk->getElement("g_pow_d1"), this->tmp_G);
    element_mul(tmp_G, tmp_G, this->tmp_G_2);
    element_mul(tmp_G, tmp_G, this->tmp_G_3);
    element_mul(tmp_G, tmp_G, this->tmp_G_4);

    skid->get_sk_prime()->insertElement("sk_1", "G1", tmp_G);

    // t = 2
    // H(0112)^b1r1a2
    std::string y0112 = "0112";
    this->Hash(this->tmp_G, y0112);
    element_pow_zn(this->tmp_G, this->tmp_G, this->b1r1a2);
    // H(0122)^b2r2a2
    std::string y0122 = "0122";
    this->Hash(this->tmp_G_2, y0122);
    element_pow_zn(this->tmp_G_2, this->tmp_G_2, this->b2r2a2);
    // H(0132)^r1r2a2
    std::string y0132 = "0132";
    this->Hash(this->tmp_G_3, y0132);
    element_pow_zn(this->tmp_G_3, this->tmp_G_3, this->r1r2a2);
    // g^(sigma_prime / a2)
    element_div(this->tmp_Zn_2, this->tmp_Zn, msk->getElement("a2"));
    element_pow_zn(this->tmp_G_4, msk->getElement("g"), this->tmp_Zn_2);
    // sk_prime2
    element_mul(tmp_G, msk->getElement("g_pow_d2"), this->tmp_G);
    element_mul(tmp_G, tmp_G, this->tmp_G_2);
    element_mul(tmp_G, tmp_G, this->tmp_G_3);
    element_mul(tmp_G, tmp_G, this->tmp_G_4);
    skid->get_sk_prime()->insertElement("sk_2", "G1", tmp_G);

    // sk_prime3 = g^d3 * g ^ (-sigma_prime)
    element_neg(this->tmp_Zn, this->tmp_Zn);
    element_pow_zn(tmp_G, msk->getElement("g"), this->tmp_Zn);
    element_mul(tmp_G, tmp_G, msk->getElement("g_pow_d3"));
    skid->get_sk_prime()->insertElement("sk_3", "G1", tmp_G);

    // pick an unassigned node in st
    // id
    element_random(id);
    // TODO
    // time_t = 2025.12.31 0:00:00
    time_t target_time = TimeUtils::TimeCast(2025, 12, 31, 0, 0, 0);
    
    // find and set an unassigned node
    Binary_tree_RABE_node *node = st->setLeafNode(id, target_time);

    // set sk_theta
    while(node != NULL){
        element_set0(this->tmp_G);
        if(element_is0(node->getGtheta())){
            // store gtheta in theta
            element_random(this->tmp_G);
            node->setGtheta(this->tmp_G);
        }else{
            element_set(this->tmp_G, node->getGtheta());
        }
        // sk_theta = g^d3 * g^(-sigma_prime) / gtheta
        element_div(this->tmp_G_2, skid->get_sk_prime()->getElement("sk_3"), this->tmp_G);

        PbcElements tmp_sk_theta;
        tmp_sk_theta.insertElement("sk_theta", "G1", this->tmp_G_2);
        skid->get_sk_theta()->emplace_back(std::make_pair(node, tmp_sk_theta));

        node = node->getParent();
    }
}

std::vector<Binary_tree_RABE_node *> RABE_XNM::KUNodes(Binary_tree_RABE *st, std::vector<RABE_XNM_revokedPreson *> *rl, time_t t)
{
    // get rl_ids
    std::vector<element_s *> rl_ids; 
    for(int i = 0;i < rl->size();i++){
        rl_ids.push_back(rl->at(i)->getElement("id"));
    }
    return st->KUNodes(rl_ids, t);
}

/**
 * input: mpk, st, rl, t
 * output: kut
 */
void RABE_XNM::KUpt(RABE_XNM_kut *kut, RABE_XNM_mpk *mpk, Binary_tree_RABE *st, std::vector<RABE_XNM_revokedPreson *> *rl, time_t t){
    std::vector<Binary_tree_RABE_node *> thetas = this->KUNodes(st, rl, t);

    kut->setTime(t);
    for(int i = 0;i < thetas.size();i++){
        // rtheta
        element_random(this->tmp_Zn);
        // gtheta * (H(1t)^rtheta)
        std::string _1t = "1" + std::to_string(t);
        this->Hash(this->tmp_G, _1t);
        element_pow_zn(this->tmp_G, this->tmp_G, this->tmp_Zn);
        element_mul(this->tmp_G, thetas[i]->getGtheta(), this->tmp_G);
        // h^rtheta
        element_pow_zn(this->tmp_H, mpk->getElement("h"), this->tmp_Zn);

        // kuTheta *ku_theta = new kuTheta();
        // ku_theta->Init(this->G, this->H);
        // ku_theta->theta = thetas[i];
        // element_set(ku_theta->ku_theta_1, this->tmp_G);
        // element_set(ku_theta->ku_theta_2, this->tmp_H);

        // kut->ku_theta.push_back(ku_theta);
        PbcElements tmp_ku_theta;
        tmp_ku_theta.insertElement("ku_theta_1", "G1", this->tmp_G);
        tmp_ku_theta.insertElement("ku_theta_2", "G2", this->tmp_H);
        kut->get_ku_theta()->emplace_back(std::make_pair(thetas[i], tmp_ku_theta));
    }
}

/**
 * input: mpk, skid, kut
 * output: dkidt
 */
void RABE_XNM::DKGen(RABE_XNM_dkidt *dkidt, RABE_XNM_mpk *mpk, RABE_XNM_skid *skid, RABE_XNM_kut *kut){
    // TODO judge Path(id) ∩ KUNodes(st, rl, t) != NULL


    // rtheta + rtheta'
    element_random(this->tmp_Zn);

    dkidt->setTime(kut->getTime());

    // sk'' = (sk1', sk2', sk3')
    dkidt->get_sk_prime_prime()->insertElement("sk_1", "G1", skid->get_sk_prime()->getElement("sk_1"));
    dkidt->get_sk_prime_prime()->insertElement("sk_2", "G1", skid->get_sk_prime()->getElement("sk_2"));
    // sk3' = g^d3 * g^(-sigma_prime) * H(1t)^(rtheta + rtheta')
    std::string _1t = "1" + std::to_string(dkidt->getTime());
    this->Hash(this->tmp_G, _1t);
    element_pow_zn(this->tmp_G, this->tmp_G, this->tmp_Zn);
    element_mul(tmp_G, skid->get_sk_prime()->getElement("sk_3"), this->tmp_G);
    dkidt->get_sk_prime_prime()->insertElement("sk_3", "G1", tmp_G);

    // sk0' = (sk01, sk02, sk03, sk04)
    dkidt->get_sk0_prime()->insertElement("sk_1", "G2", skid->get_sk0()->getElement("sk0_1"));
    dkidt->get_sk0_prime()->insertElement("sk_2", "G2", skid->get_sk0()->getElement("sk0_2"));
    dkidt->get_sk0_prime()->insertElement("sk_3", "G2", skid->get_sk0()->getElement("sk0_3"));
    // sk04 = h^(rtheta + rtheta')
    element_pow_zn(tmp_H, mpk->getElement("h"), this->tmp_Zn);
    dkidt->get_sk0_prime()->insertElement("sk_4", "G2", tmp_H);

    // sky
    for(int i = 0;i < skid->get_sk_y()->size();i++){
        PbcElements tmp_sk_y;
        tmp_sk_y.insertElement("sk_1", "G1", skid->get_sk_y(i)->getElement("sk_1"));
        tmp_sk_y.insertElement("sk_2", "G1", skid->get_sk_y(i)->getElement("sk_2"));
        tmp_sk_y.insertElement("sk_3", "G1", skid->get_sk_y(i)->getElement("sk_3"));
        dkidt->get_sk_y()->push_back(tmp_sk_y);
    }
}

/**
 * input: mpk, msg, policy_str, t, s1, s2
 * output: ciphertext
 */
void RABE_XNM::Enc(RABE_XNM_ciphertext *ciphertext, RABE_XNM_mpk *mpk, element_t msg, std::string policy_str,time_t t, element_t s1, element_t s2)
{
    this->policy_str = policy_str;

    Policy_resolution pr;
    Policy_generation pg;
    element_random(this->tmp_Zn);

    std::vector<std::string>* postfix_expression = pr.infixToPostfix(policy_str);
    for(int i = 0;i < postfix_expression->size();i++){
        printf("%s ", postfix_expression->at(i).c_str());
    }
    printf("\n");
    Binary_tree_policy* binary_tree_expression = pr.postfixToBinaryTree(postfix_expression, this->tmp_Zn);
    pg.generatePolicyInMatrixForm(binary_tree_expression);
    Element_t_matrix* M = pg.getPolicyInMatrixFormFromTree(binary_tree_expression);

    unsigned long int rows = M->row();
    unsigned long int cols = M->col();

    printf("rows: %ld, cols: %ld\n", rows, cols);
    for(int i = 0;i < rows;i++){
        for(int j = 0;j < cols;j++){
            element_printf("%B ", M->getElement(i, j));
        }
        printf("\n");
    }

    // s1,s2
    element_set(this->s1, s1);
    element_set(this->s2, s2);

    // t
    ciphertext->setTime(t);

    // ct0
    // ct0_1 = H1^s1
    element_pow_zn(tmp_H, mpk->getElement("H1"), this->s1);
    ciphertext->get_ct0()->insertElement("ct0_1", "G2", tmp_H);
    // ct0_2 = H2^s2
    element_pow_zn(tmp_H, mpk->getElement("H2"), this->s2);
    ciphertext->get_ct0()->insertElement("ct0_2", "G2", tmp_H);
    // ct0_3 = h^(s1+s2)
    element_add(this->tmp_Zn_2, this->s1, this->s2);
    element_pow_zn(tmp_H, mpk->getElement("h"), this->tmp_Zn_2);
    ciphertext->get_ct0()->insertElement("ct0_3", "G2", tmp_H);
    // ct0_4 = H(1t)^(s1+s2)
    std::string _1t = "1" + std::to_string(ciphertext->getTime());
    this->Hash(this->tmp_G, _1t);
    element_pow_zn(tmp_G, this->tmp_G, this->tmp_Zn_2);
    ciphertext->get_ct0()->insertElement("ct0_4", "G1", tmp_G);

    // ct_prime = T1^s1 * T2^s2 * msg
    element_pow_zn(this->tmp_GT, mpk->getElement("T1"), this->s1);
    element_pow_zn(this->tmp_GT_2, mpk->getElement("T2"), this->s2);
    element_mul(this->tmp_GT_3, this->tmp_GT, this->tmp_GT_2);
    element_mul(tmp_GT_3, this->tmp_GT_3, msg);
    ciphertext->get_ct_prime()->insertElement("ct_prime", "GT", tmp_GT_3);

    // ct_y
    // for i = 1,2,...,rows
    for(unsigned long int i=0; i<rows;i++){
        std::string attr = M->getName(i);
        pai[i] = attr;
        // printf("attr: %s\n", attr.c_str());

        // l = 1
        std::string attr_l_1 = attr + "1" + "1";
        // H(attr_l_1)^s1
        this->Hash(this->tmp_G, attr_l_1);
        element_pow_zn(this->tmp_G, this->tmp_G, this->s1);
        std::string attr_l_2 = attr + "1" + "2";
        // H(attr_l_2)^s2
        this->Hash(this->tmp_G_2, attr_l_2);
        element_pow_zn(this->tmp_G_2, this->tmp_G_2, this->s2);
        element_mul(tmp_G_4, this->tmp_G, this->tmp_G_2);
        // for j = 1,2,...,cols
        std::string str_0jl1,str_0jl2;
        for(unsigned long int j=0; j<cols;j++){
            str_0jl1 = "0" + std::to_string(j+1) + "1" + "1";
            str_0jl2 = "0" + std::to_string(j+1) + "1" + "2";
            // H(0jl1)^s1
            this->Hash(this->tmp_G, str_0jl1);
            element_pow_zn(this->tmp_G, this->tmp_G, this->s1);
            // H(0jl2)^s2
            this->Hash(this->tmp_G_2, str_0jl2);
            element_pow_zn(this->tmp_G_2, this->tmp_G_2, this->s2);
            // H(0jl1)^s1 * H(0jl2)^s2
            element_mul(this->tmp_G_3, this->tmp_G, this->tmp_G_2);
            // (H(0jl1)^s1 * H(0jl2)^s2)^M[i][j]
            element_pow_zn(this->tmp_G_3, this->tmp_G_3, M->getElement(i, j));
            element_mul(tmp_G_4, tmp_G_4, this->tmp_G_3);
        }

        PbcElements tmp_ct_y;
        tmp_ct_y.insertElement("ct_1", "G1", tmp_G_4);
    
        // l = 2
        attr_l_1 = attr + "2" + "1";
        // H(attr_l_1)^s1
        this->Hash(this->tmp_G, attr_l_1);
        element_pow_zn(this->tmp_G, this->tmp_G, this->s1);
        attr_l_2 = attr + "2" + "2";
        // H(attr_l_2)^s2
        this->Hash(this->tmp_G_2, attr_l_2);
        element_pow_zn(this->tmp_G_2, this->tmp_G_2, this->s2);
        element_mul(tmp_G_4, this->tmp_G, this->tmp_G_2);
        // for j = 1,2,...,cols
        for(unsigned long int j=0; j<cols;j++){
            str_0jl1 = "0" + std::to_string(j+1) + "2" + "1";
            str_0jl2 = "0" + std::to_string(j+1) + "2" + "2";
            // H(0jl1)^s1
            this->Hash(this->tmp_G, str_0jl1);
            element_pow_zn(this->tmp_G, this->tmp_G, this->s1);
            // H(0jl2)^s2
            this->Hash(this->tmp_G_2, str_0jl2);
            element_pow_zn(this->tmp_G_2, this->tmp_G_2, this->s2);
            // H(0jl1)^s1 * H(0jl2)^s2
            element_mul(this->tmp_G_3, this->tmp_G, this->tmp_G_2);
            // (H(0jl1)^s1 * H(0jl2)^s2)^M[i][j]
            element_pow_zn(this->tmp_G_3, this->tmp_G_3, M->getElement(i, j));
            element_mul(tmp_G_4, tmp_G_4, this->tmp_G_3);
        }
        tmp_ct_y.insertElement("ct_2", "G1", tmp_G_4);

        // l = 3
        attr_l_1 = attr + "3" + "1";
        // H(attr_l_1)^s1
        this->Hash(this->tmp_G, attr_l_1);
        element_pow_zn(this->tmp_G, this->tmp_G, this->s1);
        attr_l_2 = attr + "3" + "2";
        // H(attr_l_2)^s2
        this->Hash(this->tmp_G_2, attr_l_2);
        element_pow_zn(this->tmp_G_2, this->tmp_G_2, this->s2);
        element_mul(tmp_G_4, this->tmp_G, this->tmp_G_2);
        // for j = 1,2,...,cols
        for(unsigned long int j=0; j<cols;j++){
            str_0jl1 = "0" + std::to_string(j+1) + "3" + "1";
            str_0jl2 = "0" + std::to_string(j+1) + "3" + "2";
            // H(0jl1)^s1
            this->Hash(this->tmp_G, str_0jl1);
            element_pow_zn(this->tmp_G, this->tmp_G, this->s1);
            // H(0jl2)^s2
            this->Hash(this->tmp_G_2, str_0jl2);
            element_pow_zn(this->tmp_G_2, this->tmp_G_2, this->s2);
            // H(0jl1)^s1 * H(0jl2)^s2
            element_mul(this->tmp_G_3, this->tmp_G, this->tmp_G_2);
            // (H(0jl1)^s1 * H(0jl2)^s2)^M[i][j]
            element_pow_zn(this->tmp_G_3, this->tmp_G_3, M->getElement(i, j));
            element_mul(tmp_G_4, tmp_G_4, this->tmp_G_3);
        }
        tmp_ct_y.insertElement("ct_3", "G1", tmp_G_4);

        ciphertext->get_ct_y()->push_back(tmp_ct_y);
    }
}

/**
 * input: mpk, ciphertext, dkidt, 
 * output: res
 */
void RABE_XNM::Dec(element_t res, RABE_XNM_mpk *mpk, RABE_XNM_ciphertext *ciphertext, RABE_XNM_dkidt *dkidt)
{   
    // compute Yi
    // get original matrix
    Policy_resolution pr;
    Policy_generation pg;
    element_random(this->tmp_Zn);
    std::vector<std::string>* postfix_expression = pr.infixToPostfix(this->policy_str);
    Binary_tree_policy* binary_tree_expression = pr.postfixToBinaryTree(postfix_expression, this->tmp_Zn);
    pg.generatePolicyInMatrixForm(binary_tree_expression);
    Element_t_matrix* M = pg.getPolicyInMatrixFormFromTree(binary_tree_expression);
    // get matrix with attributes
    Element_t_matrix* attributesMatrix = new Element_t_matrix();
    unsigned long int rows = ciphertext->get_ct_y()->size();
    for(unsigned long int i=0; i<rows;i++){
        // judge whether the attribute is in the policy
        if(attr_map.find(pai[i]) == attr_map.end()){
            continue;
        }
        Element_t_vector *v = new Element_t_vector();
        for (signed long int j = 0; j < M->col(); ++j) {
            v->pushBack(M->getElement(i, j));
        }
        attributesMatrix->pushBack(v);
    }
    // get inverse matrix
    Element_t_matrix* inverse_attributesMatrix = attributesMatrix->inverse();

    unsigned long int r = inverse_attributesMatrix->row();
    unsigned long int c = inverse_attributesMatrix->col();
    printf("rows: %ld, cols: %ld\n", r, c);
    for(int i = 0;i < r;i++){
        for(int j = 0;j < c;j++){
            element_printf("%B ", inverse_attributesMatrix->getElement(i, j));
        }
        printf("\n");
    }
    Element_t_vector* unit = inverse_attributesMatrix->getCoordinateAxisUnitVector();

    Element_t_vector* x= new Element_t_vector(inverse_attributesMatrix->col(), inverse_attributesMatrix->getElement(0, 0));

    signed long int type = Element_t_matrix::gaussElimination(x, inverse_attributesMatrix, unit);
    if (-1 == type) {
        throw std::runtime_error("POLICY_NOT_SATISFIED");
    }
    printf("type: %ld\n", type);
    // print x
    printf("Yi:\n");
    x->printVector();


    // num
    element_t num,den;
    element_init_same_as(num, this->GT);
    element_init_same_as(den, this->GT);

    
    element_set1(this->tmp_G);
    element_set1(this->tmp_G_2);
    element_set1(this->tmp_G_3);
    int count = 0;
    for(unsigned long int i=0; i<rows;i++){
        // judge whether the attribute is in the policy
        if(attr_map.find(pai[i]) == attr_map.end()){
            continue;
        }
        element_pow_zn(this->tmp_G_4, ciphertext->get_ct_y(i)->getElement("ct_1"), x->getElement(count));
        element_mul(this->tmp_G, this->tmp_G, this->tmp_G_4);
        element_pow_zn(this->tmp_G_4, ciphertext->get_ct_y(i)->getElement("ct_2"), x->getElement(count));
        element_mul(this->tmp_G_2, this->tmp_G_2, this->tmp_G_4);
        element_pow_zn(this->tmp_G_4, ciphertext->get_ct_y(i)->getElement("ct_3"), x->getElement(count));
        element_mul(this->tmp_G_3, this->tmp_G_3, this->tmp_G_4);
        count++;
    }
    // ct_prime * e(tmp_G, sk0_1) * e(tmp_G_2, sk0_2) * e(tmp_G_3, sk0_3) * e(ct0_4, sk0_4)
    element_pairing(this->tmp_GT, this->tmp_G, dkidt->get_sk0_prime()->getElement("sk_1"));
    element_pairing(this->tmp_GT_2, this->tmp_G_2, dkidt->get_sk0_prime()->getElement("sk_2"));
    element_pairing(this->tmp_GT_3, this->tmp_G_3, dkidt->get_sk0_prime()->getElement("sk_3"));
    element_pairing(this->tmp_GT_4, ciphertext->get_ct0()->getElement("ct0_4"), dkidt->get_sk0_prime()->getElement("sk_4"));

    element_mul(num, ciphertext->get_ct_prime()->getElement("ct_prime"), this->tmp_GT);
    element_mul(num, num, this->tmp_GT_2);
    element_mul(num, num, this->tmp_GT_3);
    element_mul(num, num, this->tmp_GT_4);

    // den
    element_set1(this->tmp_G);
    element_set1(this->tmp_G_2);
    element_set1(this->tmp_G_3);
    count = 0;
    for(unsigned long int i=0; i<rows;i++){
        // judge whether the attribute is in the policy
        if(attr_map.find(pai[i]) == attr_map.end()){
            continue;
        }
        element_pow_zn(this->tmp_G_4, dkidt->get_sk_y(attr_map[pai[i]])->getElement("sk_1"), x->getElement(count));
        element_mul(this->tmp_G, this->tmp_G, this->tmp_G_4);
        element_pow_zn(this->tmp_G_4, dkidt->get_sk_y(attr_map[pai[i]])->getElement("sk_2"), x->getElement(count));
        element_mul(this->tmp_G_2, this->tmp_G_2, this->tmp_G_4);
        element_pow_zn(this->tmp_G_4, dkidt->get_sk_y(attr_map[pai[i]])->getElement("sk_3"), x->getElement(count));
        element_mul(this->tmp_G_3, this->tmp_G_3, this->tmp_G_4);
        count++;
    }
    // sk_prime_1 * tmp_G
    element_mul(this->tmp_G, dkidt->get_sk_prime_prime()->getElement("sk_1"), this->tmp_G);
    // sk_prime_2 * tmp_G_2
    element_mul(this->tmp_G_2, dkidt->get_sk_prime_prime()->getElement("sk_2"), this->tmp_G_2);
    // sk_prime_3 * tmp_G_3
    element_mul(this->tmp_G_3, dkidt->get_sk_prime_prime()->getElement("sk_3"), this->tmp_G_3);

    // e(tmp_G, ct01) * e(tmp_G_2, ct02) * e(tmp_G_3, ct03)
    element_pairing(this->tmp_GT, this->tmp_G, ciphertext->get_ct0()->getElement("ct0_1"));
    element_pairing(this->tmp_GT_2, this->tmp_G_2, ciphertext->get_ct0()->getElement("ct0_2"));
    element_pairing(this->tmp_GT_3, this->tmp_G_3, ciphertext->get_ct0()->getElement("ct0_3"));

    element_mul(den, this->tmp_GT, this->tmp_GT_2);
    element_mul(den, den, this->tmp_GT_3);

    // res = num / den
    element_div(res, num, den);

    element_clear(num);
    element_clear(den);
}

/**
 * input: rl, id, t
 */
void RABE_XNM::Rev(std::vector<RABE_XNM_revokedPreson *> *rl, element_t id, time_t t){
    RABE_XNM_revokedPreson *rp = new RABE_XNM_revokedPreson();
    rp->insertElement("id", "Zn", id);
    rp->setTime(t);
    rl->push_back(rp);
}


RABE_XNM::~RABE_XNM(){
    element_clear(this->d1);
    element_clear(this->d2);
    element_clear(this->d3);

    element_clear(this->r1);
    element_clear(this->r2);

    element_clear(this->b1r1a1);
    element_clear(this->b1r1a2);
    element_clear(this->b2r2a1);
    element_clear(this->b2r2a2);
    element_clear(this->r1r2a1);
    element_clear(this->r1r2a2);

    element_clear(this->s1);
    element_clear(this->s2);
}