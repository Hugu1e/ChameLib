#include "ABE/ABET.h"

ABET::ABET(int curve, bool swap):PbcScheme(curve){
    this->swap = swap;
    if(swap){
        element_init_G1(G2, pairing);
        element_init_G2(G1, pairing);
    }else{
        element_init_G1(G1, pairing);
        element_init_G2(G2, pairing);
    }
    element_init_GT(GT, pairing);
    element_init_Zr(Zn, pairing);

    initTmp();
}

void ABET::init(element_t _G1, element_t _G2, element_t _GT, element_t _Zn, bool swap, bool shared_pairing){
    this->swap = swap;
    PbcScheme::init(_G1, _G2, _GT, _Zn, shared_pairing);

    initTmp();
}

void ABET::initTmp(){
    element_s *zr_list[] = {d1, d2, d3, r1, r2, R, 
        b1r1a1, b1r1a2, b2r2a1, b2r2a2, r1r2a1a, r1r2a2a, aa1, aa2,
        s1, s2, z_1, z_2, z, alpha_a_1, alpha_a_2, 
        tmp_Zn, tmp_Zn_2};
    element_s *G1_list[] = {ID_i_1, tmp_G, tmp_G_2, tmp_G_3, tmp_G_4};
    element_s *G2_list[] = {tmp_H, tmp_H_2};
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

void ABET::Pairing(element_t res, element_t a, element_t b){
    if(swap){
        element_pairing(res, b, a);
    }else{
        element_pairing(res, a, b);
    }
}

/**
 * @brief 
 * 
 * @param  msk[out]  
 * @param  mpk[out]  
 * @param  k[in]     
 * 
 */
void ABET::Setup(ABET_msk &msk, ABET_mpk &mpk, int k){
    msk.get_zk().init(k);
    mpk.get_gk().init(k);
    mpk.get_gk_pow_a().init(k);
    mpk.get_hk().init(k);

    element_random(tmp_Zn);
    msk.get_msk().set(a1, tmp_Zn);
    element_random(tmp_Zn);
    msk.get_msk().set(a2, tmp_Zn);
    element_random(tmp_Zn);
    msk.get_msk().set(b1, tmp_Zn);
    element_random(tmp_Zn);
    msk.get_msk().set(b2, tmp_Zn);
    element_random(tmp_Zn);
    msk.get_msk().set(a, tmp_Zn);
    element_random(tmp_Zn);
    msk.get_msk().set(b, tmp_Zn);

    element_random(tmp_G);
    mpk.get_mpk().set(g, tmp_G);
    element_random(tmp_H);
    mpk.get_mpk().set(h, tmp_H);

    element_random(this->d1);
    // g_pow_d1 = g^d1
    element_pow_zn(tmp_G, mpk.get_mpk()[g], this->d1);
    msk.get_msk().set(g_pow_d1, tmp_G);
    element_random(this->d2);
    // g_pow_d2 = g^d2
    element_pow_zn(tmp_G, mpk.get_mpk()[g], this->d2);
    msk.get_msk().set(g_pow_d2, tmp_G);
    element_random(this->d3);
    // g_pow_d3 = g^d3
    element_pow_zn(tmp_G, mpk.get_mpk()[g], this->d3);
    msk.get_msk().set(g_pow_d3, tmp_G);

    // z1,z2,...zk
    for(int i=1;i <= k;i++){
        element_random(tmp_Zn);
        msk.get_zk().set(i-1, tmp_Zn);
    }

    // H1 = h^a1
    element_pow_zn(tmp_H, mpk.get_mpk()[h], msk.get_msk()[a1]);
    mpk.get_mpk().set(H1, tmp_H);
    // H2 = h^a2
    element_pow_zn(tmp_H, mpk.get_mpk()[h], msk.get_msk()[a2]);
    mpk.get_mpk().set(H2, tmp_H);
    // T1 = e(g, h)^(d1*a1+d3/a)
    element_mul(this->tmp_Zn, this->d1, msk.get_msk()[a1]);
    element_div(this->tmp_Zn_2, this->d3, msk.get_msk()[a]);
    element_add(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    Pairing(this->tmp_GT, mpk.get_mpk()[g], mpk.get_mpk()[h]);
    element_pow_zn(tmp_GT_2, this->tmp_GT, this->tmp_Zn);
    mpk.get_mpk().set(T1, tmp_GT_2);
    // T2 = e(g, h)^(d2*a2+d3/a)
    element_mul(this->tmp_Zn, this->d2, msk.get_msk()[a2]);
    element_add(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    element_pow_zn(tmp_GT_2, this->tmp_GT, this->tmp_Zn);
    mpk.get_mpk().set(T2, tmp_GT_2);

    // gk, gk_pow_a, hk
    for(int i = 1;i <= k;i++){
        // gk = g^zk
        element_pow_zn(tmp_G, mpk.get_mpk()[g], msk.get_zk()[i-1]);
        mpk.get_gk().set(i-1, tmp_G);
        // gk_pow_a = gk^a
        element_pow_zn(tmp_G_2, tmp_G, msk.get_msk()[a]);
        mpk.get_gk_pow_a().set(i-1, tmp_G_2);
        // hk = h^zk
        element_pow_zn(tmp_H, mpk.get_mpk()[h], msk.get_zk()[i-1]);
        mpk.get_hk().set(i-1, tmp_H);
    }
    // g^a
    element_pow_zn(tmp_G, mpk.get_mpk()[g], msk.get_msk()[a]);
    mpk.get_mpk().set(g_pow_a, tmp_G);
    // h^(d/a)
    // d = d1 + d2 + d3
    element_add(this->tmp_Zn, this->d1, this->d2);
    element_add(this->tmp_Zn, this->tmp_Zn, this->d3);
    element_div(this->tmp_Zn, this->tmp_Zn, msk.get_msk()[a]);
    element_pow_zn(tmp_H, mpk.get_mpk()[h], this->tmp_Zn);
    mpk.get_mpk().set(h_pow_d_div_a, tmp_H);
    // h^(1/a)
    element_invert(this->tmp_Zn, msk.get_msk()[a]);
    element_pow_zn(tmp_H, mpk.get_mpk()[h], this->tmp_Zn);
    mpk.get_mpk().set(h_pow_1_div_a, tmp_H);
    // h^(b/a)
    element_div(this->tmp_Zn, msk.get_msk()[b], msk.get_msk()[a]);
    element_pow_zn(tmp_H, mpk.get_mpk()[h], this->tmp_Zn);
    mpk.get_mpk().set(h_pow_b_div_a, tmp_H);
}

/**
 * @brief Generate a key for a list of attributes.
 * 
 * @param  sks[out]        
 * @param  msk[in]        
 * @param  mpk[in]        
 * @param  attr_list[in]  
 * @param  ID[in]         
 * @param  mi[in]          length of modifierID
 * 
 */
void ABET::KeyGen(ABET_sks &sks, ABET_msk &msk, ABET_mpk &mpk, std::vector<std::string> &attr_list, ABET_ID &ID, int mi){  
    element_random(this->r1);
    element_random(this->r2);
    element_random(this->R);
    // sk0 = (h^(b1r1), h^(b2r2), h^(r1+r2)/a, g^(1/a), g^(r/a), g^R) 
    element_mul(this->tmp_Zn, msk.get_msk()[b1], this->r1);
    element_pow_zn(tmp_H, mpk.get_mpk()[h], this->tmp_Zn);
    sks.get_sk0().set(sk0_1, tmp_H);
    // (b1 * r1) / a1
    element_div(this->b1r1a1, this->tmp_Zn, msk.get_msk()[a1]);
    // (b1 * r1) / a2
    element_div(this->b1r1a2, this->tmp_Zn, msk.get_msk()[a2]);
    element_mul(this->tmp_Zn, msk.get_msk()[b2], this->r2);
    element_pow_zn(tmp_H, mpk.get_mpk()[h], this->tmp_Zn);
    sks.get_sk0().set(sk0_2, tmp_H);
    // (b2 * r2) / a1
    element_div(this->b2r2a1, this->tmp_Zn, msk.get_msk()[a1]);
    // (b2 * r2) / a2
    element_div(this->b2r2a2, this->tmp_Zn, msk.get_msk()[a2]);
    element_add(this->tmp_Zn, this->r1, this->r2);
    element_div(this->tmp_Zn_2, this->tmp_Zn, msk.get_msk()[a]);
    element_pow_zn(tmp_H, mpk.get_mpk()[h], this->tmp_Zn_2);
    sks.get_sk0().set(sk0_3, tmp_H);
    // (r1 + r2) / (a1*a)
    element_div(this->r1r2a1a, this->tmp_Zn_2, msk.get_msk()[a1]);
    // (r1 + r2) / (a2*a)
    element_div(this->r1r2a2a, this->tmp_Zn_2, msk.get_msk()[a2]);
    // g^(r/a)
    element_div(this->tmp_Zn, this->tmp_Zn, msk.get_msk()[a]);
    element_pow_zn(tmp_G, mpk.get_mpk()[g], this->tmp_Zn);
    sks.get_sk0().set(sk0_5, tmp_G);
    // g^(1/a)
    element_invert(this->tmp_Zn, msk.get_msk()[a]);
    element_pow_zn(tmp_G, mpk.get_mpk()[g], this->tmp_Zn);
    sks.get_sk0().set(sk0_4, tmp_G);
    // g^R
    element_pow_zn(tmp_G, mpk.get_mpk()[g], this->R);
    sks.get_sk0().set(sk0_6, tmp_G);
    // aa1 = a * a1
    element_mul(aa1, msk.get_msk()[a], msk.get_msk()[a1]);
    // aa2 = a * a2
    element_mul(aa2, msk.get_msk()[a], msk.get_msk()[a2]);

    // compute sk_y
    sks.get_sk_y().resize(attr_list.size());
    for (int i = 0; i < attr_list.size(); i++) {
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
        // H(y31)^(r1r2a1/a)
        std::string y31 = attr_list[i] + "3" + "1";
        this->Hash(this->tmp_G_3, y31);
        element_pow_zn(this->tmp_G_3, this->tmp_G_3, r1r2a1a);
        // g^(sigma_y / (a*a1))
        element_div(this->tmp_Zn_2, this->tmp_Zn, aa1);
        element_pow_zn(this->tmp_G_4, mpk.get_mpk()[g], this->tmp_Zn_2);
        // sky1
        element_mul(tmp_G, this->tmp_G, this->tmp_G_2);
        element_mul(tmp_G, tmp_G, this->tmp_G_3);
        element_mul(tmp_G, tmp_G, this->tmp_G_4);

        sks.get_sk_y()[i].set(sk_1, tmp_G);

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
        element_pow_zn(this->tmp_G_3, this->tmp_G_3, r1r2a2a);
        // g^(sigma_y / (a*a2))
        element_div(this->tmp_Zn_2, this->tmp_Zn, aa2);
        element_pow_zn(this->tmp_G_4, mpk.get_mpk()[g], this->tmp_Zn_2);
        // sky2
        element_mul(tmp_G, this->tmp_G, this->tmp_G_2);
        element_mul(tmp_G, tmp_G, this->tmp_G_3);
        element_mul(tmp_G, tmp_G, this->tmp_G_4);

        sks.get_sk_y()[i].set(sk_2, tmp_G);

        // sky3 = g^(-sigma_y)
        element_neg(this->tmp_Zn, this->tmp_Zn);
        element_pow_zn(tmp_G, mpk.get_mpk()[g], this->tmp_Zn);

        sks.get_sk_y()[i].set(sk_3, tmp_G);
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
    element_pow_zn(this->tmp_G_3, this->tmp_G_3, r1r2a1a);
    // g^(sigma_prime / (a*a1))
    element_div(this->tmp_Zn_2, this->tmp_Zn, this->aa1);
    element_pow_zn(this->tmp_G_4, mpk.get_mpk()[g], this->tmp_Zn_2);
    // sk_prime1
    element_mul(tmp_G, msk.get_msk()[g_pow_d1], this->tmp_G);
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
    element_pow_zn(this->tmp_G_3, this->tmp_G_3, r1r2a2a);
    // g^(sigma_prime / (a*a2))
    element_div(this->tmp_Zn_2, this->tmp_Zn, this->aa2);
    element_pow_zn(this->tmp_G_4, mpk.get_mpk()[g], this->tmp_Zn_2);
    // sk_prime2
    element_mul(tmp_G, msk.get_msk()[g_pow_d2], this->tmp_G);
    element_mul(tmp_G, tmp_G, this->tmp_G_2);
    element_mul(tmp_G, tmp_G, this->tmp_G_3);
    element_mul(tmp_G, tmp_G, this->tmp_G_4);
    sks.get_sk_prime().set(sk_2, tmp_G);
   
    // sk_prime3 = g^d3 * g^(-sigma_prime)
    element_neg(this->tmp_Zn, this->tmp_Zn);
    element_pow_zn(tmp_G, mpk.get_mpk()[g], this->tmp_Zn);
    element_mul(tmp_G, tmp_G, msk.get_msk()[g_pow_d3]);
    sks.get_sk_prime().set(sk_3, tmp_G);

    // sk1 = g^d * (IDi)^(a*r) * g^(b*R)
    // IDi
    this->GetID_(this->tmp_G, mpk, ID, mi, MODIFIER);
    // IDi^(a*r)
    element_add(this->tmp_Zn, this->r1, this->r2);
    element_mul(this->tmp_Zn, msk.get_msk()[a], this->tmp_Zn);
    element_pow_zn(this->tmp_G, this->tmp_G, this->tmp_Zn);
    // g^(b*R)
    element_mul(this->tmp_Zn, msk.get_msk()[b], this->R);
    element_pow_zn(this->tmp_G_2, mpk.get_mpk()[g], this->tmp_Zn);
    // g^d = g^d1 * g^d2 * g^d3
    element_mul(this->tmp_G_3, msk.get_msk()[g_pow_d1], msk.get_msk()[g_pow_d2]);
    element_mul(this->tmp_G_3, this->tmp_G_3, msk.get_msk()[g_pow_d3]);
    element_mul(tmp_G_4, this->tmp_G_3, this->tmp_G);
    element_mul(tmp_G_4, tmp_G_4, this->tmp_G_2);
    sks.get_sk1().set(0, tmp_G_4);

    // sk2 = {gi-1^(a*r), gi-2^(a*r), ..., g1^(a*r)}
    int k = mpk.get_gk().getSize();
    int lenOfSk2 = k - mi;
    sks.get_sk2().init(lenOfSk2);
    element_add(this->tmp_Zn, this->r1, this->r2);
    element_mul(this->tmp_Zn, msk.get_msk()[a], this->tmp_Zn);
    for(int i=0; i<lenOfSk2; i++){
        element_pow_zn(tmp_G, mpk.get_gk()[lenOfSk2-i-1], this->tmp_Zn);
        sks.get_sk2().set(i, tmp_G);
    }
}

void ABET::Hash(element_t res, std::string m){
    HASH::hash(res, m);
}

void ABET::Hash2(element_t res, element_t m){
    HASH::hash(res, m);
}

/**
 * @brief Encrypt a message msg under a policy string.
 * 
 * @param  ciphertext[out]  
 * @param  mpk[in]         
 * @param  msk[in]         
 * @param  r[in]            
 * @param  R[in]           
 * @param  size_R[in]       
 * @param  MSP[in]         
 * @param  ID[in]          
 * @param  oj[in]           
 * @param  s1[in]           
 * @param  s2[in]           
 * 
 */
void ABET::Encrypt(ABET_ciphertext &ciphertext, ABET_mpk &mpk, ABET_msk &msk, element_t r, unsigned char *R, int size_R, Element_t_matrix *MSP, ABET_ID &ID, int oj, element_t s1, element_t s2){
    unsigned long int rows = MSP->row();
    unsigned long int cols = MSP->col();

    // s1,s2
    element_set(this->s1, s1);
    element_set(this->s2, s2);

    // ct0
    // ct0_1 = H1^s1
    element_pow_zn(tmp_H, mpk.get_mpk()[H1], this->s1);
    ciphertext.get_ct0().set(ct0_1, tmp_H);
    // ct0_2 = H2^s2
    element_pow_zn(tmp_H, mpk.get_mpk()[H2], this->s2);
    ciphertext.get_ct0().set(ct0_2, tmp_H);
    // ct0_3 = h^((s1+s2)/a) = (h^(1/a))^(s1+s2)
    element_add(this->tmp_Zn_2, this->s1, this->s2);
    element_pow_zn(tmp_H, mpk.get_mpk()[h_pow_1_div_a], this->tmp_Zn_2);
    ciphertext.get_ct0().set(ct0_3, tmp_H);
    // ct0_4 = h^(b*s/a) = (h^(b/a))^(s1+s2)
    element_pow_zn(tmp_H, mpk.get_mpk()[h_pow_b_div_a], this->tmp_Zn_2);
    ciphertext.get_ct0().set(ct0_4, tmp_H);

    // ct_ = r xor (T1^s1 * T2^s2)
    element_pow_zn(this->tmp_GT, mpk.get_mpk()[T1], this->s1);
    element_pow_zn(this->tmp_GT_2, mpk.get_mpk()[T2], this->s2);
    element_mul(this->tmp_GT, this->tmp_GT, this->tmp_GT_2);
    this->Hash2(this->tmp_Zn, this->tmp_GT);
    element_mul(tmp_Zn, r, this->tmp_Zn);
    ciphertext.get_ct_p().set(0, tmp_Zn);
    // (T1^s1 * T2^s2) to bytes
    unsigned char tmp_GT_bytes[element_length_in_bytes(this->tmp_GT)];
    element_to_bytes(tmp_GT_bytes, this->tmp_GT);
    // r to bytes
    unsigned char r_bytes[element_length_in_bytes(r)];
    element_to_bytes(r_bytes, r);
    // xor
    for(int i = 0;i < element_length_in_bytes(r);++i) tmp_GT_bytes[i] ^= r_bytes[i];
    ciphertext.set_ct_(tmp_GT_bytes, element_length_in_bytes(this->tmp_GT));

    // ct_prime = (R || 0^(l-|R|)) xor H2(e(g,h^(d/a))^s)
    // H2(e(g,h^(d/a))^s)
    Pairing(this->tmp_GT, mpk.get_mpk()[g], mpk.get_mpk()[h_pow_d_div_a]);
    element_add(this->tmp_Zn, this->s1, this->s2);
    element_pow_zn(this->tmp_GT, this->tmp_GT, this->tmp_Zn);
    this->Hash2(this->tmp_Zn, this->tmp_GT);
    // tmp_Zn to bytes
    unsigned char tmp_Zn_bytes[element_length_in_bytes(this->tmp_Zn)];
    element_to_bytes(tmp_Zn_bytes, this->tmp_Zn);
    for(int i = 0; i < size_R; ++i) tmp_Zn_bytes[i] ^= R[i];
    ciphertext.set_ct_prime(tmp_Zn_bytes, element_length_in_bytes(this->tmp_Zn));

    // ct1 = IDj^(a*s)
    // ct2 = IDj^s
    // ct3 = ct1^s
    this->GetID_(this->tmp_H, mpk, ID, oj, OWNER);
    element_add(this->tmp_Zn, this->s1, this->s2);
    element_pow_zn(tmp_H_2, this->tmp_H, this->tmp_Zn);
    ciphertext.get_ct2().set(0, tmp_H_2);
    element_mul(this->tmp_Zn_2, msk.get_msk()[a], this->tmp_Zn);
    element_pow_zn(tmp_H_2, this->tmp_H, this->tmp_Zn_2);
    ciphertext.get_ct1().set(0, tmp_H_2);
    element_pow_zn(tmp_H_2, tmp_H_2, this->tmp_Zn);
    ciphertext.get_ct3().set(0, tmp_H_2);

    // ct_y
    // for i = 1,2,...,rows
    ciphertext.get_ct_y().resize(rows);
    for(unsigned long int i = 0; i < rows; i++){
        ciphertext.get_ct_y()[i].init(3);
        std::string attr = MSP->getName(i);

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
            element_pow_zn(this->tmp_G_3, this->tmp_G_3, MSP->getElement(i, j));
            element_mul(tmp_G_4, tmp_G_4, this->tmp_G_3);
        }

        ciphertext.get_ct_y()[i].set(ct_1, tmp_G_4);
    
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
            element_pow_zn(this->tmp_G_3, this->tmp_G_3, MSP->getElement(i, j));
            element_mul(tmp_G_4, tmp_G_4, this->tmp_G_3);
        }

        ciphertext.get_ct_y()[i].set(ct_2, tmp_G_4);

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
            element_pow_zn(this->tmp_G_3, this->tmp_G_3, MSP->getElement(i, j));
            element_mul(tmp_G_4, tmp_G_4, this->tmp_G_3);
        }

        ciphertext.get_ct_y()[i].set(ct_3, tmp_G_4);
    }
    ciphertext.set_ownerId_length(oj);
}

bool ABET::SkDelegate(ABET_sks &sk, ABET_mpk &mpk, ABET_msk &msk, element_t ID_i_1, element_t I_i_1) {
    if (sk.get_sk2().getSize() == 0) return false;

    element_random(z_1);
    element_random(z_2);
    element_add(z, z_1, z_2);

    element_mul(tmp_Zn, msk.get_msk()[b1], z_1);
    element_pow_zn(tmp_H, mpk.get_mpk()[h], tmp_Zn);
    element_mul(sk.get_sk0()[sk0_1], sk.get_sk0()[sk0_1], tmp_H);

    element_mul(tmp_Zn, msk.get_msk()[b2], z_2);
    element_pow_zn(tmp_H, mpk.get_mpk()[h], tmp_Zn);
    element_mul(sk.get_sk0()[sk0_2], sk.get_sk0()[sk0_2], tmp_H);

    element_pow_zn(tmp_H, mpk.get_mpk()[h_pow_1_div_a], z);
    element_mul(sk.get_sk0()[sk0_3], sk.get_sk0()[sk0_3], tmp_H);

    element_pow_zn(tmp_G, sk.get_sk0()[sk0_4], z);
    element_mul(sk.get_sk0()[sk0_5], sk.get_sk0()[sk0_5], tmp_G);

    element_mul(alpha_a_1, msk.get_msk()[a], msk.get_msk()[a1]);
    element_mul(alpha_a_2, msk.get_msk()[a], msk.get_msk()[a2]);

    for (auto &entry :sk.get_attr2id()) {
        Hash(tmp_G, entry.first + "11");
        element_mul(tmp_Zn, msk.get_msk()[b1], z_1);
        element_div(tmp_Zn, tmp_Zn, msk.get_msk()[a1]);
        element_pow_zn(tmp_G, tmp_G, tmp_Zn);
        Hash(tmp_G_2, entry.first + "21");
        element_mul(tmp_Zn, msk.get_msk()[b2], z_2);
        element_div(tmp_Zn, tmp_Zn, msk.get_msk()[a1]);
        element_pow_zn(tmp_G_2, tmp_G_2, tmp_Zn);
        Hash(tmp_G_3, entry.first + "31");
        element_div(tmp_Zn, z, alpha_a_1);
        element_pow_zn(tmp_G_3, tmp_G_3, tmp_Zn);
        element_mul(sk.get_sk_y()[entry.second][sk_1], sk.get_sk_y()[entry.second][sk_1], tmp_G);
        element_mul(sk.get_sk_y()[entry.second][sk_1], sk.get_sk_y()[entry.second][sk_1], tmp_G_2);
        element_mul(sk.get_sk_y()[entry.second][sk_1], sk.get_sk_y()[entry.second][sk_1], tmp_G_3);

        Hash(tmp_G, entry.first + "12");
        element_mul(tmp_Zn, msk.get_msk()[b1], z_1);
        element_div(tmp_Zn, tmp_Zn, msk.get_msk()[a2]);
        element_pow_zn(tmp_G, tmp_G, tmp_Zn);
        Hash(tmp_G_2, entry.first + "22");
        element_mul(tmp_Zn, msk.get_msk()[b2], z_2);
        element_div(tmp_Zn, tmp_Zn, msk.get_msk()[a2]);
        element_pow_zn(tmp_G_2, tmp_G_2, tmp_Zn);
        Hash(tmp_G_3, entry.first + "32");
        element_div(tmp_Zn, z, alpha_a_2);
        element_pow_zn(tmp_G_3, tmp_G_3, tmp_Zn);
        element_mul(sk.get_sk_y()[entry.second][sk_2], sk.get_sk_y()[entry.second][sk_2], tmp_G);
        element_mul(sk.get_sk_y()[entry.second][sk_2], sk.get_sk_y()[entry.second][sk_2], tmp_G_2);
        element_mul(sk.get_sk_y()[entry.second][sk_2], sk.get_sk_y()[entry.second][sk_2], tmp_G_3);   
    }

    Hash(tmp_G, "0111");
    element_mul(tmp_Zn, msk.get_msk()[b1], z_1);
    element_div(tmp_Zn, tmp_Zn, msk.get_msk()[a1]);
    element_pow_zn(tmp_G, tmp_G, tmp_Zn);
    Hash(tmp_G_2, "0121");
    element_mul(tmp_Zn, msk.get_msk()[b2], z_2);
    element_div(tmp_Zn, tmp_Zn, msk.get_msk()[a1]);
    element_pow_zn(tmp_G_2, tmp_G_2, tmp_Zn);
    Hash(tmp_G_3, "0131");
    element_div(tmp_Zn, z, alpha_a_1);
    element_pow_zn(tmp_G_3, tmp_G_3, tmp_Zn);
    element_mul(sk.get_sk_prime()[sk_1], sk.get_sk_prime()[sk_1], tmp_G);
    element_mul(sk.get_sk_prime()[sk_1], sk.get_sk_prime()[sk_1], tmp_G_2);
    element_mul(sk.get_sk_prime()[sk_1], sk.get_sk_prime()[sk_1], tmp_G_3);

    Hash(tmp_G, "0112");
    element_mul(tmp_Zn, msk.get_msk()[b1], z_1);
    element_div(tmp_Zn, tmp_Zn, msk.get_msk()[a2]);
    element_pow_zn(tmp_G, tmp_G, tmp_Zn);
    Hash(tmp_G_2, "0122");
    element_mul(tmp_Zn, msk.get_msk()[b2], z_2);
    element_div(tmp_Zn, tmp_Zn, msk.get_msk()[a2]);
    element_pow_zn(tmp_G_2, tmp_G_2, tmp_Zn);
    Hash(tmp_G_3, "0132");
    element_div(tmp_Zn, z, alpha_a_2);
    element_pow_zn(tmp_G_3, tmp_G_3, tmp_Zn);
    element_mul(sk.get_sk_prime()[sk_2], sk.get_sk_prime()[sk_2], tmp_G);
    element_mul(sk.get_sk_prime()[sk_2], sk.get_sk_prime()[sk_2], tmp_G_2);
    element_mul(sk.get_sk_prime()[sk_2], sk.get_sk_prime()[sk_2], tmp_G_3);
    
    element_pow_zn(tmp_G, sk.get_sk2()[0], I_i_1);
    element_pow_zn(tmp_G_2, ID_i_1, z);
    element_mul(sk.get_sk1()[0], sk.get_sk1()[0], tmp_G);
    element_mul(sk.get_sk1()[0], sk.get_sk1()[0], tmp_G_2);

    // remove the first element of sk.sk_2
    sk.get_sk2().pop_front();

    for(int i = 0; i < sk.get_sk2().getSize(); ++i){
        element_pow_zn(tmp_G, mpk.get_gk_pow_a()[sk.get_sk2().getSize()-i-1], z);
        element_mul(sk.get_sk2()[i], sk.get_sk2()[i], tmp_G);
    }
    return true;
}

bool ABET::UserDelegate(ABET_sks &sk, ABET_mpk &mpk, ABET_msk &msk, ABET_ID &ID, int i) {
    GetID_(ID_i_1, mpk, ID, i+1, MODIFIER);
    element_pow_zn(ID_i_1, ID_i_1, msk.get_msk()[a]);

    return SkDelegate(sk, mpk, msk, ID_i_1, ID[i]);
}


/**
 * @brief Decrypt a ciphertext.
 * 
 * @param  res_R[out]       
 * @param  res_r[out]        
 * @param  mpk[in]         
 * @param  msk[in]         
 * @param  ciphertext[in]  
 * @param  sks[in]         
 * @param  MSP[in]         
 * @param  ID[in]          
 * @param  mi[in]           
 * 
 */
void ABET::Decrypt(unsigned char *res_R, element_t res_r, ABET_mpk &mpk, ABET_msk &msk, ABET_ciphertext &ciphertext, ABET_sks &sks, Element_t_matrix *MSP, ABET_ID &ID, int mi){
    ABET_sks sk_p(sks);
    for(int i = mi; i < ciphertext.get_ownerId_length(); ++i) if(!UserDelegate(sk_p, mpk, msk, ID, i)) throw std::runtime_error("delegate failed");

    // retrive R
    Pairing(tmp_GT, sk_p.get_sk1()[0], ciphertext.get_ct0()[ct0_3]);
    Pairing(tmp_GT_2, sk_p.get_sk0()[sk0_5], ciphertext.get_ct1()[0]);
    Pairing(tmp_GT_3, sk_p.get_sk0()[sk0_6], ciphertext.get_ct0()[ct0_4]);
    element_mul(tmp_GT_2, tmp_GT_2, tmp_GT_3);
    element_div(tmp_GT, tmp_GT, tmp_GT_2);
    Hash2(tmp_Zn, tmp_GT);
    unsigned char *R_ = new unsigned char[element_length_in_bytes(tmp_Zn)];
    element_to_bytes(R_, tmp_Zn);
    for(int i = 0;i < element_length_in_bytes(tmp_Zn); ++i) R_[i] ^= ciphertext.get_ct_prime()[i];
    memcpy(res_R, R_, element_length_in_bytes(tmp_Zn)/2);

    bool tag = true;
    for(int i = element_length_in_bytes(tmp_Zn)/2; i < element_length_in_bytes(tmp_Zn); ++i){
        if(R_[i] != 0) {
            tag = false;
            break;
        }
    }
    if(!tag) throw std::runtime_error("unable to adapt");

    // retrive r
    // compute Yi
    // get matrix with attributes
    Element_t_matrix* attributesMatrix = new Element_t_matrix();
    unsigned long int rows = ciphertext.get_ct_y().size();
    for(unsigned long int i = 0; i < rows; i++){
        // judge whether the attribute is in the policy
        if(sk_p.get_attr2id().find(MSP->getName(i)) == sk_p.get_attr2id().end()){
            continue;
        }
        Element_t_vector *v = new Element_t_vector();
        for(signed long int j = 0; j < MSP->col(); ++j) {
            v->pushBack(MSP->getElement(i, j));
        }
        attributesMatrix->pushBack(v);
        delete v;
    }
    // get inverse matrix
    Element_t_matrix* inverse_attributesMatrix = attributesMatrix->inverse();

    unsigned long int r = inverse_attributesMatrix->row();
    unsigned long int c = inverse_attributesMatrix->col();

    Element_t_vector* unit = inverse_attributesMatrix->getCoordinateAxisUnitVector();

    Element_t_vector* x = new Element_t_vector(inverse_attributesMatrix->col(), inverse_attributesMatrix->getElement(0, 0));

    signed long int type = Element_t_matrix::gaussElimination(x, inverse_attributesMatrix, unit);
    if(-1 == type) {
        throw std::runtime_error("POLICY_NOT_SATISFIED");
    }

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
        if(sk_p.get_attr2id().find(MSP->getName(i)) == sk_p.get_attr2id().end()){
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
    Pairing(this->tmp_GT, this->tmp_G, sk_p.get_sk0()[sk0_1]);
    Pairing(this->tmp_GT_2, this->tmp_G_2, sk_p.get_sk0()[sk0_2]);
    Pairing(this->tmp_GT_3, this->tmp_G_3, sk_p.get_sk0()[sk0_3]);

    element_mul(num, this->tmp_GT, this->tmp_GT_2);
    element_mul(num, num, this->tmp_GT_3);

    // den
    element_set1(this->tmp_G);
    element_set1(this->tmp_G_2);
    element_set1(this->tmp_G_3);
    count = 0;
    for(unsigned long int i = 0; i < rows; i++){
        // judge whether the attribute is in the policy
        if(sk_p.get_attr2id().find(MSP->getName(i)) == sk_p.get_attr2id().end()){
            continue;
        }
        element_pow_zn(this->tmp_G_4, sk_p.get_sk_y(sk_p.get_attr2id()[MSP->getName(i)])[sk_1], x->getElement(count));
        element_mul(this->tmp_G, this->tmp_G, this->tmp_G_4);
        element_pow_zn(this->tmp_G_4, sk_p.get_sk_y(sk_p.get_attr2id()[MSP->getName(i)])[sk_2], x->getElement(count));
        element_mul(this->tmp_G_2, this->tmp_G_2, this->tmp_G_4);
        element_pow_zn(this->tmp_G_4, sk_p.get_sk_y(sk_p.get_attr2id()[MSP->getName(i)])[sk_3], x->getElement(count));
        element_mul(this->tmp_G_3, this->tmp_G_3, this->tmp_G_4);
        count++;
    }
    // sk_prime_1 * tmp_G
    element_mul(this->tmp_G, sk_p.get_sk_prime()[sk_1], this->tmp_G);
    // sk_prime_2 * tmp_G_2
    element_mul(this->tmp_G_2, sk_p.get_sk_prime()[sk_2], this->tmp_G_2);
    // sk_prime_3 * tmp_G_3
    element_mul(this->tmp_G_3, sk_p.get_sk_prime()[sk_3], this->tmp_G_3);

    // e(tmp_G, ct01) * e(tmp_G_2, ct02) * e(tmp_G_3, ct03)
    Pairing(this->tmp_GT, this->tmp_G, ciphertext.get_ct0()[ct0_1]);
    Pairing(this->tmp_GT_2, this->tmp_G_2, ciphertext.get_ct0()[ct0_2]);
    Pairing(this->tmp_GT_3, this->tmp_G_3, ciphertext.get_ct0()[ct0_3]);

    element_mul(den, this->tmp_GT, this->tmp_GT_2);
    element_mul(den, den, this->tmp_GT_3);

    // res = - num / den
    element_div(num, num, den);
    element_neg(num, num);
    this->Hash2(this->tmp_Zn, num);

    element_div(res_r, ciphertext.get_ct_p()[0], this->tmp_Zn);

    // free
    element_clear(num);
    element_clear(den);
    delete attributesMatrix;
    delete inverse_attributesMatrix;
    delete unit;
    delete x;
    delete R_;
}

void ABET::GetID_(element_t ID_, ABET_mpk &mpk, ABET_ID &ID, int mi_oj, int type){
    int k = mpk.get_gk().getSize();
    if(type == MODIFIER){
        element_set(this->tmp_G, mpk.get_mpk()[g]);
        for(int i=0; i<mi_oj; i++){
            element_pow_zn(this->tmp_G_2, mpk.get_gk()[k-i-1], ID[i]);
            element_mul(this->tmp_G, this->tmp_G, this->tmp_G_2);
        }
        element_set(ID_, this->tmp_G);
    }else if(type == OWNER){
        element_set(this->tmp_H, mpk.get_mpk()[h]);
        for(int i=0 ;i<mi_oj; i++){
            element_pow_zn(this->tmp_H_2, mpk.get_hk()[k-i-1], ID[i]);
            element_mul(this->tmp_H, this->tmp_H, this->tmp_H_2);
        }
        element_set(ID_, this->tmp_H);
    }
}

Element_t_matrix* ABET::ComputeMSP(const std::string &policy_str){
    std::vector<std::string> postfix_expression = Policy_resolution::infixToPostfix(policy_str);
    Binary_tree_policy* binary_tree_expression = Policy_resolution::postfixToBinaryTree(postfix_expression, Zn);
    Element_t_matrix* MSP = Policy_generation::getPolicyInMatrixFormFromTree(binary_tree_expression);

    delete binary_tree_expression;
    return MSP;
}

ABET::~ABET(){
    element_s *clear_list[] = {d1, d2, d3, r1, r2, R,
        b1r1a1, b1r1a2, b2r2a1, b2r2a2, r1r2a1a, r1r2a2a, aa1, aa2,
        s1, s2, z_1, z_2, z, alpha_a_1, alpha_a_2, ID_i_1,
        tmp_G, tmp_G_2, tmp_G_3, tmp_G_4, 
        tmp_H, tmp_H_2, 
        tmp_GT, tmp_GT_2, tmp_GT_3, 
        tmp_Zn, tmp_Zn_2};
    
    for(int i = 0; i < sizeof(clear_list)/sizeof(clear_list[0]); i++){
        element_clear(clear_list[i]);
    }
}