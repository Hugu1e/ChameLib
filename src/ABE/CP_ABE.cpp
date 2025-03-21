#include "ABE/CP_ABE.h"

CP_ABE::CP_ABE(int curve, bool swap): PbcScheme(curve){
    this->swap = swap;
    if(swap){
        element_init_G2(G1, pairing);
        element_init_G1(G2, pairing);
    }else{
        element_init_G1(G1, pairing);
        element_init_G2(G2, pairing);
    }
    element_init_GT(GT, pairing);
    element_init_Zr(Zn, pairing);

    initTmp();
}

void CP_ABE::init(element_t _G1, element_t _G2, element_t _GT, element_t _Zn, bool swap){
    this->swap = swap;

    element_init_same_as(G1, _G1);
    element_init_same_as(G2, _G2);
    element_init_same_as(GT, _GT);
    element_init_same_as(Zn, _Zn);

    initTmp();
}

void CP_ABE::initTmp(){
    element_s *zr_list[] = {d1, d2, d3, r1, r2, 
        b1r1a1, b1r1a2, b2r2a1, b2r2a2, r1r2a1, r1r2a2, 
        s1, s2, 
        tmp_Zn, tmp_Zn_2};
    element_s *G1_list[] = {sk_1_G, sk_2_G, sk_3_G, ct_1_G, ct_2_G, ct_3_G, tmp_G, tmp_G_2, tmp_G_3, tmp_G_4};
    element_s *G2_list[] = {sk_1_H, sk_2_H, sk_3_H, ct_1_H, ct_2_H, ct_3_H, tmp_H};
    element_s *GT_list[] = {tmp_GT, tmp_GT_2, tmp_GT_3};

    for(int i = 0;i < sizeof(zr_list)/sizeof(element_s*);i++){
        element_init_same_as(zr_list[i], Zn);
    }
    for(int i = 0;i < sizeof(G1_list)/sizeof(element_s*);i++){
        element_init_same_as(G1_list[i], G1);
    }
    for(int i = 0;i < sizeof(G2_list)/sizeof(element_s*);i++){
        element_init_same_as(G2_list[i], G2);
    }
    for(int i = 0;i < sizeof(GT_list)/sizeof(element_s*);i++){
        element_init_same_as(GT_list[i], GT);
    }
}

void CP_ABE::Pairing(element_t res, element_t a, element_t b){
    if(swap){
        element_pairing(res, b, a);
    }else{
        element_pairing(res, a, b);
    }
}

/**
 * output: mpk, msk
 */
void CP_ABE::Setup(CP_ABE_msk &msk, CP_ABE_mpk &mpk){
    element_random(tmp_G);
    msk.set(g, tmp_G);
    element_random(tmp_H);
    msk.set(h, tmp_H);
    element_random(tmp_Zn);
    msk.set(a1, tmp_Zn);
    element_random(tmp_Zn);
    msk.set(a2, tmp_Zn);
    element_random(tmp_Zn);
    msk.set(b1, tmp_Zn);
    element_random(tmp_Zn);
    msk.set(b2, tmp_Zn);
    
    element_random(this->d1);
    // g_pow_d1 = g^d1
    element_pow_zn(tmp_G, msk[g], this->d1);
    msk.set(g_pow_d1, tmp_G);
    element_random(this->d2);
    // g_pow_d2 = g^d2
    element_pow_zn(tmp_G, msk[g], this->d2);
    msk.set(g_pow_d2, tmp_G);
    element_random(this->d3);
    // g_pow_d3 = g^d3
    element_pow_zn(tmp_G, msk[g], this->d3);
    msk.set(g_pow_d3, tmp_G);

    mpk.set(h, msk[h]);
    // H1 = h^a1
    element_pow_zn(tmp_H, msk[h], msk[a1]);
    mpk.set(H1, tmp_H);
    // H2 = h^a2
    element_pow_zn(tmp_H, msk[h], msk[a2]);
    mpk.set(H2, tmp_H);
    // T1 = e(g, h)^(d1*a1+d3)
    element_mul(this->tmp_Zn, this->d1, msk[a1]);
    element_add(this->tmp_Zn_2, this->tmp_Zn, this->d3);
    Pairing(this->tmp_GT, msk[g], msk[h]);
    element_pow_zn(tmp_GT_2, this->tmp_GT, this->tmp_Zn_2);
    mpk.set(T1, tmp_GT_2);
    // T2 = e(g, h)^(d2*a2+d3)
    element_mul(this->tmp_Zn, this->d2, msk[a2]);
    element_add(this->tmp_Zn_2, this->tmp_Zn, this->d3);
    element_pow_zn(tmp_GT_2, this->tmp_GT, this->tmp_Zn_2);
    mpk.set(T2, tmp_GT_2);
}


/**
 * Generate a key for a list of attributes.
 * input: msk, mpk, attr
 * output: sks
 */
void CP_ABE::KeyGen(CP_ABE_sks &sks, CP_ABE_msk &msk, CP_ABE_mpk &mpk, std::vector<std::string> &attr_list){    
    element_random(this->r1);
    element_random(this->r2);
    // sk0 = (h^(b1r1), h^(b2r2), h^(r1+r2))
    element_mul(this->tmp_Zn, msk[b1], this->r1);
    element_pow_zn(sk_1_H, mpk[h], this->tmp_Zn);
    sks.get_sk_0().set(sk_1, sk_1_H);
    // (b1 * r1) / a1
    element_div(this->b1r1a1, this->tmp_Zn, msk[a1]);
    // (b1 * r1) / a2
    element_div(this->b1r1a2, this->tmp_Zn, msk[a2]);
    element_mul(this->tmp_Zn, msk[b2], this->r2);
    element_pow_zn(sk_2_H, mpk[h], this->tmp_Zn);
    sks.get_sk_0().set(sk_2, sk_2_H);
    // (b2 * r2) / a1
    element_div(this->b2r2a1, this->tmp_Zn, msk[a1]);
    // (b2 * r2) / a2
    element_div(this->b2r2a2, this->tmp_Zn, msk[a2]);
    element_add(this->tmp_Zn, this->r1, this->r2);
    element_pow_zn(sk_3_H, mpk[h], this->tmp_Zn);
    sks.get_sk_0().set(sk_3, sk_3_H);
    // (r1 + r2) / a1
    element_div(this->r1r2a1, this->tmp_Zn, msk[a1]);
    // (r1 + r2) / a2
    element_div(this->r1r2a2, this->tmp_Zn, msk[a2]);

    // compute sk_y
    sks.get_sk_y().resize(attr_list.size());
    for(int i = 0;i < attr_list.size();i++){
        sks.get_sk_y()[i].init(3);
        sks.get_attr2id()[attr_list[i]] = i;
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
        // H(y31)^r1r2a1
        std::string y31 = attr_list[i] + "3" + "1";
        this->Hash(this->tmp_G_3, y31);
        element_pow_zn(this->tmp_G_3, this->tmp_G_3, this->r1r2a1);
        // g^(sigma_y / a1)
        element_div(this->tmp_Zn_2, this->tmp_Zn, msk[a1]);
        element_pow_zn(this->tmp_G_4, msk[g], this->tmp_Zn_2);
        // sky1
        element_mul(sk_1_G, this->tmp_G, this->tmp_G_2);
        element_mul(sk_1_G, sk_1_G, this->tmp_G_3);
        element_mul(sk_1_G, sk_1_G, this->tmp_G_4);

        sks.get_sk_y()[i].set(sk_1, sk_1_G);

        // t = 2
        // H(y12)^b1r1a2
        std::string y12 = attr_list[i] + "1" + "2";
        this->Hash(this->tmp_G, y12);
        element_pow_zn(this->tmp_G, this->tmp_G, this->b1r1a2);
        // H(y22)^b2r2a2
        std::string y22 = attr_list[i] + "2" + "2";
        this->Hash(this->tmp_G_2, y22);
        element_pow_zn(this->tmp_G_2, this->tmp_G_2, this->b2r2a2);
        // H(y32)^r1r2a2
        std::string y32 = attr_list[i] + "3" + "2";
        this->Hash(this->tmp_G_3, y32);
        element_pow_zn(this->tmp_G_3, this->tmp_G_3, this->r1r2a2);
        // g^(sigma_y / a2)
        element_div(this->tmp_Zn_2, this->tmp_Zn, msk[a2]);
        element_pow_zn(this->tmp_G_4, msk[g], this->tmp_Zn_2);
        // sky2
        element_mul(sk_2_G, this->tmp_G, this->tmp_G_2);
        element_mul(sk_2_G, sk_2_G, this->tmp_G_3);
        element_mul(sk_2_G, sk_2_G, this->tmp_G_4);
        sks.get_sk_y()[i].set(sk_2, sk_2_G);

        // sky3 = g^(-sigma_y)
        element_neg(this->tmp_Zn, this->tmp_Zn);
        element_pow_zn(sk_3_G, msk[g], this->tmp_Zn);
        sks.get_sk_y()[i].set(sk_3, sk_3_G);
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
    element_div(this->tmp_Zn_2, this->tmp_Zn, msk[a1]);
    element_pow_zn(this->tmp_G_4, msk[g], this->tmp_Zn_2);
    // sk_prime1
    element_mul(sk_1_G, msk[g_pow_d1], this->tmp_G);
    element_mul(sk_1_G, sk_1_G, this->tmp_G_2);
    element_mul(sk_1_G, sk_1_G, this->tmp_G_3);
    element_mul(sk_1_G, sk_1_G, this->tmp_G_4);
    sks.get_sk_prime().set(sk_1, sk_1_G);

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
    element_div(this->tmp_Zn_2, this->tmp_Zn, msk[a2]);
    element_pow_zn(this->tmp_G_4, msk[g], this->tmp_Zn_2);
    // sk_prime2
    element_mul(sk_2_G, msk[g_pow_d2], this->tmp_G);
    element_mul(sk_2_G, sk_2_G, this->tmp_G_2);
    element_mul(sk_2_G, sk_2_G, this->tmp_G_3);
    element_mul(sk_2_G, sk_2_G, this->tmp_G_4);
    sks.get_sk_prime().set(sk_2, sk_2_G);
   
    // sk_prime3 = g^d3 * g ^ (-sigma_prime)
    element_neg(this->tmp_Zn, this->tmp_Zn);
    element_pow_zn(sk_3_G, msk[g], this->tmp_Zn);
    element_mul(sk_3_G, sk_3_G, msk[g_pow_d3]);
    sks.get_sk_prime().set(sk_3, sk_3_G);
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
 * input: mpk, msg, policy_str, s1, s2
 * output: ct
 */
void CP_ABE::Encrypt(CP_ABE_ciphertext &ciphertext, CP_ABE_mpk &mpk, element_t msg, Element_t_matrix *MSP, element_t s1, element_t s2){
    unsigned long int rows = MSP->row();
    unsigned long int cols = MSP->col();

    // ct0
    // ct0_1 = H1^s1
    element_pow_zn(ct_1_H, mpk[H1], s1);
    ciphertext.get_ct_0().set(ct_1, ct_1_H);
    // ct0_2 = H2^s2
    element_pow_zn(ct_2_H, mpk[H2], s2);
    ciphertext.get_ct_0().set(ct_2, ct_2_H);
    // ct0_3 = h^(s1+s2)
    element_add(this->tmp_Zn_2, s1, s2);
    element_pow_zn(ct_3_H, mpk[h], this->tmp_Zn_2);
    ciphertext.get_ct_0().set(ct_3, ct_3_H);

    // ct_prime = T1^s1 * T2^s2 * msg
    element_pow_zn(this->tmp_GT, mpk[T1], s1);
    element_pow_zn(this->tmp_GT_2, mpk[T2], s2);
    element_mul(this->tmp_GT_3, this->tmp_GT, this->tmp_GT_2);
    element_mul(tmp_GT_3, this->tmp_GT_3, msg);
    ciphertext.get_ct_prime().set(0, tmp_GT_3);

    // ct_y
    // for i = 1, 2, ..., rows
    ciphertext.get_ct_y().resize(rows);
    for(unsigned long int i = 0; i < rows; i++){
        ciphertext.get_ct_y()[i].init(3);
        std::string attr = MSP->getName(i);

        // l = 1
        std::string attr_l_1 = attr + "1" + "1";
        // H(attr_l_1)^s1
        this->Hash(this->tmp_G, attr_l_1);
        element_pow_zn(this->tmp_G, this->tmp_G, s1);
        std::string attr_l_2 = attr + "1" + "2";
        // H(attr_l_2)^s2
        this->Hash(this->tmp_G_2, attr_l_2);
        element_pow_zn(this->tmp_G_2, this->tmp_G_2, s2);
        element_mul(ct_1_G, this->tmp_G, this->tmp_G_2);
        // for j = 1, 2, ..., cols
        std::string str_0jl1, str_0jl2;
        for(unsigned long int j = 0; j < cols; j++){
            str_0jl1 = "0" + std::to_string(j + 1) + "1" + "1";
            str_0jl2 = "0" + std::to_string(j + 1) + "1" + "2";
            // H(0jl1)^s1
            this->Hash(this->tmp_G, str_0jl1);
            element_pow_zn(this->tmp_G, this->tmp_G, s1);
            // H(0jl2)^s2
            this->Hash(this->tmp_G_2, str_0jl2);
            element_pow_zn(this->tmp_G_2, this->tmp_G_2, s2);
            // H(0jl1)^s1 * H(0jl2)^s2
            element_mul(this->tmp_G_3, this->tmp_G, this->tmp_G_2);
            // (H(0jl1)^s1 * H(0jl2)^s2)^M[i][j]
            element_pow_zn(this->tmp_G_4, this->tmp_G_3, MSP->getElement(i, j));
            element_mul(ct_1_G, ct_1_G, this->tmp_G_4);
        }

        ciphertext.get_ct_y()[i].set(ct_1, ct_1_G);

        // l = 2
        attr_l_1 = attr + "2" + "1";
        // H(attr_l_1)^s1
        this->Hash(this->tmp_G, attr_l_1);
        element_pow_zn(this->tmp_G, this->tmp_G, s1);
        attr_l_2 = attr + "2" + "2";
        // H(attr_l_2)^s2
        this->Hash(this->tmp_G_2, attr_l_2);
        element_pow_zn(this->tmp_G_2, this->tmp_G_2, s2);
        element_mul(ct_2_G, this->tmp_G, this->tmp_G_2);
        // for j = 1, 2, ..., cols
        for(unsigned long int j = 0; j < cols; j++){
            str_0jl1 = "0" + std::to_string(j + 1) + "2" + "1";
            str_0jl2 = "0" + std::to_string(j + 1) + "2" + "2";
            // H(0jl1)^s1
            this->Hash(this->tmp_G, str_0jl1);
            element_pow_zn(this->tmp_G, this->tmp_G, s1);
            // H(0jl2)^s2
            this->Hash(this->tmp_G_2, str_0jl2);
            element_pow_zn(this->tmp_G_2, this->tmp_G_2, s2);
            // H(0jl1)^s1 * H(0jl2)^s2
            element_mul(this->tmp_G_3, this->tmp_G, this->tmp_G_2);
            // (H(0jl1)^s1 * H(0jl2)^s2)^M[i][j]
            element_pow_zn(this->tmp_G_4, this->tmp_G_3, MSP->getElement(i, j));
            element_mul(ct_2_G, ct_2_G, this->tmp_G_4);
        }
        ciphertext.get_ct_y()[i].set(ct_2, ct_2_G);

        // l = 3
        attr_l_1 = attr + "3" + "1";
        // H(attr_l_1)^s1
        this->Hash(this->tmp_G, attr_l_1);
        element_pow_zn(this->tmp_G, this->tmp_G, s1);
        attr_l_2 = attr + "3" + "2";
        // H(attr_l_2)^s2
        this->Hash(this->tmp_G_2, attr_l_2);
        element_pow_zn(this->tmp_G_2, this->tmp_G_2, s2);
        element_mul(ct_3_G, this->tmp_G, this->tmp_G_2);
        // for j = 1, 2, ..., cols
        for(unsigned long int j = 0; j < cols; j++){
            str_0jl1 = "0" + std::to_string(j + 1) + "3" + "1";
            str_0jl2 = "0" + std::to_string(j + 1) + "3" + "2";
            // H(0jl1)^s1
            this->Hash(this->tmp_G, str_0jl1);
            element_pow_zn(this->tmp_G, this->tmp_G, s1);
            // H(0jl2)^s2
            this->Hash(this->tmp_G_2, str_0jl2);
            element_pow_zn(this->tmp_G_2, this->tmp_G_2, s2);
            // H(0jl1)^s1 * H(0jl2)^s2
            element_mul(this->tmp_G_3, this->tmp_G, this->tmp_G_2);
            // (H(0jl1)^s1 * H(0jl2)^s2)^M[i][j]
            element_pow_zn(this->tmp_G_4, this->tmp_G_3, MSP->getElement(i, j));
            element_mul(ct_3_G, ct_3_G, this->tmp_G_4);
        }
        ciphertext.get_ct_y()[i].set(ct_3, ct_3_G);
    }
}
/**
 * Decrypt a ciphertext.
 * input: mpk, ciphertext, sks
 * output: res
 */
void CP_ABE::Decrypt(element_t res, CP_ABE_ciphertext &ciphertext, Element_t_matrix *MSP, CP_ABE_mpk &mpk, CP_ABE_sks &sks){
    // compute Yi
    // get matrix with attributes
    Element_t_matrix* attributesMatrix = new Element_t_matrix();
    unsigned long int rows = ciphertext.get_ct_y().size();
    for(unsigned long int i=0; i<rows;i++){
        // judge whether the attribute is in the policy
        if(sks.get_attr2id().find(MSP->getName(i)) == sks.get_attr2id().end()){
            continue;
        }
        Element_t_vector *v = new Element_t_vector();
        for (signed long int j = 0; j < MSP->col(); ++j) {
            v->pushBack(MSP->getElement(i, j));
        }
        attributesMatrix->pushBack(v);
    }
    // get inverse matrix
    Element_t_matrix* inverse_attributesMatrix = attributesMatrix->inverse();

    unsigned long int r = inverse_attributesMatrix->row();
    unsigned long int c = inverse_attributesMatrix->col();

    Element_t_vector* unit = inverse_attributesMatrix->getCoordinateAxisUnitVector();

    Element_t_vector* x= new Element_t_vector(inverse_attributesMatrix->col(), inverse_attributesMatrix->getElement(0, 0));

    signed long int type = Element_t_matrix::gaussElimination(x, inverse_attributesMatrix, unit);
    if (-1 == type) {
        throw std::runtime_error("POLICY_NOT_SATISFIED");
    }

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
        if(sks.get_attr2id().find(MSP->getName(i)) == sks.get_attr2id().end()){
            continue;
        }
        
        element_pow_zn(this->tmp_G_4, ciphertext.get_ct_y()[i][ct_1], x->getElement(count));
        element_mul(this->tmp_G, this->tmp_G, this->tmp_G_4);
        element_pow_zn(this->tmp_G_4, ciphertext.get_ct_y()[i][ct_2], x->getElement(count));
        element_mul(this->tmp_G_2, this->tmp_G_2, this->tmp_G_4);
        element_pow_zn(this->tmp_G_4, ciphertext.get_ct_y()[i][ct_3], x->getElement(count));
        element_mul(this->tmp_G_3, this->tmp_G_3, this->tmp_G_4);
        count++;
    }
    // ct_prime * e(tmp_G, sk0_1) * e(tmp_G_2, sk0_2) * e(tmp_G_3, sk0_3)
    Pairing(this->tmp_GT, this->tmp_G, sks.get_sk_0()[sk_1]);
    Pairing(this->tmp_GT_2, this->tmp_G_2, sks.get_sk_0()[sk_2]);
    Pairing(this->tmp_GT_3, this->tmp_G_3, sks.get_sk_0()[sk_3]);

    element_mul(num, ciphertext.get_ct_prime()[0], this->tmp_GT);
    element_mul(num, num, this->tmp_GT_2);
    element_mul(num, num, this->tmp_GT_3);

    // den
    element_set1(this->tmp_G);
    element_set1(this->tmp_G_2);
    element_set1(this->tmp_G_3);
    count = 0;
    for(unsigned long int i=0; i<rows;i++){
        // judge whether the attribute is in the policy
        if(sks.get_attr2id().find(MSP->getName(i)) == sks.get_attr2id().end()){
            continue;
        }
        
        element_pow_zn(this->tmp_G_4, sks.get_sk_y()[sks.get_attr2id()[MSP->getName(i)]][sk_1], x->getElement(count));
        element_mul(this->tmp_G, this->tmp_G, this->tmp_G_4);
        element_pow_zn(this->tmp_G_4, sks.get_sk_y()[sks.get_attr2id()[MSP->getName(i)]][sk_2], x->getElement(count));
        element_mul(this->tmp_G_2, this->tmp_G_2, this->tmp_G_4);
        element_pow_zn(this->tmp_G_4, sks.get_sk_y()[sks.get_attr2id()[MSP->getName(i)]][sk_3], x->getElement(count));
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
    Pairing(this->tmp_GT, this->tmp_G, ciphertext.get_ct_0()[ct_1]);
    Pairing(this->tmp_GT_2, this->tmp_G_2, ciphertext.get_ct_0()[ct_2]);
    Pairing(this->tmp_GT_3, this->tmp_G_3, ciphertext.get_ct_0()[ct_3]);

    element_mul(den, this->tmp_GT, this->tmp_GT_2);
    element_mul(den, den, this->tmp_GT_3);

    // res = num / den
    element_div(res, num, den);

    element_clear(num);
    element_clear(den);
}


CP_ABE::~CP_ABE(){
    element_s *clear_list[] = {d1, d2, d3, r1, r2, 
        b1r1a1, b1r1a2, b2r2a1, b2r2a2, r1r2a1, r1r2a2, 
        s1, s2, 
        sk_1_G, sk_2_G, sk_3_G, sk_1_H, sk_2_H, sk_3_H, 
        ct_1_G, ct_2_G, ct_3_G, ct_1_H, ct_2_H, ct_3_H, 
        tmp_G, tmp_G_2, tmp_G_3, tmp_G_4, tmp_H, tmp_GT, tmp_GT_2, tmp_GT_3, tmp_Zn, tmp_Zn_2,
        G1, G2, GT, Zn};

    for(int i = 0;i < sizeof(clear_list)/sizeof(element_s*);i++){
        element_clear(clear_list[i]);
    }
}