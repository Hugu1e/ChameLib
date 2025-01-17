#include <ABE/CP_ABE.h>

CP_ABE::CP_ABE(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn):G1(_G1),G2(_G2),GT(_GT),Zn(_Zn){
    element_init_same_as(this->tmp_G, G1);
    element_init_same_as(this->tmp_G_2, G1);
    element_init_same_as(this->tmp_G_3, G1);
    element_init_same_as(this->tmp_G_4, G1);
    element_init_same_as(this->tmp_H, G2);
    element_init_same_as(this->tmp_H_2, G2);
    element_init_same_as(this->tmp_H_3, G2);
    element_init_same_as(this->tmp_GT, GT);
    element_init_same_as(this->tmp_GT_2, GT);
    element_init_same_as(this->tmp_GT_3, GT);
    element_init_same_as(this->tmp_Zn, Zn);
    element_init_same_as(this->tmp_Zn_2, Zn);
    element_init_same_as(this->tmp_Zn_3, Zn);

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

    element_init_same_as(this->g_pow_d1, G1);
    element_init_same_as(this->g_pow_d2, G1);
    element_init_same_as(this->g_pow_d3, G1);

    element_init_same_as(this->H1, G2);
    element_init_same_as(this->H2, G2);
    element_init_same_as(this->T1, GT);
    element_init_same_as(this->T2, GT);

    element_init_same_as(this->sk_1_G, G1);
    element_init_same_as(this->sk_2_G, G1);
    element_init_same_as(this->sk_3_G, G1);
    element_init_same_as(this->sk_1_H, G2);
    element_init_same_as(this->sk_2_H, G2);
    element_init_same_as(this->sk_3_H, G2);

    element_init_same_as(this->ct_1_G, G1);
    element_init_same_as(this->ct_2_G, G1);
    element_init_same_as(this->ct_3_G, G1);
    element_init_same_as(this->ct_1_H, G2);
    element_init_same_as(this->ct_2_H, G2);
    element_init_same_as(this->ct_3_H, G2);
    element_init_same_as(this->ct_prime, GT);
}

/**
 * output: mpk, msk
 */
void CP_ABE::Setup(CP_ABE_msk *msk, CP_ABE_mpk *mpk){
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
    // g_pow_d1 = g^d1
    element_pow_zn(this->g_pow_d1, msk->getElement("g"), this->d1);
    msk->insertElement("g_pow_d1", "G1", this->g_pow_d1);
    element_random(this->d2);
    // g_pow_d2 = g^d2
    element_pow_zn(this->g_pow_d2, msk->getElement("g"), this->d2);
    msk->insertElement("g_pow_d2", "G1", this->g_pow_d2);
    element_random(this->d3);
    // g_pow_d3 = g^d3
    element_pow_zn(this->g_pow_d3, msk->getElement("g"), this->d3);
    msk->insertElement("g_pow_d3", "G1", this->g_pow_d3);

    mpk->insertElement("h", "G2", msk->getElement("h"));
    // H1 = h^a1
    element_pow_zn(H1, msk->getElement("h"), msk->getElement("a1"));
    mpk->insertElement("H1", "G2", H1);
    // H2 = h^a2
    element_pow_zn(H2, msk->getElement("h"), msk->getElement("a2"));
    mpk->insertElement("H2", "G2", H2);
    // T1 = e(g, h)^(d1*a1+d3)
    element_mul(this->tmp_Zn, this->d1, msk->getElement("a1"));
    element_add(this->tmp_Zn_2, this->tmp_Zn, this->d3);
    element_pairing(this->tmp_GT, msk->getElement("g"), msk->getElement("h"));
    element_pow_zn(T1, this->tmp_GT, this->tmp_Zn_2);
    mpk->insertElement("T1", "GT", T1);
    // T2 = e(g, h)^(d2*a2+d3)
    element_mul(this->tmp_Zn, this->d2, msk->getElement("a2"));
    element_add(this->tmp_Zn_2, this->tmp_Zn, this->d3);
    element_pow_zn(T2, this->tmp_GT, this->tmp_Zn_2);
    mpk->insertElement("T2", "GT", T2);
}


/**
 * Generate a key for a list of attributes.
 * input: msk, mpk, attr
 * output: sks
 */
void CP_ABE::KeyGen(CP_ABE_sks *sks, CP_ABE_msk *msk, CP_ABE_mpk *mpk, std::vector<std::string> *attr_list){
    element_random(this->r1);
    element_random(this->r2);
    // sk0 = (h^(b1r1), h^(b2r2), h^(r1+r2))
    element_mul(this->tmp_Zn, msk->getElement("b1"), this->r1);
    element_pow_zn(sk_1_H, mpk->getElement("h"), this->tmp_Zn);
    sks->get_sk0()->insertElement("sk_1", "G2", sk_1_H);
    // (b1 * r1) / a1
    element_div(this->b1r1a1, this->tmp_Zn, msk->getElement("a1"));
    // (b1 * r1) / a2
    element_div(this->b1r1a2, this->tmp_Zn, msk->getElement("a2"));
    element_mul(this->tmp_Zn, msk->getElement("b2"), this->r2);
    element_pow_zn(sk_2_H, mpk->getElement("h"), this->tmp_Zn);
    sks->get_sk0()->insertElement("sk_2", "G2", sk_2_H);
    // (b2 * r2) / a1
    element_div(this->b2r2a1, this->tmp_Zn, msk->getElement("a1"));
    // (b2 * r2) / a2
    element_div(this->b2r2a2, this->tmp_Zn, msk->getElement("a2"));
    element_add(this->tmp_Zn, this->r1, this->r2);
    element_pow_zn(sk_3_H, mpk->getElement("h"), this->tmp_Zn);
    sks->get_sk0()->insertElement("sk_3", "G2", sk_3_H);
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
        element_mul(sk_1_G, this->tmp_G, this->tmp_G_2);
        element_mul(sk_1_G, sk_1_G, this->tmp_G_3);
        element_mul(sk_1_G, sk_1_G, this->tmp_G_4);

        PbcElements tmp_sky;
        tmp_sky.insertElement("sk_1", "G1", sk_1_G);

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
        element_mul(sk_2_G, this->tmp_G, this->tmp_G_2);
        element_mul(sk_2_G, sk_2_G, this->tmp_G_3);
        element_mul(sk_2_G, sk_2_G, this->tmp_G_4);
        tmp_sky.insertElement("sk_2", "G1", sk_2_G);

        // sky3 = g^(-sigma_y)
        element_neg(this->tmp_Zn, this->tmp_Zn);
        element_pow_zn(sk_3_G, msk->getElement("g"), this->tmp_Zn);
        tmp_sky.insertElement("sk_3", "G1", sk_3_G);

        sks->get_sk_y()->push_back(tmp_sky);
    }

    // sk_prime
    // sigma_prime
    element_random(this->tmp_Zn);
    // t = 1
    // g^d1
    element_pow_zn(sk_1_G, msk->getElement("g"), this->d1);
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
    element_mul(sk_1_G, sk_1_G, this->tmp_G);
    element_mul(sk_1_G, sk_1_G, this->tmp_G_2);
    element_mul(sk_1_G, sk_1_G, this->tmp_G_3);
    element_mul(sk_1_G, sk_1_G, this->tmp_G_4);
    sks->get_sk_prime()->insertElement("sk_1", "G1", sk_1_G);

    // t = 2
    // g^d2
    element_pow_zn(sk_2_G, msk->getElement("g"), this->d2);
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
    element_mul(sk_2_G, sk_2_G, this->tmp_G);
    element_mul(sk_2_G, sk_2_G, this->tmp_G_2);
    element_mul(sk_2_G, sk_2_G, this->tmp_G_3);
    element_mul(sk_2_G, sk_2_G, this->tmp_G_4);
    sks->get_sk_prime()->insertElement("sk_2", "G1", sk_2_G);
   
    // sk_prime3 = g^d3 * g ^ (-sigma_prime)
    element_neg(this->tmp_Zn, this->tmp_Zn);
    element_pow_zn(sk_3_G, msk->getElement("g"), this->tmp_Zn);
    element_mul(sk_3_G, sk_3_G, msk->getElement("g_pow_d3"));
    sks->get_sk_prime()->insertElement("sk_3", "G1", sk_3_G);
}

/**
 * hash function {0,1}* -> G
 * input: m
 * output: res
 */
void CP_ABE::Hash(element_t res, std::string m){
    HASH::hash(res, m);
}


/**
 * Encrypt a message msg under a policy string.
 * input: mpk, msg, policy_str
 * output: ct
 */
void CP_ABE::Encrypt(CP_ABE_ciphertext *ciphertext, CP_ABE_mpk *mpk, element_t msg, std::string policy_str){
    element_t s1,s2;
    element_init_same_as(s1, Zn);
    element_init_same_as(s2, Zn);
    element_random(s1);
    element_random(s2);
    Encrypt(ciphertext, mpk, msg, policy_str, s1, s2);
    element_clear(s1);
    element_clear(s2);
}

/**
 * Encrypt a message msg under a policy string.
 * input: mpk, msg, policy_str, s1, s2
 * output: ct
 */
void CP_ABE::Encrypt(CP_ABE_ciphertext *ciphertext, CP_ABE_mpk *mpk, element_t msg, std::string policy_str, element_t s1, element_t s2){
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

    // ct0
    // ct0_1 = H1^s1
    element_pow_zn(ct_1_H, mpk->getElement("H1"), this->s1);
    ciphertext->get_ct0()->insertElement("ct_1", "G2", ct_1_H);
    // ct0_2 = H2^s2
    element_pow_zn(ct_2_H, mpk->getElement("H2"), this->s2);
    ciphertext->get_ct0()->insertElement("ct_2", "G2", ct_2_H);
    // ct0_3 = h^(s1+s2)
    element_add(this->tmp_Zn_2, this->s1, this->s2);
    element_pow_zn(ct_3_H, mpk->getElement("h"), this->tmp_Zn_2);
    ciphertext->get_ct0()->insertElement("ct_3", "G2", ct_3_H);

    // ct_prime = T1^s1 * T2^s2 * msg
    element_pow_zn(this->tmp_GT, mpk->getElement("T1"), this->s1);
    element_pow_zn(this->tmp_GT_2, mpk->getElement("T2"), this->s2);
    element_mul(this->tmp_GT_3, this->tmp_GT, this->tmp_GT_2);
    element_mul(ct_prime, this->tmp_GT_3, msg);
    ciphertext->get_ct_prime()->insertElement("ct_prime", "GT", ct_prime);

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
        element_mul(ct_1_G, this->tmp_G, this->tmp_G_2);
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
            element_pow_zn(this->tmp_G_4, this->tmp_G_3, M->getElement(i, j));
            element_mul(ct_1_G, ct_1_G, this->tmp_G_4);
        }
        PbcElements tmp_ct_y;
        tmp_ct_y.insertElement("ct_1", "G1", ct_1_G);
    
        // l = 2
        attr_l_1 = attr + "2" + "1";
        // H(attr_l_1)^s1
        this->Hash(this->tmp_G, attr_l_1);
        element_pow_zn(this->tmp_G, this->tmp_G, this->s1);
        attr_l_2 = attr + "2" + "2";
        // H(attr_l_2)^s2
        this->Hash(this->tmp_G_2, attr_l_2);
        element_pow_zn(this->tmp_G_2, this->tmp_G_2, this->s2);
        element_mul(ct_2_G, this->tmp_G, this->tmp_G_2);
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
            element_pow_zn(this->tmp_G_4, this->tmp_G_3, M->getElement(i, j));
            element_mul(ct_2_G, ct_2_G, this->tmp_G_4);
        }
        tmp_ct_y.insertElement("ct_2", "G1", ct_2_G);

        // l = 3
        attr_l_1 = attr + "3" + "1";
        // H(attr_l_1)^s1
        this->Hash(this->tmp_G, attr_l_1);
        element_pow_zn(this->tmp_G, this->tmp_G, this->s1);
        attr_l_2 = attr + "3" + "2";
        // H(attr_l_2)^s2
        this->Hash(this->tmp_G_2, attr_l_2);
        element_pow_zn(this->tmp_G_2, this->tmp_G_2, this->s2);
        element_mul(ct_3_G, this->tmp_G, this->tmp_G_2);
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
            element_pow_zn(this->tmp_G_4, this->tmp_G_3, M->getElement(i, j));
            element_mul(ct_3_G, ct_3_G, this->tmp_G_4);
        }
        tmp_ct_y.insertElement("ct_3", "G1", ct_3_G);

        ciphertext->get_ct_y()->push_back(tmp_ct_y);
    }
}

/**
 * Decrypt a ciphertext.
 * input: mpk, ciphertext, sks
 * output: res
 */
void CP_ABE::Decrypt(element_t res, CP_ABE_ciphertext *ciphertext,  CP_ABE_mpk *mpk, CP_ABE_sks *sks){
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

    // // 验算 inverse_attributesMatrix * x = unit
    // element_t_vector* test = new element_t_vector();
    // for (unsigned long int i = 0; i < inverse_attributesMatrix->row(); ++i) {
    //     element_set0(this->tmp_Zn);
    //     PrintElement("add0",this->tmp_Zn);
    //     for (unsigned long int j = 0; j < inverse_attributesMatrix->col(); ++j) {
    //         element_mul(this->tmp_Zn_2,inverse_attributesMatrix->getElement(i, j), x->getElement(j));
    //         PrintElement("mul1",this->tmp_Zn_2);
    //         element_add(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    //     }
    //     PrintElement("res0",this->tmp_Zn);
    //     test->pushBack(this->tmp_Zn);
    // }
    // printf("test:\n");
    // test->printVector();

    // num
    element_t num,den;
    element_init_same_as(num, GT);
    element_init_same_as(den, GT);

    
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
    // ct_prime * e(tmp_G, sk0_1) * e(tmp_G_2, sk0_2) * e(tmp_G_3, sk0_3)
    element_pairing(this->tmp_GT, this->tmp_G, sks->get_sk0()->getElement("sk_1"));
    element_pairing(this->tmp_GT_2, this->tmp_G_2, sks->get_sk0()->getElement("sk_2"));
    element_pairing(this->tmp_GT_3, this->tmp_G_3, sks->get_sk0()->getElement("sk_3"));

    element_mul(num, ciphertext->get_ct_prime()->getElement("ct_prime"), this->tmp_GT);
    element_mul(num, num, this->tmp_GT_2);
    element_mul(num, num, this->tmp_GT_3);

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
        
        element_pow_zn(this->tmp_G_4, sks->get_sk_y(attr_map[pai[i]])->getElement("sk_1"), x->getElement(count));
        element_mul(this->tmp_G, this->tmp_G, this->tmp_G_4);
        element_pow_zn(this->tmp_G_4, sks->get_sk_y(attr_map[pai[i]])->getElement("sk_2"), x->getElement(count));
        element_mul(this->tmp_G_2, this->tmp_G_2, this->tmp_G_4);
        element_pow_zn(this->tmp_G_4, sks->get_sk_y(attr_map[pai[i]])->getElement("sk_3"), x->getElement(count));
        element_mul(this->tmp_G_3, this->tmp_G_3, this->tmp_G_4);
        count++;
    }
    // sk_prime_1 * tmp_G
    element_mul(this->tmp_G, sks->get_sk_prime()->getElement("sk_1"), this->tmp_G);
    // sk_prime_2 * tmp_G_2
    element_mul(this->tmp_G_2, sks->get_sk_prime()->getElement("sk_2"), this->tmp_G_2);
    // sk_prime_3 * tmp_G_3
    element_mul(this->tmp_G_3, sks->get_sk_prime()->getElement("sk_3"), this->tmp_G_3);

    // e(tmp_G, ct01) * e(tmp_G_2, ct02) * e(tmp_G_3, ct03)
    element_pairing(this->tmp_GT, this->tmp_G, ciphertext->get_ct0()->getElement("ct_1"));
    element_pairing(this->tmp_GT_2, this->tmp_G_2, ciphertext->get_ct0()->getElement("ct_2"));
    element_pairing(this->tmp_GT_3, this->tmp_G_3, ciphertext->get_ct0()->getElement("ct_3"));

    element_mul(den, this->tmp_GT, this->tmp_GT_2);
    element_mul(den, den, this->tmp_GT_3);

    // res = num / den
    element_div(res, num, den);

    element_clear(num);
    element_clear(den);
}


CP_ABE::~CP_ABE(){
    element_clear(this->tmp_G);
    element_clear(this->tmp_G_2);
    element_clear(this->tmp_G_3);
    element_clear(this->tmp_G_4);
    element_clear(this->tmp_H);
    element_clear(this->tmp_H_2);
    element_clear(this->tmp_H_3);
    element_clear(this->tmp_GT);
    element_clear(this->tmp_GT_2);
    element_clear(this->tmp_GT_3);
    element_clear(this->tmp_Zn);
    element_clear(this->tmp_Zn_2);
    element_clear(this->tmp_Zn_3);

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

    element_clear(this->g_pow_d1);
    element_clear(this->g_pow_d2);
    element_clear(this->g_pow_d3);
    
    element_clear(this->H1);
    element_clear(this->H2);
    element_clear(this->T1);
    element_clear(this->T2);

    element_clear(this->sk_1_G);
    element_clear(this->sk_2_G);
    element_clear(this->sk_3_G);
    element_clear(this->sk_1_H);
    element_clear(this->sk_2_H);
    element_clear(this->sk_3_H);

    element_clear(this->ct_1_G);
    element_clear(this->ct_2_G);
    element_clear(this->ct_3_G);
    element_clear(this->ct_1_H);
    element_clear(this->ct_2_H);
    element_clear(this->ct_3_H);
    element_clear(this->ct_prime);
}