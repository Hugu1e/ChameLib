#include "ABE/ABET.h"

ABET::ABET(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn):PbcScheme(_G1, _G2, _GT, _Zn){
    element_init_same_as(this->d1, Zn);
    element_init_same_as(this->d2, Zn);
    element_init_same_as(this->d3, Zn);

    element_init_same_as(this->r1, Zn);
    element_init_same_as(this->r2, Zn);
    element_init_same_as(this->R, Zn);

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
 * output: mpk, msk
 */
void ABET::Setup(ABET_msk &msk, ABET_mpk &mpk, ABET_sks &sks, ABET_ciphertext &ciphertext, int k){
    msk.init(9);
    msk.get_zk().init(k);
    mpk.init(10);
    mpk.get_gk().init(k);
    mpk.get_gk_pow_a().init(k);
    mpk.get_hk().init(k);

    sks.get_sk0().init(6);
    sks.get_sk_prime().init(3);
    sks.get_sk1().init(1);

    ciphertext.get_ct0().init(4);
    ciphertext.get_ct_().init(1);
    ciphertext.get_ct_prime().init(1);
    ciphertext.get_ct1().init(1);
    ciphertext.get_ct2().init(1);
    ciphertext.get_ct3().init(1);

    this->k = k;
    element_random(tmp_Zn);
    msk.set(a1, tmp_Zn);
    element_random(tmp_Zn);
    msk.set(a2, tmp_Zn);
    element_random(tmp_Zn);
    msk.set(b1, tmp_Zn);
    element_random(tmp_Zn);
    msk.set(b2, tmp_Zn);
    element_random(tmp_Zn);
    msk.set(a, tmp_Zn);
    element_random(tmp_Zn);
    msk.set(b, tmp_Zn);

    element_random(tmp_G);
    mpk.set(g, tmp_G);
    element_random(tmp_H);
    mpk.set(h, tmp_H);

    element_random(this->d1);
    // g_pow_d1 = g^d1
    element_pow_zn(tmp_G, mpk[g], this->d1);
    msk.set(g_pow_d1, tmp_G);
    element_random(this->d2);
    // g_pow_d2 = g^d2
    element_pow_zn(tmp_G, mpk[g], this->d2);
    msk.set(g_pow_d2, tmp_G);
    element_random(this->d3);
    // g_pow_d3 = g^d3
    element_pow_zn(tmp_G, mpk[g], this->d3);
    msk.set(g_pow_d3, tmp_G);

    // z1,z2,...zk
    for(int i=1;i <= k;i++){
        element_random(tmp_Zn);
        msk.get_zk().set(i-1, tmp_Zn);
    }

    // H1 = h^a1
    element_pow_zn(tmp_H, mpk[h], msk[a1]);
    mpk.set(H1, tmp_H);
    // H2 = h^a2
    element_pow_zn(tmp_H, mpk[h], msk[a2]);
    mpk.set(H2, tmp_H);
    // T1 = e(g, h)^(d1*a1+d3/a)
    element_mul(this->tmp_Zn, this->d1, msk[a1]);
    element_div(this->tmp_Zn_2, this->d3, msk[a]);
    element_add(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    element_pairing(this->tmp_GT, mpk[g], mpk[h]);
    element_pow_zn(tmp_GT_2, this->tmp_GT, this->tmp_Zn);
    mpk.set(T1, tmp_GT_2);
    // T2 = e(g, h)^(d2*a2+d3/a)
    element_mul(this->tmp_Zn, this->d2, msk[a2]);
    element_div(this->tmp_Zn_2, this->d3, msk[a]);
    element_add(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    element_pow_zn(tmp_GT_2, this->tmp_GT, this->tmp_Zn);
    mpk.set(T2, tmp_GT_2);

    // gk, gk_pow_a, hk
    for(int i = 1;i <= k;i++){
        // gk = g^zk
        element_pow_zn(tmp_G, mpk[g], msk.get_zk()[i-1]);
        mpk.get_gk().set(i-1, tmp_G);
        // gk_pow_a = gk^a
        element_pow_zn(tmp_G_2, tmp_G, msk[a]);
        mpk.get_gk_pow_a().set(i-1, tmp_G_2);
        // hk = h^zk
        element_pow_zn(tmp_H, mpk[h], msk.get_zk()[i-1]);
        mpk.get_hk().set(i-1, tmp_H);
    }
    // g^a
    element_pow_zn(tmp_G, mpk[g], msk[a]);
    mpk.set(g_pow_a, tmp_G);
    // h^(d/a)
    // d = d1 + d2 + d3
    element_add(this->tmp_Zn, this->d1, this->d2);
    element_add(this->tmp_Zn, this->tmp_Zn, this->d3);
    element_div(this->tmp_Zn, this->tmp_Zn, msk[a]);
    element_pow_zn(tmp_H, mpk[h], this->tmp_Zn);
    mpk.set(h_pow_d_div_a, tmp_H);
    // h^(1/a)
    element_invert(this->tmp_Zn, msk[a]);
    element_pow_zn(tmp_H, mpk[h], this->tmp_Zn);
    mpk.set(h_pow_1_div_a, tmp_H);
    // h^(b/a)
    element_div(this->tmp_Zn, msk[b], msk[a]);
    element_pow_zn(tmp_H, mpk[h], this->tmp_Zn);
    mpk.set(h_pow_b_div_a, tmp_H);
}
/**
 * Generate a key for a list of attributes.
 * input: msk, mpk, attr , ID, i
 * output: sks
 */
void ABET::KeyGen(ABET_sks &sks, ABET_msk &msk, ABET_mpk &mpk, std::vector<std::string> &attr_list, ABET_ID &ID, int mi){  
    element_random(this->r1);
    element_random(this->r2);
    element_random(this->R);
    // sk0 = (h^(b1r1), h^(b2r2), h^(r1+r2)/a, g^(1/a), g^(r/a), g^R) 
    element_mul(this->tmp_Zn, msk[b1], this->r1);
    element_pow_zn(tmp_H, mpk[h], this->tmp_Zn);
    sks.get_sk0().set(sk0_1, tmp_H);
    // (b1 * r1) / a1
    element_div(this->b1r1a1, this->tmp_Zn, msk[a1]);
    // (b1 * r1) / a2
    element_div(this->b1r1a2, this->tmp_Zn, msk[a2]);
    element_mul(this->tmp_Zn, msk[b2], this->r2);
    element_pow_zn(tmp_H, mpk[h], this->tmp_Zn);
    sks.get_sk0().set(sk0_2, tmp_H);
    // (b2 * r2) / a1
    element_div(this->b2r2a1, this->tmp_Zn, msk[a1]);
    // (b2 * r2) / a2
    element_div(this->b2r2a2, this->tmp_Zn, msk[a2]);
    element_add(this->tmp_Zn, this->r1, this->r2);
    element_div(this->tmp_Zn_2, this->tmp_Zn, msk[a]);
    element_pow_zn(tmp_H, mpk[h], this->tmp_Zn_2);
    sks.get_sk0().set(sk0_3, tmp_H);
    // (r1 + r2) / a1
    element_div(this->r1r2a1, this->tmp_Zn, msk[a1]);
    // (r1 + r2) / a2
    element_div(this->r1r2a2, this->tmp_Zn, msk[a2]);
    // g^(1/a)
    element_invert(this->tmp_Zn, msk[a]);
    element_pow_zn(tmp_G, mpk[g], this->tmp_Zn);
    sks.get_sk0().set(sk0_4, tmp_G);
    // g^(r/a)
    element_add(this->tmp_Zn, this->r1, this->r2);
    element_div(this->tmp_Zn, this->tmp_Zn, msk[a]);
    element_pow_zn(tmp_G, mpk[g], this->tmp_Zn);
    sks.get_sk0().set(sk0_5, tmp_G);
    // g^R
    element_pow_zn(tmp_G, mpk[g], this->R);
    sks.get_sk0().set(sk0_6, tmp_G);

    // compute sk_y
    sks.get_sk_y().resize(attr_list.size());
    for (int i = 0; i < attr_list.size(); i++) {
        attr_map[attr_list[i]] = i;
        // sigma_y
        element_random(this->tmp_Zn);
        // t = 1
        // H(y11)^b1r1a1
        std::string y11 = attr_list[i] + "1" + "1";
        this->Hash(this->tmp_G, y11);
        element_pow_zn(this->tmp_G, this->tmp_G, this->b1r1a1);
        // H(y21)^b2r2a1
        std::string y21 = attr_list[i] + "2" + "1";
        this->Hash(this->tmp_G_2, y21);
        element_pow_zn(this->tmp_G_2, this->tmp_G_2, this->b2r2a1);
        // H(y31)^(r1r2a1/a)
        std::string y31 = attr_list[i] + "3" + "1";
        this->Hash(this->tmp_G_3, y31);
        element_div(this->tmp_Zn_2, this->r1r2a1, msk[a]);
        element_pow_zn(this->tmp_G_3, this->tmp_G_3, this->tmp_Zn_2);
        // g^(sigma_y / (a*a1))
        element_mul(this->tmp_Zn_2, msk[a], msk[a1]);
        element_div(this->tmp_Zn_2, this->tmp_Zn, this->tmp_Zn_2);
        element_pow_zn(this->tmp_G_4, mpk[g], this->tmp_Zn_2);
        // sky1
        element_mul(tmp_G, this->tmp_G, this->tmp_G_2);
        element_mul(tmp_G, tmp_G, this->tmp_G_3);
        element_mul(tmp_G, tmp_G, this->tmp_G_4);

        PbcElements sk_y;
        sk_y.init(3);
        sk_y.set(sk_1, tmp_G);

        // t = 2
        // H(y12)^b1r1a2
        std::string y12 = attr_list[i] + "1" + "2";
        this->Hash(this->tmp_G, y12);
        element_pow_zn(this->tmp_G, this->tmp_G, this->b1r1a2);
        // H(y22)^b2r2a2
        std::string y22 = attr_list[i] + "2" + "2";
        this->Hash(this->tmp_G_2, y22);
        element_pow_zn(this->tmp_G_2, this->tmp_G_2, this->b2r2a2);
        // H(y32)^(r1r2a2/a)
        std::string y32 = attr_list[i] + "3" + "2";
        this->Hash(this->tmp_G_3, y32);
        element_div(this->tmp_Zn_2, this->r1r2a2, msk[a]);
        element_pow_zn(this->tmp_G_3, this->tmp_G_3, this->tmp_Zn_2);
        // g^(sigma_y / (a*a2))
        element_mul(this->tmp_Zn_2, msk[a], msk[a2]);
        element_div(this->tmp_Zn_2, this->tmp_Zn, this->tmp_Zn_2);
        element_pow_zn(this->tmp_G_4, mpk[g], this->tmp_Zn_2);
        // sky2
        element_mul(tmp_G, this->tmp_G, this->tmp_G_2);
        element_mul(tmp_G, tmp_G, this->tmp_G_3);
        element_mul(tmp_G, tmp_G, this->tmp_G_4);

        sk_y.set(sk_2, tmp_G);

        // sky3 = g^(-sigma_y)
        element_neg(this->tmp_Zn, this->tmp_Zn);
        element_pow_zn(tmp_G, mpk[g], this->tmp_Zn);

        sk_y.set(sk_3, tmp_G);

        sks.get_sk_y()[i] = sk_y;
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
    // H(0131)^(r1r2a1/a)
    std::string y0131 = "0131";
    this->Hash(this->tmp_G_3, y0131);
    element_div(this->tmp_Zn_2, this->r1r2a1, msk[a]);
    element_pow_zn(this->tmp_G_3, this->tmp_G_3, this->tmp_Zn_2);
    // g^(sigma_prime / (a*a1))
    element_mul(this->tmp_Zn_2, msk[a], msk[a1]);
    element_div(this->tmp_Zn_2, this->tmp_Zn, this->tmp_Zn_2);
    element_pow_zn(this->tmp_G_4, mpk[g], this->tmp_Zn_2);
    // sk_prime1
    element_mul(tmp_G, msk[g_pow_d1], this->tmp_G);
    element_mul(tmp_G, tmp_G, this->tmp_G_2);
    element_mul(tmp_G, tmp_G, this->tmp_G_3);
    element_mul(tmp_G, tmp_G, this->tmp_G_4);
    sks.get_sk_prime().set(sk_1, tmp_G);

    // t = 2
    // H(0112)^b1r1a2
    std::string y0112 = "0112";
    this->Hash(this->tmp_G, y0112);
    element_pow_zn(this->tmp_G, this->tmp_G, this->b1r1a2);
    // H(0122)^b2r2a2
    std::string y0122 = "0122";
    this->Hash(this->tmp_G_2, y0122);
    element_pow_zn(this->tmp_G_2, this->tmp_G_2, this->b2r2a2);
    // H(0132)^(r1r2a2/a)
    std::string y0132 = "0132";
    this->Hash(this->tmp_G_3, y0132);
    element_div(this->tmp_Zn_2, this->r1r2a2, msk[a]);
    element_pow_zn(this->tmp_G_3, this->tmp_G_3, this->tmp_Zn_2);
    // g^(sigma_prime / (a*a2))
    element_mul(this->tmp_Zn_2, msk[a], msk[a2]);
    element_div(this->tmp_Zn_2, this->tmp_Zn, this->tmp_Zn_2);
    element_pow_zn(this->tmp_G_4, mpk[g], this->tmp_Zn_2);
    // sk_prime2
    element_mul(tmp_G, msk[g_pow_d2], this->tmp_G);
    element_mul(tmp_G, tmp_G, this->tmp_G_2);
    element_mul(tmp_G, tmp_G, this->tmp_G_3);
    element_mul(tmp_G, tmp_G, this->tmp_G_4);
    sks.get_sk_prime().set(sk_2, tmp_G);
   
    // sk_prime3 = g^d3 * g^(-sigma_prime)
    element_neg(this->tmp_Zn, this->tmp_Zn);
    element_pow_zn(tmp_G, mpk[g], this->tmp_Zn);
    element_mul(tmp_G, tmp_G, msk[g_pow_d3]);
    sks.get_sk_prime().set(sk_3, tmp_G);

    // sk1 = g^d * (IDi)^(a*r) * g^(b*R)
    // IDi
    this->GetID_(this->tmp_G, mpk, ID, mi, MODIFIER);
    // IDi^(a*r)
    element_add(this->tmp_Zn, this->r1, this->r2);
    element_mul(this->tmp_Zn, msk[a], this->tmp_Zn);
    element_pow_zn(this->tmp_G, this->tmp_G, this->tmp_Zn);
    // g^(b*R)
    element_mul(this->tmp_Zn, msk[b], this->R);
    element_pow_zn(this->tmp_G_2, mpk[g], this->tmp_Zn);
    // g^d = g^d1 * g^d2 * g^d3
    element_mul(this->tmp_G_3, msk[g_pow_d1], msk[g_pow_d2]);
    element_mul(this->tmp_G_3, this->tmp_G_3, msk[g_pow_d3]);
    element_mul(tmp_G_4, this->tmp_G_3, this->tmp_G);
    element_mul(tmp_G_4, tmp_G_4, this->tmp_G_2);
    sks.get_sk1().set(0, tmp_G_4);

    // sk2 = {gi-1^(a*r), gi-2^(a*r), ..., g1^(a*r)}
    sks.get_sk2().init(mi-1);
    element_add(this->tmp_Zn, this->r1, this->r2);
    element_mul(this->tmp_Zn, msk[a], this->tmp_Zn);
    for(int i=mi-1;i>=1;i--){
        element_pow_zn(tmp_G, mpk.get_gk()[i-1], this->tmp_Zn);
        sks.get_sk2().set(mi-i-1, tmp_G);
    }
}

/**
 * hash function {0,1}* -> G
 * input: m
 * output: res
 */
void ABET::Hash(element_t res, std::string m){
    HASH::hash(res, m);
}

/**
 * hash function {0,1}* -> Zq
 * input: m
 * output: res
 */
void ABET::Hash2(element_t res, element_t m){
    HASH::hash(res, m);
}
/**
 * Encrypt a message msg under a policy string.
 * input: mpk, msk,msg(r,R), policy_str, ID, oj, s1, s2
 * output: ct
 */
void ABET::Encrypt(ABET_ciphertext &ciphertext, ABET_mpk &mpk, ABET_msk &msk, element_t r, element_t R, std::string policy_str, ABET_ID &ID, int oj, element_t s1, element_t s2){
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
    element_pow_zn(tmp_H, mpk[H1], this->s1);
    ciphertext.get_ct0().set(ct0_1, tmp_H);
    // ct0_2 = H2^s2
    element_pow_zn(tmp_H, mpk[H2], this->s2);
    ciphertext.get_ct0().set(ct0_2, tmp_H);
    // ct0_3 = h^((s1+s2)/a) = (h^(1/a))^(s1+s2)
    element_add(this->tmp_Zn_2, this->s1, this->s2);
    element_pow_zn(tmp_H, mpk[h_pow_1_div_a], this->tmp_Zn_2);
    ciphertext.get_ct0().set(ct0_3, tmp_H);
    // ct0_4 = h^(b*s/a) = (h^(b/a))^(s1+s2)
    element_pow_zn(tmp_H, mpk[h_pow_b_div_a], this->tmp_Zn_2);
    ciphertext.get_ct0().set(ct0_4, tmp_H);

    // ct_ = r xor G(T1^s1 * T2^s2)
    element_pow_zn(this->tmp_GT, mpk[T1], this->s1);
    element_pow_zn(this->tmp_GT_2, mpk[T2], this->s2);
    element_mul(this->tmp_GT, this->tmp_GT, this->tmp_GT_2);
    this->Hash2(this->tmp_Zn, this->tmp_GT);
    element_mul(tmp_Zn, r, this->tmp_Zn);
    ciphertext.get_ct_().set(0, tmp_Zn);

    // ct_prime = (R || 0^(l-|R|)) xor H2(e(g,h^(d/a))^s)
    // H2(e(g,h^(d/a))^s)
    element_pairing(this->tmp_GT, mpk[g], mpk[h_pow_d_div_a]);
    element_add(this->tmp_Zn, this->s1, this->s2);
    element_pow_zn(this->tmp_GT, this->tmp_GT, this->tmp_Zn);
    this->Hash2(this->tmp_Zn, this->tmp_GT);
    element_mul(tmp_Zn, R, this->tmp_Zn);
    ciphertext.get_ct_prime().set(0, tmp_Zn);

    // ct1 = IDj^(a*s)
    // ct2 = IDj^s
    // ct3 = ct1^s
    this->GetID_(this->tmp_H, mpk, ID, oj, OWNER);
    element_add(this->tmp_Zn, this->s1, this->s2);
    element_pow_zn(tmp_H_2, this->tmp_H, this->tmp_Zn);
    ciphertext.get_ct2().set(0, tmp_H_2);
    element_mul(this->tmp_Zn_2, msk[a], this->tmp_Zn);
    element_pow_zn(tmp_H_2, this->tmp_H, this->tmp_Zn_2);
    ciphertext.get_ct1().set(0, tmp_H_2);
    element_pow_zn(tmp_H_2, tmp_H_2, this->tmp_Zn);
    ciphertext.get_ct3().set(0, tmp_H_2);

    // ct_y
    // for i = 1,2,...,rows
    ciphertext.get_ct_y().resize(rows);
    for(unsigned long int i = 0; i < rows; i++){
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
        std::string str_0jl1, str_0jl2;
        for(unsigned long int j = 0; j < cols; j++){
            str_0jl1 = "0" + std::to_string(j + 1) + "1" + "1";
            str_0jl2 = "0" + std::to_string(j + 1) + "1" + "2";
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

        PbcElements ct_y;
        ct_y.init(3);
        ct_y.set(ct_1, tmp_G_4);
    
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
        for(unsigned long int j = 0; j < cols; j++){
            str_0jl1 = "0" + std::to_string(j + 1) + "2" + "1";
            str_0jl2 = "0" + std::to_string(j + 1) + "2" + "2";
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

        ct_y.set(ct_2, tmp_G_4);

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
        for(unsigned long int j = 0; j < cols; j++){
            str_0jl1 = "0" + std::to_string(j + 1) + "3" + "1";
            str_0jl2 = "0" + std::to_string(j + 1) + "3" + "2";
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

        ct_y.set(ct_3, tmp_G_4);

        ciphertext.get_ct_y()[i] = ct_y;
    }
}
/**
 * Decrypt a ciphertext.
 * input: mpk, ciphertext, sks
 * output: res_R, res_r
 */
void ABET::Decrypt(element_t res_R, element_t res_r, ABET_mpk &mpk, ABET_ciphertext &ciphertext, ABET_sks &sks){
    // retrive R
    element_pairing(this->tmp_GT, sks.get_sk1()[0], ciphertext.get_ct0()[ct0_3]);
    element_pairing(this->tmp_GT_2, sks.get_sk0()[sk0_5], ciphertext.get_ct1()[0]);
    element_pairing(this->tmp_GT_3, sks.get_sk0()[sk0_6], ciphertext.get_ct0()[ct0_4]);
    element_mul(this->tmp_GT_2, this->tmp_GT_2, this->tmp_GT_3);
    element_div(this->tmp_GT, this->tmp_GT, this->tmp_GT_2);
    this->Hash2(this->tmp_Zn, this->tmp_GT);
    element_div(res_R, ciphertext.get_ct_prime()[0], this->tmp_Zn);

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
    unsigned long int rows = ciphertext.get_ct_y().size();
    for(unsigned long int i = 0; i < rows; i++){
        // judge whether the attribute is in the policy
        if(attr_map.find(pai[i]) == attr_map.end()){
            continue;
        }
        Element_t_vector *v = new Element_t_vector();
        for(signed long int j = 0; j < M->col(); ++j) {
            v->pushBack(M->getElement(i, j));
        }
        attributesMatrix->pushBack(v);
    }
    // get inverse matrix
    Element_t_matrix* inverse_attributesMatrix = attributesMatrix->inverse();

    unsigned long int r = inverse_attributesMatrix->row();
    unsigned long int c = inverse_attributesMatrix->col();
    printf("rows: %ld, cols: %ld\n", r, c);
    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++){
            element_printf("%B ", inverse_attributesMatrix->getElement(i, j));
        }
        printf("\n");
    }
    Element_t_vector* unit = inverse_attributesMatrix->getCoordinateAxisUnitVector();

    Element_t_vector* x = new Element_t_vector(inverse_attributesMatrix->col(), inverse_attributesMatrix->getElement(0, 0));

    signed long int type = Element_t_matrix::gaussElimination(x, inverse_attributesMatrix, unit);
    if(-1 == type) {
        throw std::runtime_error("POLICY_NOT_SATISFIED");
    }
    printf("type: %ld\n", type);
    // print x
    printf("Yi:\n");
    x->printVector();

    // retrive r
    // num
    element_t num, den;
    element_init_same_as(num, this->GT);
    element_init_same_as(den, this->GT);
    
    element_set1(this->tmp_G);
    element_set1(this->tmp_G_2);
    element_set1(this->tmp_G_3);
    int count = 0;
    for(unsigned long int i = 0; i < rows; i++){
        // judge whether the attribute is in the policy
        if(attr_map.find(pai[i]) == attr_map.end()){
            continue;
        }
        element_pow_zn(this->tmp_G_4, ciphertext.get_ct_y(i)[ct_1], x->getElement(count));
        element_mul(this->tmp_G, this->tmp_G, this->tmp_G_4);
        element_pow_zn(this->tmp_G_4, ciphertext.get_ct_y(i)[ct_2], x->getElement(count));
        element_mul(this->tmp_G_2, this->tmp_G_2, this->tmp_G_4);
        element_pow_zn(this->tmp_G_4, ciphertext.get_ct_y(i)[ct_3], x->getElement(count));
        element_mul(this->tmp_G_3, this->tmp_G_3, this->tmp_G_4);
        count++;
    }
    // ct_prime * e(tmp_G, sk0_1) * e(tmp_G_2, sk0_2) * e(tmp_G_3, sk0_3)
    element_pairing(this->tmp_GT, this->tmp_G, sks.get_sk0()[sk0_1]);
    element_pairing(this->tmp_GT_2, this->tmp_G_2, sks.get_sk0()[sk0_2]);
    element_pairing(this->tmp_GT_3, this->tmp_G_3, sks.get_sk0()[sk0_3]);

    element_mul(num, this->tmp_GT, this->tmp_GT_2);
    element_mul(num, num, this->tmp_GT_3);

    // den
    element_set1(this->tmp_G);
    element_set1(this->tmp_G_2);
    element_set1(this->tmp_G_3);
    count = 0;
    for(unsigned long int i = 0; i < rows; i++){
        // judge whether the attribute is in the policy
        if(attr_map.find(pai[i]) == attr_map.end()){
            continue;
        }
        element_pow_zn(this->tmp_G_4, sks.get_sk_y(attr_map[pai[i]])[sk_1], x->getElement(count));
        element_mul(this->tmp_G, this->tmp_G, this->tmp_G_4);
        element_pow_zn(this->tmp_G_4, sks.get_sk_y(attr_map[pai[i]])[sk_2], x->getElement(count));
        element_mul(this->tmp_G_2, this->tmp_G_2, this->tmp_G_4);
        element_pow_zn(this->tmp_G_4, sks.get_sk_y(attr_map[pai[i]])[sk_3], x->getElement(count));
        element_mul(this->tmp_G_3, this->tmp_G_3, this->tmp_G_4);
        count++;
    }
    // sk_prime_1 * tmp_G
    element_mul(this->tmp_G, sks.get_sk_prime()[sk_1], this->tmp_G);
    // sk_prime_2 * tmp_G_2
    element_mul(this->tmp_G_2, sks.get_sk_prime()[sk_2], this->tmp_G_2);
    // sk_prime_3 * tmp_G_3
    element_mul(this->tmp_G_3, sks.get_sk_prime()[sk_3], this->tmp_G_3);

    // e(tmp_G, ct01) * e(tmp_G_2, ct02) * e(tmp_G_3, ct03)
    element_pairing(this->tmp_GT, this->tmp_G, ciphertext.get_ct0()[ct0_1]);
    element_pairing(this->tmp_GT_2, this->tmp_G_2, ciphertext.get_ct0()[ct0_2]);
    element_pairing(this->tmp_GT_3, this->tmp_G_3, ciphertext.get_ct0()[ct0_3]);

    element_mul(den, this->tmp_GT, this->tmp_GT_2);
    element_mul(den, den, this->tmp_GT_3);

    // res = - num / den
    element_div(num, num, den);
    element_neg(num, num);
    this->Hash2(this->tmp_Zn, num);

    element_div(res_r, ciphertext.get_ct_()[0], this->tmp_Zn);

    element_clear(num);
    element_clear(den);
}
/**
 * get ID^
 * input: mpk, ID, mi_oj, type
 * output: ID_
 */
void ABET::GetID_(element_t ID_, ABET_mpk &mpk, ABET_ID &ID, int mi_oj, int type){
    if(type == MODIFIER){
        element_set(this->tmp_G, mpk[g]);
        for(int i=1;i<=mi_oj;i++){
            element_pow_zn(this->tmp_G_2, mpk.get_gk()[k-i], ID[i-1]);
            element_mul(this->tmp_G, this->tmp_G, this->tmp_G_2);
        }
        element_set(ID_, this->tmp_G);
    }else if(type == OWNER){
        element_set(this->tmp_H, mpk[h]);
        for(int i=1;i<=mi_oj;i++){
            element_pow_zn(this->tmp_H_2, mpk.get_hk()[k-i], ID[i-1]);
            element_mul(this->tmp_H, this->tmp_H, this->tmp_H_2);
        }
        element_set(ID_, this->tmp_H);
    }
}


ABET::~ABET(){
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