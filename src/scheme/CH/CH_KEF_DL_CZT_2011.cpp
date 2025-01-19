#include <scheme/CH_KEF_DL_CZT_2011.h>

void CH_KEF_DL_CZT_2011::H(element_t *gs, element_t *m, element_t *res) {
    Hgsm_1(*gs,*m,*res);  
}

// void CH_KEF_DL_CZT_2011::H_G1(element_t *m, element_t *res) {
//     Hm(*m, *res, this->tmp_Zn, *this->G1);
// }

CH_KEF_DL_CZT_2011::CH_KEF_DL_CZT_2011(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT) {
    this->G1 = _G1;
    this->G2 = _G2;
    this->Zn = _Zn;
    this->GT = _GT;

    element_init_same_as(this->P_pub, *this->G1);
    element_init_same_as(this->tmp_G1, *this->G1);  
    element_init_same_as(this->tmp_G2, *this->G2);
    element_init_same_as(this->tmp_G1_2, *this->G1); 
    element_init_same_as(this->tmp_GT, *this->GT);
    element_init_same_as(this->tmp_Zn, *this->Zn);  
    element_init_same_as(this->tmp_Zn_2, *this->Zn);  
    element_init_same_as(this->tmp_G1_hash, *this->G1);

    element_init_same_as(this->g, *this->G1); 

    element_init_same_as(this->tmp_h, *this->G1);

}

void CH_KEF_DL_CZT_2011::PG() {
    element_random(this->g);  // G1生成元g, order q
}

void CH_KEF_DL_CZT_2011::KG(element_t *x, element_t *y) {
    element_pow_zn(*y, this->g, *x);
}


void CH_KEF_DL_CZT_2011::Hash(element_t *L, element_t *m, element_t *r_1, element_t *r_2, element_t *a, element_t *y, element_t *h) {
    element_pow_zn(*r_1, this->g, *a);
    element_pow_zn(*r_2, *y, *a);
   
    // L: indentity
    this->H(y, L, &this->tmp_h);
        
    element_pow_zn(this->tmp_G1, this->tmp_h, *m);
    element_mul(*h, *r_1, this->tmp_G1);
}

void CH_KEF_DL_CZT_2011::Forge(element_t *h, element_t *x, element_t *L, element_t *m, element_t *m_p, element_t *r_1, element_t *r_2, element_t *r_1_p, element_t *r_2_p) {
    // y
    element_pow_zn(this->tmp_G1, this->g, *x);
    // tmp_h
    this->H(&this->tmp_G1, L, &this->tmp_h);
    // g^a  r_1
    // h^(m-m')
    element_sub(this->tmp_Zn, *m, *m_p);
    element_pow_zn(this->tmp_G1_2, this->tmp_h, this->tmp_Zn);
    element_mul(*r_1_p, *r_1, this->tmp_G1_2);
    // y^a r_2
    // x(m-m')
    // element_sub(this->tmp_Zn, *m, *m_p);
    element_mul(this->tmp_Zn_2, *x, this->tmp_Zn);
    element_pow_zn(this->tmp_G1, this->tmp_h, this->tmp_Zn_2);
    element_mul(*r_2_p, *r_2, this->tmp_G1);
}

bool CH_KEF_DL_CZT_2011::Verify(element_t *h, element_t *L,element_t *m_p, element_t *r_1_p, element_t *x) {
    // g^a' r_1_p
    // h^m'
    // y
    element_pow_zn(this->tmp_G1, this->g, *x);
    // tmp_h
    this->H(&this->tmp_G1, L, &this->tmp_h);

    element_pow_zn(this->tmp_G1_2, this->tmp_h, *m_p);

    element_mul(this->tmp_G1_hash, *r_1_p, this->tmp_G1_2);

    return element_cmp(*h, this->tmp_G1_hash) == 0;

}

CH_KEF_DL_CZT_2011::~CH_KEF_DL_CZT_2011() {
    element_clear(this->g);
    element_clear(this->tmp_h);
    element_clear(this->P_pub);
    element_clear(this->tmp_G1);
    element_clear(this->tmp_G2);
    element_clear(this->tmp_G1_2);
    element_clear(this->tmp_GT);
    element_clear(this->tmp_Zn);
    element_clear(this->tmp_Zn_2);
    element_clear(this->tmp_G1_hash);
}