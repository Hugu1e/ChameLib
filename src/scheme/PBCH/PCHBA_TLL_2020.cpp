#include "scheme/PBCH/PCHBA_TLL_2020.h"

PCHBA_TLL_2020::PCHBA_TLL_2020(int curve, bool swap): PbcScheme(curve){
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

    abet.init(G1, G2, GT, Zn, swap);

    element_s *init_G1_list[] = {tmp_G, tmp_G_2};
    element_s *init_G2_list[] = {tmp_H, tmp_H_2};
    element_s *init_GT_list[] = {tmp_GT, tmp_GT_2, tmp_GT_3};
    element_s *init_zr_list[] = {r, s1, s2, esk, tmp_Zn, tmp_Zn_2, tmp_Zn_3};

    for(int i = 0;i < sizeof(init_G1_list)/sizeof(element_s*);i++){
        element_init_same_as(init_G1_list[i], G1);
    }
    for(int i = 0;i < sizeof(init_G2_list)/sizeof(element_s*);i++){
        element_init_same_as(init_G2_list[i], G2);
    }
    for(int i = 0;i < sizeof(init_GT_list)/sizeof(element_s*);i++){
        element_init_same_as(init_GT_list[i], GT);
    }
    for(int i = 0;i < sizeof(init_zr_list)/sizeof(element_s*);i++){
        element_init_same_as(init_zr_list[i], Zn);
    }
}

bool PCHBA_TLL_2020::Pairing(element_t res, element_t a, element_t b){
    if(swap){
        element_pairing(res, b, a);
    }else{
        element_pairing(res, a, b);
    }
}

void PCHBA_TLL_2020::H(element_t res, std::string str){
    HASH::hash(res, str);
}

/**
 * @brief 
 * 
 * @param  pkPCHBA[out]  
 * @param  skPCHBA[out]  
 * @param  k[in]         
 * 
 */
void PCHBA_TLL_2020::SetUp(PCHBA_TLL_2020_pk &pkPCHBA, PCHBA_TLL_2020_sk &skPCHBA, int k) {
    abet.Setup(skPCHBA.get_skABET(), pkPCHBA.get_pkABET(), k);

    // x
    element_random(tmp_Zn);
    skPCHBA.get_skCHET().set(x, tmp_Zn);

    // h^x
    element_pow_zn(tmp_H, pkPCHBA.get_pkABET().get_mpk()[ABET::h], tmp_Zn);
    pkPCHBA.get_pkCHET().set(h_pow_x, tmp_H);
}

/**
 * @brief 
 * 
 * @param  sksPCHBA[out]   
 * @param  pkPCHBA[in]    
 * @param  skPCHBA[in]    
 * @param  attr_list[in]  
 * @param  ID[in]         
 * @param  mi[in]          length of modifierID
 * 
 */
void PCHBA_TLL_2020::KeyGen(PCHBA_TLL_2020_sks &sksPCHBA, PCHBA_TLL_2020_pk &pkPCHBA, PCHBA_TLL_2020_sk &skPCHBA, std::vector<std::string> &attr_list, PCHBA_TLL_2020_ID &ID, int mi) {
    sksPCHBA.get_skCHET().set(x, skPCHBA.get_skCHET()[x]);
   
    abet.KeyGen(sksPCHBA.get_sksABET(), skPCHBA.get_skABET(), pkPCHBA.get_pkABET(), attr_list, ID.get_IDABET(), mi);
}

/**
 * @brief 
 * 
 * @param  h[out]        
 * @param  random[out]   
 * @param  m[in]         
 * @param  pkPCHBA[in]  
 * @param  skPCHBA[in]  
 * @param  MSP[in]      
 * @param  ID[in]       
 * @param  oj[in]        length of ownerID
 * 
 */
void PCHBA_TLL_2020::Hash(PCHBA_TLL_2020_h &h, PCHBA_TLL_2020_r &random, element_t m, PCHBA_TLL_2020_pk &pkPCHBA, PCHBA_TLL_2020_sk &skPCHBA, Element_t_matrix *MSP, PCHBA_TLL_2020_ID &ID, int oj) {
    // r
    element_random(this->r);
    // R
    unsigned char R[element_length_in_bytes(r) / 2];
    RandomGenerator::Random_bytes(R, element_length_in_bytes(r) / 2);

    element_random(this->s1);
    element_random(this->s2);
    // C
    abet.Encrypt(random.get_C(), pkPCHBA.get_pkABET(), skPCHBA.get_skABET(), 
        this->r, R, element_length_in_bytes(r) / 2, MSP, ID.get_IDABET(), oj, this->s1, this->s2);

    // esk
    element_random(this->esk);
    // epk = g^esk
    element_pow_zn(tmp_G, pkPCHBA.get_pkABET().get_mpk()[ABET::g], this->esk);
    random.get_r().set(epk, tmp_G);

    // c = h^(s1+s2+H(R))
    element_add(this->tmp_Zn, this->s1, this->s2);
    H(tmp_Zn_2, std::string((char *)R, element_length_in_bytes(r) / 2));
    element_add(this->tmp_Zn_2, this->tmp_Zn, tmp_Zn_2);
    element_pow_zn(tmp_H, pkPCHBA.get_pkABET().get_mpk()[ABET::h], this->tmp_Zn_2);
    random.get_r().set(c, tmp_H);

    // epk_str + c_str
    unsigned char bytes_epk[element_length_in_bytes(tmp_G)];
    unsigned char bytes_c[element_length_in_bytes(tmp_H)];
    element_to_bytes(bytes_epk, tmp_G);
    element_to_bytes(bytes_c, tmp_H);
    std::string epk_str((char *)bytes_epk, element_length_in_bytes(tmp_G));
    std::string c_str((char *)bytes_c, element_length_in_bytes(tmp_H));
    std::string combine = epk_str + c_str;
    // sigma = esk + (s1 + s2) * H2(epk||c)
    H(this->tmp_Zn_2, combine);
    element_mul(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    element_add(tmp_Zn, this->esk, this->tmp_Zn);
    random.get_r().set(sigma, tmp_Zn);

    // p = pk^r
    element_pow_zn(tmp_H_2, pkPCHBA.get_pkCHET()[h_pow_x], this->r);
    random.get_r().set(p, tmp_H_2);

    // e = H2(R)
    H(tmp_Zn, std::string((char *)R, element_length_in_bytes(r) / 2));
    // h' = h^e
    element_pow_zn(tmp_H, pkPCHBA.get_pkABET().get_mpk()[ABET::h], this->tmp_Zn);
    random.get_r().set(h_, tmp_H);

    // b = p * (h'^m)
    element_pow_zn(this->tmp_H, tmp_H, m);
    element_mul(tmp_H_2, tmp_H_2, this->tmp_H);
    h.get_h().set(b, tmp_H_2);
}

/**
 * @brief 
 * 
 * @param  h[in]        
 * @param  random[in]   
 * @param  m[in]         
 * @param  pkPCHBA[in]  
 * 
 * @return 
 */
bool PCHBA_TLL_2020::Check(PCHBA_TLL_2020_h &h, PCHBA_TLL_2020_r &random, element_t m, PCHBA_TLL_2020_pk &pkPCHBA) {
    // b =? p * (h'^m)
    element_pow_zn(this->tmp_H, random.get_r()[h_], m);
    element_mul(this->tmp_H_2, random.get_r()[p], this->tmp_H);
    if (element_cmp(h.get_h()[b], this->tmp_H_2) != 0) {
        return false;
    }
    // e(g^a, ct2)^sigma =? e(epk,ct1) * e(g,ct3)^(H2(epk||c))
    Pairing(this->tmp_GT, pkPCHBA.get_pkABET().get_mpk()[ABET::g_pow_a], random.get_C().get_ct2()[0]);
    element_pow_zn(this->tmp_GT, this->tmp_GT, random.get_r()[sigma]);

    Pairing(this->tmp_GT_2, random.get_r()[epk], random.get_C().get_ct1()[0]);

    Pairing(this->tmp_GT_3, pkPCHBA.get_pkABET().get_mpk()[ABET::g], random.get_C().get_ct3()[0]);
    // epk_str + c_str
    unsigned char bytes_epk[element_length_in_bytes(random.get_r()[epk])];
    unsigned char bytes_c[element_length_in_bytes(random.get_r()[c])];
    element_to_bytes(bytes_epk, random.get_r()[epk]);
    element_to_bytes(bytes_c, random.get_r()[c]);
    std::string epk_str((char *)bytes_epk, element_length_in_bytes(random.get_r()[epk]));
    std::string c_str((char *)bytes_c, element_length_in_bytes(random.get_r()[c]));
    std::string combine = epk_str + c_str;
    H(this->tmp_Zn, combine);
    element_pow_zn(this->tmp_GT_3, this->tmp_GT_3, this->tmp_Zn);

    element_mul(this->tmp_GT_2, this->tmp_GT_2, this->tmp_GT_3);

    if (element_cmp(this->tmp_GT, this->tmp_GT_2) != 0) {
        return false;
    }
    return true;
}

/**
 * @brief 
 * 
 * @param  random_p[out]  
 * @param  m_p[in]        
 * @param  h[in]         
 * @param  random[in]    
 * @param  m[in]          
 * @param  MSP[in]       
 * @param  ID[in]        
 * @param  mi[in]         length of modifierID
 * @param  pkPCHBA[in]   
 * @param  skPCHBA[in]   
 * @param  sksPCHBA[in]  
 * 
 */
void PCHBA_TLL_2020::Adapt(PCHBA_TLL_2020_r &random_p, element_t m_p, PCHBA_TLL_2020_h &h, PCHBA_TLL_2020_r &random, element_t m, Element_t_matrix *MSP, PCHBA_TLL_2020_ID &ID, int mi, PCHBA_TLL_2020_pk &pkPCHBA, PCHBA_TLL_2020_sk &skPCHBA, PCHBA_TLL_2020_sks &sksPCHBA) {
    // R
    unsigned char R[element_length_in_bytes(r) / 2];
    
    // retrieve R, r
    this->abet.Decrypt(R, this->r, pkPCHBA.get_pkABET(), skPCHBA.get_skABET(), random.get_C(), sksPCHBA.get_sksABET(), MSP, ID.get_IDABET(), mi);

    element_random(this->s1);
    element_random(this->s2);
    // C
    abet.Encrypt(random_p.get_C(), pkPCHBA.get_pkABET(), skPCHBA.get_skABET(), 
        this->r, R, element_length_in_bytes(r) / 2, MSP, ID.get_IDABET(), mi, this->s1, this->s2);

    // esk
    element_random(this->esk);
    // epk = g^esk
    element_pow_zn(tmp_G, pkPCHBA.get_pkABET().get_mpk()[ABET::g], this->esk);
    random_p.get_r().set(epk, tmp_G);

    // c = h^(s1+s2+H(R))
    element_add(this->tmp_Zn, this->s1, this->s2);
    H(tmp_Zn_2, std::string((char *)R, element_length_in_bytes(r) / 2));
    element_add(this->tmp_Zn_2, this->tmp_Zn, tmp_Zn_2);
    element_pow_zn(tmp_H, pkPCHBA.get_pkABET().get_mpk()[ABET::h], this->tmp_Zn_2);
    random_p.get_r().set(c, tmp_H);

    // epk_str + c_str
    unsigned char bytes_epk[element_length_in_bytes(tmp_G)];
    unsigned char bytes_c[element_length_in_bytes(tmp_H)];
    element_to_bytes(bytes_epk, tmp_G);
    element_to_bytes(bytes_c, tmp_H);
    std::string epk_str((char *)bytes_epk, element_length_in_bytes(tmp_G));
    std::string c_str((char *)bytes_c, element_length_in_bytes(tmp_H));
    std::string combine = epk_str + c_str;
    // sigma = esk + (s1 + s2) * H2(epk||c)
    H(this->tmp_Zn_2, combine);
    element_mul(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    element_add(tmp_Zn, this->esk, this->tmp_Zn);
    random_p.get_r().set(sigma, tmp_Zn);

    // p = pk^r
    // e = H2(R)
    H(tmp_Zn, std::string((char *)R, element_length_in_bytes(r) / 2));
    // r' = r + (m-m')*e/x
    element_sub(this->tmp_Zn_2, m, m_p);
    element_mul(this->tmp_Zn_2, this->tmp_Zn_2, this->tmp_Zn);
    element_div(this->tmp_Zn_2, this->tmp_Zn_2, sksPCHBA.get_skCHET()[x]);
    element_add(this->tmp_Zn_3, this->r, this->tmp_Zn_2);
    element_pow_zn(tmp_H_2, pkPCHBA.get_pkCHET()[h_pow_x], tmp_Zn_3);
    random_p.get_r().set(p, tmp_H_2);

    // e = H2(R)
    H(tmp_Zn, std::string((char *)R, element_length_in_bytes(r) / 2));
    // h' = h^e
    element_pow_zn(tmp_H, pkPCHBA.get_pkABET().get_mpk()[ABET::h], this->tmp_Zn);
    random_p.get_r().set(h_, tmp_H);
}

/**
 * @brief 
 * 
 * @param  h_p[in]       
 * @param  random_p[in]  
 * @param  m_p[in]        
 * @param  pkPCHBA[in]   
 * 
 * @return 
 */
bool PCHBA_TLL_2020::Verify(PCHBA_TLL_2020_h &h_p, PCHBA_TLL_2020_r &random_p, element_t m_p, PCHBA_TLL_2020_pk &pkPCHBA) {
    return this->Check(h_p, random_p, m_p, pkPCHBA);
}

PCHBA_TLL_2020::~PCHBA_TLL_2020() {
    element_s *clear_list[] = {r, s1, s2, esk, 
        tmp_G, tmp_G_2, tmp_H, tmp_H_2, tmp_GT, tmp_GT_2, tmp_GT_3, tmp_Zn, tmp_Zn_2, tmp_Zn_3, 
        G1, G2, GT, Zn};
    for(int i=0;i<sizeof(clear_list)/sizeof(clear_list[0]);i++){
        element_clear(clear_list[i]);
    }
}