#include "scheme/CH/CH_KEF_CZK_2004.h"

CH_KEF_CZK_2004::CH_KEF_CZK_2004(int curve, int group): PbcScheme(curve){
    switch(group){
        case Group::G1:
            element_init_G1(G1, pairing);
            break;
        case Group::G2:
            element_init_G2(G1, pairing);
            break;
        case Group::GT:
            element_init_GT(G1, pairing);
            break;
        default:
            throw CurveException(CurveException::INVALID_GROUP);
    }
    element_init_Zr(Zn, pairing);

    element_init_same_as(tmp_G, G1);
    element_init_same_as(tmp_G_2, G1);
    element_init_same_as(tmp_Zn, Zn);
    element_init_same_as(tmp_Zn_2, Zn);
    element_init_same_as(tmp_Zn_3, Zn);
    element_init_same_as(tmp_Zn_4, Zn);
}

/**
 * @brief 
 * 
 * @param  pp[out]  
 * 
 */
void CH_KEF_CZK_2004::SetUp(CH_KEF_CZK_2004_pp &pp) {    
    element_random(tmp_G);
    pp.set(g, tmp_G);
}

/**
 * @brief 
 * 
 * @param  pk[out]  
 * @param  sk[out]  
 * @param  pp[in]  
 * 
 */
void CH_KEF_CZK_2004::KeyGen(CH_KEF_CZK_2004_pk &pk, CH_KEF_CZK_2004_sk &sk, CH_KEF_CZK_2004_pp &pp) {  
    element_random(tmp_Zn);
    sk.set(x, tmp_Zn);

    element_pow_zn(tmp_G, pp[g], sk[x]);
    pk.set(y, tmp_G);
}
 

/**
 * @brief 
 * 
 * @param  h[out]    
 * @param  r[out]   
 * @param  m[in]    
 * @param  I[in]    
 * @param  pk[in]  
 * @param  pp[in]  
 * 
 */
void CH_KEF_CZK_2004::Hash(element_t h, CH_KEF_CZK_2004_r &r, element_t m, element_t I, CH_KEF_CZK_2004_pk &pk, CH_KEF_CZK_2004_pp &pp) {
    element_random(tmp_Zn);

    element_pow_zn(tmp_G, pp[g], tmp_Zn);
    r.set(r1, tmp_G);
    element_pow_zn(tmp_G_2, pk[y], tmp_Zn);
    r.set(r2, tmp_G_2);

    // compute h
    element_mul(this->tmp_G, pp[g], I);
    element_pow_zn(this->tmp_G, this->tmp_G, m);
    element_mul(h, this->tmp_G, tmp_G_2);
}

/**
 * @brief 
 * 
 * @param  I[in]    
 * @param  m[in]    
 * @param  r[in]   
 * @param  h[in]    
 * @param  pp[in]  
 * 
 * @return 
 */
bool CH_KEF_CZK_2004::Check(element_t I, element_t m, CH_KEF_CZK_2004_r &r, element_t h, CH_KEF_CZK_2004_pp &pp){
    element_mul(this->tmp_G, pp[g], I);
    element_pow_zn(this->tmp_G, this->tmp_G, m);
    element_mul(tmp_G_2, this->tmp_G, r[r2]);

    return element_cmp(h, tmp_G_2) == 0;
}

/**
 * @brief 
 * 
 * @param  r_p[out]  
 * @param  sk[in]   
 * @param  h[in]     
 * @param  m[in]     
 * @param  r[in]    
 * @param  m_p[in]   
 * @param  I[in]     
 * @param  pp[in]   
 * 
 */
void CH_KEF_CZK_2004::Adapt(CH_KEF_CZK_2004_r &r_p, CH_KEF_CZK_2004_sk &sk, element_t h, element_t m, CH_KEF_CZK_2004_r &r, element_t m_p, element_t I, CH_KEF_CZK_2004_pp &pp) {
    // compute r1_p
    element_sub(this->tmp_Zn, m, m_p);
    // x^-1
    element_invert(this->tmp_Zn_2, sk[x]);
    element_mul(this->tmp_Zn, this->tmp_Zn_2, this->tmp_Zn);
    element_mul(this->tmp_G, pp[g], I);
    element_pow_zn(this->tmp_G, this->tmp_G, this->tmp_Zn);
    element_mul(tmp_G, r[r1], this->tmp_G);
    r_p.set(r1, tmp_G);

    // compute r2_p
    element_sub(this->tmp_Zn, m, m_p);
    element_mul(this->tmp_G, pp[g], I);
    element_pow_zn(this->tmp_G, this->tmp_G, this->tmp_Zn);
    element_mul(tmp_G, r[r2], this->tmp_G);
    r_p.set(r2, tmp_G);
}

/**
 * @brief 
 * 
 * @param  I[in]     
 * @param  m_p[in]   
 * @param  r_p[in]  
 * @param  h[in]     
 * @param  pp[in]   
 * 
 * @return 
 */
bool CH_KEF_CZK_2004::Verify(element_t I, element_t m_p, CH_KEF_CZK_2004_r &r_p, element_t h, CH_KEF_CZK_2004_pp &pp) {
    return this->Check(I, m_p, r_p, h, pp);
}

CH_KEF_CZK_2004::~CH_KEF_CZK_2004() {
    element_clear(tmp_G);
    element_clear(tmp_G_2);
    element_clear(tmp_Zn);
    element_clear(tmp_Zn_2);
    element_clear(tmp_Zn_3);
    element_clear(tmp_Zn_4);

    element_clear(G1);
    element_clear(Zn);
}