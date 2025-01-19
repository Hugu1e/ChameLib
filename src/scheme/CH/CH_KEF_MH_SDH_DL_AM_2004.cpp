#include <scheme/CH_KEF_MH_SDH_DL_AM_2004.h>

void CH_KEF_MH_SDH_DL_AM_2004::H(element_t *m, element_t *res) {
    Hm(*m, *res, this->tmp_Zn, *this->G1);
}

CH_KEF_MH_SDH_DL_AM_2004::CH_KEF_MH_SDH_DL_AM_2004(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT) {
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
    element_init_same_as(this->x, *this->Zn);
    element_init_same_as(this->a, *this->Zn);

}

/**
 * input : 
 * output: 
 */
void CH_KEF_MH_SDH_DL_AM_2004::PG() {
    element_random(this->g);
}

/**
 * input : 
 * output: y
 */
void CH_KEF_MH_SDH_DL_AM_2004::KG(element_t *y) {  
    element_pow_zn(*y, this->g, this->x);
}
 

/**
 * input : label, m, r, y
 * output: h
 */
void CH_KEF_MH_SDH_DL_AM_2004::Hash(element_t *label, element_t *m, element_t *r, element_t *y, element_t *h) {
    // H(m)
    this->H(m, &this->tmp_Zn);
    // e = H(label)
    this->H(label, &this->tmp_Zn_2);
    // g^H(m)
    element_pow_zn(this->tmp_G1, this->g, this->tmp_Zn);
    // g^e
    element_pow_zn(this->tmp_G1_2, this->g, this->tmp_Zn_2);
    // g^e * y
    element_mul(this->tmp_G1_2, this->tmp_G1_2, *y);
    // (g^e * y)^r
    element_pow_zn(this->tmp_G1_2, this->tmp_G1_2, *r);
    // h = g^H(m) * (g^e * y)^r
    element_mul(*h, this->tmp_G1, this->tmp_G1_2);
    element_printf("h = %B\n", *h);
}

/**
 * input : (this->x) ,h, m, label, r, m_p
 * output: r_p
 */
void CH_KEF_MH_SDH_DL_AM_2004::Forge(element_t *h, element_t *m, element_t *label, element_t *r, element_t *m_p, element_t *r_p) {
    // H(m) - H(m')
    this->H(m, &this->tmp_Zn);
    this->H(m_p, &this->tmp_Zn_2);
    element_sub(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    // x + e
    this->H(label, &this->tmp_Zn_2);
    element_add(this->tmp_Zn_2, this->x, this->tmp_Zn_2);
    // ( H(m) - H(m')) / (x + e)
    element_div(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    // r_p = r + ( H(m) - H(m')) / (x + e)
    element_add(*r_p, *r, this->tmp_Zn);
    element_printf("r_p = %B\n", *r_p);
}

/**
 * input : label, m_p, r_p, y, h
 * output: bool
 */
bool CH_KEF_MH_SDH_DL_AM_2004::Verify(element_t *label, element_t *m_p, element_t *r_p, element_t *y, element_t *h) {
    this->Hash(label, m_p, r_p, y, &this->tmp_G1);
    return element_cmp(*h, this->tmp_G1) == 0;
}

CH_KEF_MH_SDH_DL_AM_2004::~CH_KEF_MH_SDH_DL_AM_2004() {
    element_clear(this->tmp_G1);
    element_clear(this->tmp_G1_2);
    element_clear(this->tmp_G2);
    element_clear(this->tmp_Zn);
    element_clear(this->tmp_Zn_2);
    element_clear(this->tmp_GT);
    element_clear(this->tmp_GT_2);
    element_clear(this->tmp_GT_3);


    element_clear(this->g);
    element_clear(this->a);
    element_clear(this->x);
}