#include "scheme/IBCH/IB_CH_ZSS_S1_2003.h"

IB_CH_ZSS_S1_2003::IB_CH_ZSS_S1_2003(int curve, bool swap) : PbcScheme(curve) {
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

    element_init_same_as(tmp_G, G1);
    element_init_same_as(tmp_H, G2);
    element_init_same_as(tmp_GT, GT);
    element_init_same_as(tmp_GT_2, GT);
    element_init_same_as(tmp_Zn, Zn);
    element_init_same_as(tmp_Zn_2, Zn);
}

/**
 * @brief 
 * 
 * @param  pp[out]   
 * @param  msk[out]  
 * 
 */
void IB_CH_ZSS_S1_2003::SetUp(IB_CH_ZSS_S1_2003_pp &pp, IB_CH_ZSS_S1_2003_msk &msk) {
    // s
    element_random(tmp_Zn);
    msk.set(s, tmp_Zn);
    // P
    element_random(tmp_H);
    pp.set(P, tmp_H);
    // Ppub = sP
    element_mul_zn(tmp_H, tmp_H, tmp_Zn);
    pp.set(Ppub, tmp_H);
}

/**
 * @brief 
 * 
 * @param  pk[out]   
 * @param  sk[out]   
 * @param  msk[in]  
 * @param  ID[in]    
 * 
 */
void IB_CH_ZSS_S1_2003::Extract(IB_CH_ZSS_S1_2003_pk &pk, IB_CH_ZSS_S1_2003_sk &sk, IB_CH_ZSS_S1_2003_msk &msk, element_t ID) {
    // QID = H(ID)
    this->H(this->tmp_G, ID);
    pk.set(QID, this->tmp_G);

    // SID = s*QID
    element_mul_zn(tmp_G, this->tmp_G, msk[s]);
    sk.set(SID, tmp_G);
}

void IB_CH_ZSS_S1_2003::H(element_t res, element_t m) {
    HASH::hash(res, m);
}

void IB_CH_ZSS_S1_2003::Pairing(element_t res, element_t g1, element_t g2){
    if(swap){
        element_pairing(res, g2, g1);
    }else{
        element_pairing(res, g1, g2);
    }
}

/**
 * @brief 
 * 
 * @param  h[out]   
 * @param  r[out]   
 * @param  m[in]    
 * @param  ID[in]   
 * @param  pp[in]  
 * 
 */
void IB_CH_ZSS_S1_2003::Hash(IB_CH_ZSS_S1_2003_h &h, IB_CH_ZSS_S1_2003_r &r, element_t m, element_t ID, IB_CH_ZSS_S1_2003_pp &pp) {
    // r
    element_random(tmp_G);
    r.set(r1, tmp_G);

    // e(R, P)
    Pairing(tmp_GT, tmp_G, pp[P]);
    
    this->H(this->tmp_G, ID);
    this->H(this->tmp_Zn, m);
    element_mul_zn(this->tmp_G, this->tmp_G, this->tmp_Zn);
    Pairing(this->tmp_GT_2, this->tmp_G, pp[Ppub]);
    element_mul(tmp_GT, tmp_GT, tmp_GT_2);
    h.set(h1, tmp_GT);
}

/**
 * @brief 
 * 
 * @param  h[in]   
 * @param  m[in]    
 * @param  r[in]   
 * @param  ID[in]   
 * @param  pp[in]  
 * 
 * @return 
 */
bool IB_CH_ZSS_S1_2003::Check(IB_CH_ZSS_S1_2003_h &h, element_t m, IB_CH_ZSS_S1_2003_r &r, element_t ID, IB_CH_ZSS_S1_2003_pp &pp){
    // e(R, P)
    Pairing(tmp_GT, r[r1], pp[P]);
    
    this->H(this->tmp_G, ID);
    this->H(this->tmp_Zn, m);
    element_mul_zn(this->tmp_G, this->tmp_G, this->tmp_Zn);
    Pairing(this->tmp_GT_2, this->tmp_G, pp[Ppub]);
    element_mul(tmp_GT, tmp_GT, tmp_GT_2);
    
    return element_cmp(h[h1], tmp_GT) == 0;
}

/**
 * @brief 
 * 
 * @param  r_p[out]  
 * @param  m_p[in]   
 * @param  h[in]    
 * @param  m[in]     
 * @param  r[in]    
 * @param  ID[in]    
 * @param  sk[in]   
 * @param  pp[in]   
 * 
 */
void IB_CH_ZSS_S1_2003::Adapt(IB_CH_ZSS_S1_2003_r &r_p, element_t m_p, IB_CH_ZSS_S1_2003_h &h, element_t m, IB_CH_ZSS_S1_2003_r &r, element_t ID, IB_CH_ZSS_S1_2003_sk &sk, IB_CH_ZSS_S1_2003_pp &pp) {
    this->H(this->tmp_Zn, m);
    this->H(this->tmp_Zn_2, m_p);
    element_sub(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);

    element_mul_zn(this->tmp_G, sk[SID], this->tmp_Zn);
    element_add(tmp_G, this->tmp_G, r[r1]);
    r_p.set(r1, tmp_G);
}

/**
 * @brief 
 * 
 * @param  h_p[in]  
 * @param  m_p[in]   
 * @param  r_p[in]  
 * @param  ID[in]    
 * @param  pp[in]   
 * 
 * @return 
 */
bool IB_CH_ZSS_S1_2003::Verify(IB_CH_ZSS_S1_2003_h &h_p, element_t m_p, IB_CH_ZSS_S1_2003_r &r_p, element_t ID, IB_CH_ZSS_S1_2003_pp &pp) {
    return this->Check(h_p, m_p, r_p, ID, pp);
}

IB_CH_ZSS_S1_2003::~IB_CH_ZSS_S1_2003() {
    element_clear(tmp_G);
    element_clear(tmp_H);
    element_clear(tmp_GT);
    element_clear(tmp_GT_2);
    element_clear(tmp_Zn);
    element_clear(tmp_Zn_2);

    element_clear(G1);
    element_clear(G2);
    element_clear(GT);
    element_clear(Zn);
}
