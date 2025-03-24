#include "scheme/CH/CH_KEF_DLP_LLA_2012.h"

CH_KEF_DLP_LLA_2012::CH_KEF_DLP_LLA_2012(int curve, int group): PbcScheme(curve) {
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
}

/**
 * @brief 
 * 
 * @param  pp[out]  
 * 
 */
void CH_KEF_DLP_LLA_2012::SetUp(CH_KEF_DLP_LLA_2012_pp &pp) {    
    element_random(tmp_G);
    pp.set(g, tmp_G);
}

/**
 * @brief 
 * 
 * @param  sk[out]  
 * @param  pk[out]  
 * @param  lm[out]  
 * @param  pp[in]  
 * 
 */
void CH_KEF_DLP_LLA_2012::KeyGen(CH_KEF_DLP_LLA_2012_sk &sk, CH_KEF_DLP_LLA_2012_pk &pk, CH_KEF_DLP_LLA_2012_labelManager &lm, CH_KEF_DLP_LLA_2012_pp &pp) {  
    // alpha
    element_random(tmp_Zn);
    sk.set(alpha, tmp_Zn);

    // x_1
    element_random(tmp_Zn);
    sk.set(x1, tmp_Zn);

    // x_2
    element_random(tmp_Zn);
    sk.set(x2, tmp_Zn);

    CH_KEF_DLP_LLA_2012_labelGen* lg = new CH_KEF_DLP_LLA_2012_labelGen();

    // lg.y_1 = g^x_1
    element_pow_zn(tmp_G, pp[g], sk[x1]);
    lg->set(y_1, tmp_G);

    // lg.omega_1 = y_1^alpha
    element_pow_zn(tmp_G, lg->get(y_1), sk[alpha]);
    lg->set(omega_1, tmp_G);

    // y_2 = g^x_2
    element_pow_zn(tmp_G, pp[g], sk[x2]);
    pk.set(y2, tmp_G);
  
    lm.add(pk, lg);
}

void CH_KEF_DLP_LLA_2012::getLabel(CH_KEF_DLP_LLA_2012_label &label, CH_KEF_DLP_LLA_2012_labelManager &lm, CH_KEF_DLP_LLA_2012_pk &pk){
    // t
    element_random(tmp_G);
    // H2(t)
    H2(tmp_Zn, tmp_G);

    CH_KEF_DLP_LLA_2012_labelGen *lg = lm.get_lg(pk.toString());

    // L = y1^H2(t)
    element_pow_zn(tmp_G_2, lg->get(y_1), tmp_Zn);
    label.set(L, tmp_G_2);

    // R = t*(w1^H2(t))
    element_pow_zn(tmp_G_2, lg->get(omega_1), tmp_Zn);
    element_mul(tmp_G, tmp_G, tmp_G_2);
    label.set(R, tmp_G);
}

void CH_KEF_DLP_LLA_2012::H1(element_t res, element_t m1, element_t m2, element_t m3) {
    HASH::hash(res, m1, m2, m3);
}

/**
 * input : m
 * output: res
 */
void CH_KEF_DLP_LLA_2012::H2(element_t res, element_t m) {
    HASH::hash(res, m);
}

/**
 * @brief 
 * 
 * @param  h[out]       
 * @param  r[out]       
 * @param  label[out]  
 * @param  pk[in]     
 * @param  m[in]       
 * @param  lm[in]     
 * @param  pp[in]     
 * 
 */
void CH_KEF_DLP_LLA_2012::Hash(element_t h, element_t r, CH_KEF_DLP_LLA_2012_label &label, CH_KEF_DLP_LLA_2012_pk &pk, element_t m, CH_KEF_DLP_LLA_2012_labelManager &lm, CH_KEF_DLP_LLA_2012_pp &pp) {
    getLabel(label, lm, pk);
    
    element_random(r);
    
    // c = H1(label, L)
    this->H1(this->tmp_Zn, label[L], label[R], label[L]);
    // y2^c
    element_pow_zn(this->tmp_G, pk[y2], this->tmp_Zn);
    // L * (y2^c)
    element_mul(this->tmp_G, label[L], this->tmp_G);
    // (L * (y2^c)) ^ r
    element_pow_zn(this->tmp_G, this->tmp_G, r);
    // g^m
    element_pow_zn(this->tmp_G_2, pp[g], m);
    // S = (g^m) * ((L * (y2^c)) ^ r)
    element_mul(h, this->tmp_G_2, this->tmp_G);   
}

/**
 * @brief 
 * 
 * @param  m[in]       
 * @param  r[in]       
 * @param  pk[in]     
 * @param  label[in]  
 * @param  h[in]       
 * @param  pp[in]     
 * 
 * @return 
 */
bool CH_KEF_DLP_LLA_2012::Check(element_t m, element_t r, CH_KEF_DLP_LLA_2012_pk &pk, CH_KEF_DLP_LLA_2012_label &label, element_t h, CH_KEF_DLP_LLA_2012_pp &pp) {
    // c = H1(label, L)
    this->H1(this->tmp_Zn, label[L], label[R], label[L]);
    // y2^c
    element_pow_zn(this->tmp_G, pk[y2], this->tmp_Zn);
    // L * (y2^c)
    element_mul(this->tmp_G, label[L], this->tmp_G);
    // (L * (y2^c)) ^ r
    element_pow_zn(this->tmp_G, this->tmp_G, r);
    // g^m
    element_pow_zn(this->tmp_G_2, pp[g], m);
    // S = (g^m) * ((L * (y2^c)) ^ r)
    element_mul(this->tmp_G, this->tmp_G_2, this->tmp_G);   
    // compare
    return element_cmp(h, this->tmp_G) == 0;
}

/**
 * @brief 
 * 
 * @param  r_p[out]     
 * @param  sk[in]     
 * @param  pk[in]     
 * @param  label[in]  
 * @param  h[in]       
 * @param  m[in]       
 * @param  m_p[in]     
 * @param  r[in]       
 * @param  pp[in]     
 * 
 */
void CH_KEF_DLP_LLA_2012::UForge(element_t r_p, CH_KEF_DLP_LLA_2012_sk &sk, CH_KEF_DLP_LLA_2012_pk &pk, CH_KEF_DLP_LLA_2012_label &label, element_t h, element_t m, element_t m_p, element_t r, CH_KEF_DLP_LLA_2012_pp &pp) {
    // check
    if(!this->Check(m, r, pk, label, h, pp)){ 
        throw std::invalid_argument("UForge failed, h is not correct");
    }
    // t = R / (L^alpha)
    element_pow_zn(this->tmp_G, label[L], sk[alpha]);
    element_div(tmp_G_2, label[R], this->tmp_G);
    
    // check if y1^H2(t) = L
    this->H2(this->tmp_Zn, tmp_G_2);
    element_mul(tmp_Zn_2, tmp_Zn, sk[x1]);
    element_pow_zn(this->tmp_G, pp[g], this->tmp_Zn_2);

    if(element_cmp(this->tmp_G, label[L]) != 0){
        throw std::invalid_argument("UForge failed, label is not correct");
    }

    // c = H1(label, L)
    this->H1(this->tmp_Zn_2, label[L], label[R], label[L]);
    // s = x1 *H2(t) + x2 *c
    element_mul(this->tmp_Zn, sk[x1], this->tmp_Zn);
    element_mul(this->tmp_Zn_2, sk[x2], this->tmp_Zn_2);
    element_add(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    
    // r_p = s^-1(m-m_p) + r
    element_sub(this->tmp_Zn_2, m, m_p);
    element_invert(this->tmp_Zn, this->tmp_Zn);
    element_mul(this->tmp_Zn_2, this->tmp_Zn, this->tmp_Zn_2);
    element_add(r_p, this->tmp_Zn_2, r);
}

/**
 * @brief 
 * 
 * @param  r_pp[out]    
 * @param  label[in]  
 * @param  m[in]       
 * @param  m_p[in]     
 * @param  r[in]       
 * @param  r_p[in]     
 * @param  m_pp[in]    
 * 
 */
void CH_KEF_DLP_LLA_2012::IForge(element_t r_pp, CH_KEF_DLP_LLA_2012_label &label, element_t m, element_t m_p, element_t r, element_t r_p, element_t m_pp){
    // s = (m-m')/(r'-r)
    element_sub(this->tmp_Zn, m, m_p);
    element_sub(this->tmp_Zn_2, r_p, r);
    element_div(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);

    // r'' = (s^-1)(m'-m'') + r'
    element_sub(this->tmp_Zn_2, m_p, m_pp);
    element_invert(this->tmp_Zn, this->tmp_Zn);
    element_mul(this->tmp_Zn_2, this->tmp_Zn, this->tmp_Zn_2);
    element_add(r_pp, this->tmp_Zn_2, r_p);
}

/**
 * @brief 
 * 
 * @param  m_p[in]     
 * @param  r_p[in]     
 * @param  pk[in]     
 * @param  label[in]  
 * @param  h[in]       
 * @param  pp[in]     
 * 
 * @return 
 */
bool CH_KEF_DLP_LLA_2012::Verify(element_t m_p, element_t r_p, CH_KEF_DLP_LLA_2012_pk &pk, CH_KEF_DLP_LLA_2012_label &label, element_t h, CH_KEF_DLP_LLA_2012_pp &pp) {
    return this->Check(m_p, r_p, pk, label, h, pp);
}

CH_KEF_DLP_LLA_2012::~CH_KEF_DLP_LLA_2012() {
    element_clear(tmp_G);
    element_clear(tmp_G_2);
    element_clear(tmp_Zn);
    element_clear(tmp_Zn_2);
    element_clear(G1);
    element_clear(Zn);
}