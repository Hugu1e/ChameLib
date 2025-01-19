#include <scheme/IB_CH_KEF_CZS_2014.h>

/**
 * input : (y,h,m),(u11,u12,u2)
 * output: res
 */
void IB_CH_KEF_CZS_2014::H(element_t *m, element_t *res) {
    Hm_1(*m, *res);
}

IB_CH_KEF_CZS_2014::IB_CH_KEF_CZS_2014(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT) {
    this->G1 = _G1;
    this->G2 = _G2;
    this->Zn = _Zn;
    this->GT = _GT;

    element_init_same_as(this->tmp_G1, *this->G1);
    element_init_same_as(this->tmp_G1_2, *this->G1);  
    element_init_same_as(this->tmp_G1_3, *this->G1);  
    element_init_same_as(this->tmp_G1_4, *this->G1);
    element_init_same_as(this->tmp_G2, *this->G2);
    element_init_same_as(this->tmp_Zn, *this->Zn);  
    element_init_same_as(this->tmp_Zn_2, *this->Zn);  
    element_init_same_as(this->tmp_GT, *this->GT);
    element_init_same_as(this->tmp_GT_2, *this->GT);
    element_init_same_as(this->tmp_GT_3, *this->GT);
    

    element_init_same_as(this->P, *this->G1);
    element_init_same_as(this->x, *this->Zn);
    element_init_same_as(this->Ppub, *this->G1);
    element_init_same_as(this->SID, *this->G1);
    element_init_same_as(this->QID, *this->G1);
    element_init_same_as(this->a, *this->Zn);
    
}

/**
 * input : 
 * output: x, Ppub
 */
void IB_CH_KEF_CZS_2014::PG(element_t *_x, element_t *_Ppub) {
    element_random(this->P);
    element_printf("P = %B\n", this->P);
    element_random(this->x);
    element_printf("x = %B\n", this->x);
    // Ppub = x * P
    element_mul_zn(this->Ppub, this->P, this->x);
    element_printf("Ppub = %B\n", this->Ppub);

    element_set(*_x, this->x);
    element_set(*_Ppub, this->Ppub);
}

/**
 * input : x, ID
 * output: SID
 */
void IB_CH_KEF_CZS_2014::KG(element_t *_x, element_t *ID, element_t *_SID) {  
    // QID = H(ID)
    this->H(ID, &this->QID);
    // SID = x * QID
    element_mul_zn(this->SID, this->QID, *_x);
    element_set(*_SID, this->SID);
    element_printf("SID = %B\n", *_SID);
}
 

/**
 * input : ID, L, m
 * output: r(r1,r2), h
 */
void IB_CH_KEF_CZS_2014::Hash(element_t *ID, element_t *L, element_t *m, element_t *r1, element_t *r2, element_t *h) {
    // r1 = a * P
    element_random(this->a);
    element_mul_zn(*r1, this->P, this->a);
    element_printf("r1 = %B\n", *r1);
    // r2 = e(a * Ppub, QID)
    element_mul_zn(this->tmp_G1, this->Ppub, this->a);
    element_pairing(*r2, this->tmp_G1, this->QID);
    element_printf("r2 = %B\n", *r2);
    // h = a * P + m * H(L)
    this->H(L, &this->tmp_G1);
    element_mul_zn(this->tmp_G1, this->tmp_G1, *m);
    element_add(*h, *r1, this->tmp_G1);
    element_printf("h = %B\n", *h);

    // check the correctness of the r
    // e(a * P,SID) == e(a * Ppub, QID)
    element_pairing(this->tmp_GT, *r1, this->SID);
    element_mul_zn(this->tmp_G1_2, this->Ppub, this->a);
    element_pairing(this->tmp_GT_2, this->tmp_G1_2, this->QID);
    if(element_cmp(this->tmp_GT, this->tmp_GT_2) == 0){
        printf("Hash success\n");
    }
    else{
        printf("Hash failed, r is invaid\n");
    }

}

/**
 * input : h, L, m, r1
 * output: bool
 */
bool IB_CH_KEF_CZS_2014::Check(element_t *h, element_t *L,element_t *m, element_t *r1){
    // h = r1 + m * H(L)
    this->H(L, &this->tmp_G1);
    element_mul_zn(this->tmp_G1, this->tmp_G1, *m);
    element_add(this->tmp_G1, *r1, this->tmp_G1);

    return element_cmp(*h, this->tmp_G1) == 0;    
}

/**
 * input : SID, ID, L, h, m, r1, r2, m_p
 * output: r_p(r1_p, r2_p)
 */
void IB_CH_KEF_CZS_2014::Forge(element_t *_SID, element_t *ID, element_t *L, element_t *h, element_t *m, element_t *r1, element_t *r2, element_t *m_p, 
                                element_t *r1_p, element_t *r2_p) {
    // r1_p = r1 + (m - m_p) * H(L)
    element_sub(this->tmp_Zn, *m, *m_p);
    this->H(L, &this->tmp_G1);
    element_mul_zn(this->tmp_G1_2, this->tmp_G1, this->tmp_Zn);
    element_add(*r1_p, *r1, this->tmp_G1_2);
    element_printf("r1_p = %B\n", *r1_p);
    
    // r2_p = r2 * e(SID, H(L))^(m-m_p)
    element_pairing(this->tmp_GT, *_SID, this->tmp_G1);
    element_pow_zn(this->tmp_GT_2, this->tmp_GT, this->tmp_Zn);
    element_mul(*r2_p, *r2, this->tmp_GT_2);
    element_printf("r2_p = %B\n", *r2_p);

    // check the correctness of the r_p
    // e(r1_p, SID) == r2_p
    element_pairing(this->tmp_GT_3, *r1_p, *_SID);
    if(element_cmp(this->tmp_GT_3, *r2_p) == 0){
        printf("Forge success\n");
    }
    else{
        printf("Forge failed, r_p is invaid\n");
    }
}

/**
 * input : h, L, m_p, r1_p
 * output: bool
 */
bool IB_CH_KEF_CZS_2014::Verify(element_t *h, element_t *L,element_t *m_p, element_t *r1_p) {
    return this->Check(h, L, m_p, r1_p);
}


IB_CH_KEF_CZS_2014::~IB_CH_KEF_CZS_2014() {
    element_clear(this->tmp_G1);
    element_clear(this->tmp_G1_2);
    element_clear(this->tmp_G1_3);
    element_clear(this->tmp_G1_4);
    element_clear(this->tmp_G2);
    element_clear(this->tmp_Zn);
    element_clear(this->tmp_Zn_2);
    element_clear(this->tmp_GT);
    element_clear(this->tmp_GT_2);
    element_clear(this->tmp_GT_3);

    element_clear(this->P);
    element_clear(this->x);
    element_clear(this->Ppub);
    element_clear(this->SID);
    element_clear(this->QID);
    element_clear(this->a);
}