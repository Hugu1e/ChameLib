#include "scheme/IBCH/IB_CH_KEF_CZS_2014.h"

IB_CH_KEF_CZS_2014::IB_CH_KEF_CZS_2014(int curve, bool swap):PbcScheme(curve) {
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
    element_init_same_as(tmp_G_2, G1);
    element_init_same_as(tmp_H, G2);
    element_init_same_as(tmp_GT, GT);
    element_init_same_as(tmp_GT_2, GT);
    element_init_same_as(tmp_GT_3, GT);
    element_init_same_as(tmp_Zn, Zn);
}

void IB_CH_KEF_CZS_2014::Pairing(element_t res, element_t g1, element_t g2){
    if(swap){
        element_pairing(res, g2, g1);
    }else{
        element_pairing(res, g1, g2);
    }
}

/**
 * @brief 
 * 
 * @param  pp[out]   
 * @param  msk[out]  
 * 
 */
void IB_CH_KEF_CZS_2014::SetUp(IB_CH_KEF_CZS_2014_pp &pp, IB_CH_KEF_CZS_2014_msk &msk) {
    // P
    element_random(tmp_G);
    pp.set(P, tmp_G);
    
    // x
    element_random(tmp_Zn);
    msk.set(0, tmp_Zn);
    // Ppub = x * P
    element_mul_zn(tmp_G, tmp_G, tmp_Zn);
    pp.set(Ppub, tmp_G);
}

/**
 * @brief 
 * 
 * @param  td[out]   
 * @param  ID[in]    
 * @param  msk[in]  
 * 
 */
void IB_CH_KEF_CZS_2014::Extract(IB_CH_KEF_CZS_2014_td &td, std::string ID, IB_CH_KEF_CZS_2014_msk &msk) {  
    // QID = H(ID)
    this->H(tmp_H, ID);
    // SID = x * QID
    element_mul_zn(tmp_H, tmp_H, msk[0]);
    td.set(SID, tmp_H);
}
 

void IB_CH_KEF_CZS_2014::H(element_t res, std::string m) {
    HASH::hash(res, m);
}

/**
 * @brief 
 * 
 * @param  h[out]   
 * @param  r[out]   
 * @param  ID[in]   
 * @param  L[in]    
 * @param  m[in]    
 * @param  pp[in]  
 * 
 */
void IB_CH_KEF_CZS_2014::Hash(IB_CH_KEF_CZS_2014_h &h, IB_CH_KEF_CZS_2014_r &r, std::string ID, std::string L, element_t m, IB_CH_KEF_CZS_2014_pp &pp) {
    // a
    element_random(tmp_Zn);
    // QID = H(ID)
    this->H(tmp_H, ID);

    // r1 = a * P
    element_mul_zn(tmp_G, pp[P], tmp_Zn);
    r.set(r1, tmp_G);
    // r2 = e(a * Ppub, QID)
    element_mul_zn(tmp_G, pp[Ppub], tmp_Zn);
    Pairing(tmp_GT, tmp_G, tmp_H);
    r.set(r2, tmp_GT);

    // h = a * P + m * H(L)
    this->H(tmp_G, L);
    element_mul_zn(tmp_G, tmp_G, m);
    element_add(tmp_G, r[r1], tmp_G);
    h.set(h1, tmp_G);
}

/**
 * @brief 
 * 
 * @param  h[in]   
 * @param  r[in]   
 * @param  L[in]    
 * @param  m[in]    
 * @param  td[in]  
 * 
 * @return 
 */
bool IB_CH_KEF_CZS_2014::Check(IB_CH_KEF_CZS_2014_h &h, IB_CH_KEF_CZS_2014_r &r, std::string L, element_t m, IB_CH_KEF_CZS_2014_td &td){
    // h = r1 + m * H(L)
    this->H(tmp_G, L);
    element_mul_zn(tmp_G, tmp_G, m);
    element_add(tmp_G, r[r1], tmp_G);
    if(element_cmp(h[h1], tmp_G) != 0){
        return false;
    }

    // check the correctness of the r
    // e(a * P,SID) == e(a * Ppub, QID)
    // e(r1, SID) == r2
    Pairing(this->tmp_GT, r[r1], td[SID]);
    if(element_cmp(this->tmp_GT, r[r2]) != 0){
        return false;
    }
    return true;
}

/**
 * @brief 
 * 
 * @param  r_p[out]  
 * @param  m_p[in]   
 * @param  h[in]    
 * @param  r[in]    
 * @param  L[in]     
 * @param  m[in]     
 * @param  td[in]   
 * 
 */
void IB_CH_KEF_CZS_2014::Adapt(IB_CH_KEF_CZS_2014_r &r_p, element_t m_p, IB_CH_KEF_CZS_2014_h &h, IB_CH_KEF_CZS_2014_r &r, std::string L, element_t m, IB_CH_KEF_CZS_2014_td &td) {
    // r1_p = r1 + (m - m_p) * H(L)
    element_sub(this->tmp_Zn, m, m_p);
    this->H(this->tmp_G, L);
    element_mul_zn(this->tmp_G_2, this->tmp_G, this->tmp_Zn);
    element_add(tmp_G_2, r[r1], this->tmp_G_2);
    r_p.set(r1, tmp_G_2);
    
    // r2_p = r2 * e(SID, H(L))^(m-m_p)
    Pairing(this->tmp_GT, tmp_G, td[SID]);
    element_pow_zn(this->tmp_GT_2, this->tmp_GT, this->tmp_Zn);
    element_mul(tmp_GT_2, r[r2], this->tmp_GT_2);
    r_p.set(r2, tmp_GT_2);

    // check the correctness of the r_p
    // e(r1_p, SID) == r2_p
    Pairing(this->tmp_GT_3, r_p[r1], td[SID]);
    if(element_cmp(this->tmp_GT_3, r_p[r2]) != 0){
        throw std::runtime_error("IB_CH_KEF_CZS_2014::Adapt(): Adapt failed, r_p is invaid");
    }
}

/**
 * @brief 
 * 
 * @param  h[in]   
 * @param  r[in]   
 * @param  L[in]    
 * @param  m[in]    
 * @param  td[in]  
 * 
 * @return 
 */
bool IB_CH_KEF_CZS_2014::Verify(IB_CH_KEF_CZS_2014_h &h, IB_CH_KEF_CZS_2014_r &r, std::string L, element_t m, IB_CH_KEF_CZS_2014_td &td) {
    return this->Check(h, r, L, m, td);
}


IB_CH_KEF_CZS_2014::~IB_CH_KEF_CZS_2014() {
    element_clear(tmp_G);
    element_clear(tmp_G_2);
    element_clear(tmp_H);
    element_clear(tmp_GT);
    element_clear(tmp_GT_2);
    element_clear(tmp_GT_3);
    element_clear(tmp_Zn);

    element_clear(G1);
    element_clear(G2);
    element_clear(GT);
    element_clear(Zn);
}