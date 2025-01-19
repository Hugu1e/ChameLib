#include <scheme/CH_KEF_DLP_LLA_2012.h>

CH_KEF_DLP_LLA_2012::CH_KEF_DLP_LLA_2012(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT) {
    this->G1 = _G1;
    this->G2 = _G2;
    this->Zn = _Zn;
    this->GT = _GT;

    element_init_same_as(this->tmp_G1, *this->G1);
    element_init_same_as(this->tmp_G1_2, *this->G1);  
    element_init_same_as(this->tmp_G2, *this->G2);
    element_init_same_as(this->tmp_Zn, *this->Zn);  
    element_init_same_as(this->tmp_Zn_2, *this->Zn);  
    element_init_same_as(this->tmp_GT, *this->GT);
    element_init_same_as(this->tmp_GT_2, *this->GT);
    element_init_same_as(this->tmp_GT_3, *this->GT);

    element_init_same_as(this->g, *this->G1); 
    element_init_same_as(this->y, *this->G1);
    element_init_same_as(this->y1, *this->G1);
    element_init_same_as(this->w1, *this->G1);
    element_init_same_as(this->t, *this->G1);


}

/**
 * input : m1,m2,m3
 * output: res
 */
void CH_KEF_DLP_LLA_2012::H1(element_t *m1, element_t *m2, element_t *m3, element_t *res) {
    Hm_4(*m1, *m2, *m3, *res);
}

/**
 * input : m
 * output: res
 */
void CH_KEF_DLP_LLA_2012::H2(element_t *m, element_t *res) {
    Hm_1(*m, *res);
}

/**
 * input : 
 * output: 
 */
void CH_KEF_DLP_LLA_2012::PG() {

}

/**
 * input : 
 * output: sk, pk, label
 */
void CH_KEF_DLP_LLA_2012::KG(sk *sk, pk *pk, label *label) {  
    element_random(this->g);
    
    element_random(sk->a);
    element_random(sk->x1);
    element_random(sk->x2);

    // y = g^a
    element_pow_zn(this->y, this->g, sk->a);

    // y1 = g^x1
    element_pow_zn(this->y1, this->g, sk->x1);
    // y2 = g^x2
    element_pow_zn(pk->y2, this->g, sk->x2);
    // w1 = y^x1
    element_pow_zn(this->w1, this->y, sk->x1);

    // obtain a label
    element_random(this->t);
    LabelManager(&this->y1, &this->w1, &this->t, label);
}

/**
 * input: y1, w1, t
 * output: label
 */
void CH_KEF_DLP_LLA_2012::LabelManager(element_t *y1, element_t *w1, element_t *t, label *label) {
    // H2(t)
    this->H2(t, &this->tmp_Zn);
    // L = y1^H2(t)
    element_pow_zn(label->L, *y1, this->tmp_Zn);

    // R = t*(w1^H2(t))
    element_pow_zn(this->tmp_G1, *w1, this->tmp_Zn);
    element_mul(label->R, *t, this->tmp_G1);
}
 

/**
 * input : pk, m, r, lable
 * output: S
 */
void CH_KEF_DLP_LLA_2012::Hash(pk *pk, element_t *m, element_t *r, label *label, element_t *S) {
    element_random(*r);
    
    // c = H1(label, L)
    this->H1(&label->L, &label->R, &label->L, &this->tmp_Zn);
    // y2^c
    element_pow_zn(this->tmp_G1, pk->y2, this->tmp_Zn);
    // L * (y2^c)
    element_mul(this->tmp_G1, label->L, this->tmp_G1);
    // (L * (y2^c)) ^ r
    element_pow_zn(this->tmp_G1, this->tmp_G1, *r);
    // g^m
    element_pow_zn(this->tmp_G1_2, this->g, *m);
    // S = (g^m) * ((L * (y2^c)) ^ r)
    element_mul(*S, this->tmp_G1_2, this->tmp_G1);   
}

/**
 * input : m, r, pk, lable, S
 * output: bool
 */
bool CH_KEF_DLP_LLA_2012::Check(element_t *m, element_t *r, pk *pk, label *label, element_t *S) {
    // c = H1(label, L)
    this->H1(&label->L, &label->R, &label->L, &this->tmp_Zn);
    // y2^c
    element_pow_zn(this->tmp_G1, pk->y2, this->tmp_Zn);
    // L * (y2^c)
    element_mul(this->tmp_G1, label->L, this->tmp_G1);
    // (L * (y2^c)) ^ r
    element_pow_zn(this->tmp_G1, this->tmp_G1, *r);
    // g^m
    element_pow_zn(this->tmp_G1_2, this->g, *m);
    // S = (g^m) * ((L * (y2^c)) ^ r)
    element_mul(this->tmp_G1, this->tmp_G1_2, this->tmp_G1);   
    // compare
    return element_cmp(*S, this->tmp_G1) == 0;
}

/**
 * input: sk,pk,label, S, m, m_p, r
 * output: r_p
 */
void CH_KEF_DLP_LLA_2012::UForge(sk *sk,pk *pk,label *label, element_t *S, element_t *m, element_t *m_p, element_t *r, element_t *r_p) {
    // check
    if(!this->Check(m, r, pk, label, S)){ 
        throw std::invalid_argument("UForge failed, S is not correct");
    }
    // t = R / (L^a)
    element_pow_zn(this->tmp_G1, label->L, sk->a);
    element_div(this->t, label->R, this->tmp_G1);
    
    // XXX
    // check if y1^H2(t) = L
    this->H2(&this->t, &this->tmp_Zn);
    element_pow_zn(this->tmp_G1, this->y1, this->tmp_Zn);

    if(element_cmp(this->tmp_G1, label->L) != 0){
        throw std::invalid_argument("UForge failed, label is not correct");
    }

    // c = H1(label, L)
    this->H1(&label->L, &label->R, &label->L, &this->tmp_Zn_2);
    // s = x1 *H2(t) + x2 *c
    element_mul(this->tmp_Zn, sk->x1, this->tmp_Zn);
    element_mul(this->tmp_Zn_2, sk->x2, this->tmp_Zn_2);
    element_add(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    
    // r_p = s^-1(m-m_p) + r
    element_sub(this->tmp_Zn_2, *m, *m_p);
    element_invert(this->tmp_Zn, this->tmp_Zn);
    element_mul(this->tmp_Zn_2, this->tmp_Zn, this->tmp_Zn_2);
    element_add(*r_p, this->tmp_Zn_2, *r);
}

/**
 * input: label, m, m_p, r,r_p, m_pp
 * output: r_pp
 */
void CH_KEF_DLP_LLA_2012::IForge(label *label, element_t *m, element_t *m_p, element_t *r, element_t *r_p, element_t *m_pp, element_t *r_pp){
    // s = (m-m')/(r'-r)
    element_sub(this->tmp_Zn, *m, *m_p);
    element_sub(this->tmp_Zn_2, *r_p, *r);
    element_div(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);

    // r'' = (s^-1)(m'-m'') + r'
    element_sub(this->tmp_Zn_2, *m_p, *m_pp);
    element_invert(this->tmp_Zn, this->tmp_Zn);
    element_mul(this->tmp_Zn_2, this->tmp_Zn, this->tmp_Zn_2);
    element_add(*r_pp, this->tmp_Zn_2, *r_p);
}

/**
 * input : m_p, r_p, pk, label, S
 * output: bool
 */
bool CH_KEF_DLP_LLA_2012::Verify(element_t *m_p, element_t *r_p, pk *pk, label *label, element_t *S) {
    return this->Check(m_p, r_p, pk, label, S);
    
}

CH_KEF_DLP_LLA_2012::~CH_KEF_DLP_LLA_2012() {
    element_clear(this->tmp_G1);
    element_clear(this->tmp_G1_2);
    element_clear(this->tmp_G2);
    element_clear(this->tmp_Zn);
    element_clear(this->tmp_Zn_2);
    element_clear(this->tmp_GT);
    element_clear(this->tmp_GT_2);
    element_clear(this->tmp_GT_3);


    element_clear(this->g);
    element_clear(this->y);
    element_clear(this->y1);
    element_clear(this->w1);
    element_clear(this->t);

}