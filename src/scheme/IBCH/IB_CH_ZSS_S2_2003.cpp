#include "scheme/IBCH/IB_CH_ZSS_S2_2003.h"

IB_CH_ZSS_S2_2003::IB_CH_ZSS_S2_2003(int curve): PbcScheme(curve) {
    element_init_G1(G1, pairing);
    element_init_GT(GT, pairing);
    element_init_Zr(Zn, pairing);

    element_init_same_as(tmp_G, G1);
    element_init_same_as(tmp_G_2, G1);
    element_init_same_as(tmp_GT, GT);
    element_init_same_as(tmp_GT_2, GT);
    element_init_same_as(tmp_Zn, Zn);
    element_init_same_as(tmp_Zn_2, Zn);
    element_init_same_as(tmp_Zn_3, Zn);
}

/**
 * @brief 
 * 
 * @param  pp[out]   
 * @param  msk[out]  
 * 
 */
void IB_CH_ZSS_S2_2003::SetUp(IB_CH_ZSS_S2_2003_pp &pp, IB_CH_ZSS_S2_2003_msk &msk) {
    element_random(tmp_Zn);
    msk.set(s, tmp_Zn);
    
    element_random(tmp_G);
    pp.set(P, tmp_G);

    // e(P, P)
    element_pairing(tmp_GT, tmp_G, tmp_G);
    pp.set(ePP, tmp_GT);

    element_mul_zn(tmp_G, tmp_G, tmp_Zn);
    pp.set(Ppub, tmp_G);
}

/**
 * @brief 
 * 
 * @param  sk[out]   
 * @param  msk[in]  
 * @param  ID[in]    
 * @param  pp[in]   
 * 
 */
void IB_CH_ZSS_S2_2003::Extract(IB_CH_ZSS_S2_2003_sk &sk, IB_CH_ZSS_S2_2003_msk &msk, element_t ID, IB_CH_ZSS_S2_2003_pp &pp) {
    H1(this->tmp_Zn, ID);
    element_add(this->tmp_Zn, msk[s], this->tmp_Zn);
    element_invert(this->tmp_Zn, this->tmp_Zn);
    element_mul_zn(tmp_G, pp[P], this->tmp_Zn);
    sk.set(SID, tmp_G);
}

void IB_CH_ZSS_S2_2003::H1(element_t res, element_t m) {
    HASH::hash(res, m);
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
void IB_CH_ZSS_S2_2003::Hash(IB_CH_ZSS_S2_2003_h &h, IB_CH_ZSS_S2_2003_r &r, element_t m, element_t ID, IB_CH_ZSS_S2_2003_pp &pp) {
    // r
    element_random(tmp_G);
    r.set(r1, tmp_G);

    H1(this->tmp_Zn, ID);
    element_mul_zn(this->tmp_G_2, pp[P], this->tmp_Zn);
    element_add(this->tmp_G_2, this->tmp_G_2, pp[Ppub]);
    element_pairing(tmp_GT, tmp_G_2, tmp_G);

    H1(this->tmp_Zn, m);
    element_pow_zn(tmp_GT, tmp_GT, this->tmp_Zn);

    element_pow_zn(this->tmp_GT_2, pp[ePP], this->tmp_Zn);
    
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
bool IB_CH_ZSS_S2_2003::Check(IB_CH_ZSS_S2_2003_h &h, element_t m, IB_CH_ZSS_S2_2003_r &r, element_t ID, IB_CH_ZSS_S2_2003_pp &pp){
    H1(this->tmp_Zn, ID);
    element_mul_zn(this->tmp_G_2, pp[P], this->tmp_Zn);
    element_add(this->tmp_G_2, this->tmp_G_2, pp[Ppub]);
    element_pairing(tmp_GT, tmp_G_2, r[r1]);

    H1(this->tmp_Zn, m);
    element_pow_zn(tmp_GT, tmp_GT, this->tmp_Zn);

    element_pow_zn(this->tmp_GT_2, pp[ePP], this->tmp_Zn);
    
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
void IB_CH_ZSS_S2_2003::Adapt(IB_CH_ZSS_S2_2003_r &r_p, element_t m_p, IB_CH_ZSS_S2_2003_h &h, element_t m, IB_CH_ZSS_S2_2003_r &r, element_t ID, IB_CH_ZSS_S2_2003_sk &sk, IB_CH_ZSS_S2_2003_pp &pp){
    this->H1(this->tmp_Zn, m);
    this->H1(this->tmp_Zn_2, m_p);
    element_sub(this->tmp_Zn_3, this->tmp_Zn, this->tmp_Zn_2);

    element_mul_zn(this->tmp_G, sk[SID], this->tmp_Zn_3);
    element_mul_zn(this->tmp_G_2, r[r1], this->tmp_Zn);
    element_add(this->tmp_G, this->tmp_G, this->tmp_G_2);

    element_invert(tmp_Zn_2, tmp_Zn_2);
    element_mul_zn(this->tmp_G, tmp_G, this->tmp_Zn_2);

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
bool IB_CH_ZSS_S2_2003::Verify(IB_CH_ZSS_S2_2003_h &h_p, element_t m_p, IB_CH_ZSS_S2_2003_r &r_p, element_t ID, IB_CH_ZSS_S2_2003_pp &pp) {
    return this->Check(h_p, m_p, r_p, ID, pp);
}

IB_CH_ZSS_S2_2003::~IB_CH_ZSS_S2_2003(){
    element_clear(tmp_G);
    element_clear(tmp_G_2);
    element_clear(tmp_GT);
    element_clear(tmp_GT_2);
    element_clear(tmp_Zn);
    element_clear(tmp_Zn_2);
    element_clear(tmp_Zn_3);
    element_clear(G1);
    element_clear(GT);
    element_clear(Zn);
}
