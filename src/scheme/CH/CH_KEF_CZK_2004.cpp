#include <scheme/CH_KEF_CZK_2004.h>

CH_KEF_CZK_2004::CH_KEF_CZK_2004(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT) {
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
void CH_KEF_CZK_2004::PG() {
    element_random(this->g);
}

/**
 * input : 
 * output: y
 */
void CH_KEF_CZK_2004::KG(element_t *y) {  
    element_pow_zn(*y, this->g, this->x);
}
 

/**
 * input : I, m, y
 * output: h, r1, r2
 */
void CH_KEF_CZK_2004::Hash(element_t *I, element_t *m, element_t *y, element_t *h, element_t *r1, element_t *r2) {
    element_random(this->a);

    element_pow_zn(*r1, this->g, this->a);
    element_pow_zn(*r2, *y, this->a);
    element_printf("r1 = %B\n", *r1);
    element_printf("r2 = %B\n", *r2);

    // compute h
    element_mul(this->tmp_G1, this->g, *I);
    element_pow_zn(this->tmp_G1, this->tmp_G1, *m);
    element_mul(*h, this->tmp_G1, *r2);
    element_printf("h = %B\n", *h);
}

/**
 * input : I, m, r1, r2
 * output: h
 */
void CH_KEF_CZK_2004::hash_with_r(element_t *I, element_t *m, element_t *r1, element_t *r2, element_t *h) {
    // compute h
    element_mul(this->tmp_G1, this->g, *I);
    element_pow_zn(this->tmp_G1, this->tmp_G1, *m);
    element_mul(*h, this->tmp_G1, *r2);
}

/**
 * input : (this->x) ,h, m, r1, r2, m_p, I
 * output: r1_p, r2_p
 */
void CH_KEF_CZK_2004::Forge(element_t *h, element_t *m, element_t *r1, element_t *r2, element_t *m_p, element_t *I,
                            element_t *r1_p, element_t *r2_p) {
    // compute r1_p
    element_sub(this->tmp_Zn, *m, *m_p);
    // x^-1
    element_invert(this->tmp_Zn_2, this->x);
    element_mul(this->tmp_Zn, this->tmp_Zn_2, this->tmp_Zn);
    element_mul(this->tmp_G1, this->g, *I);
    element_pow_zn(this->tmp_G1, this->tmp_G1, this->tmp_Zn);
    element_mul(*r1_p, *r1, this->tmp_G1);
    element_printf("r1_p = %B\n", *r1_p);

    // compute r2_p
    element_sub(this->tmp_Zn, *m, *m_p);
    element_mul(this->tmp_G1, this->g, *I);
    element_pow_zn(this->tmp_G1, this->tmp_G1, this->tmp_Zn);
    element_mul(*r2_p, *r2, this->tmp_G1);
    element_printf("r2_p = %B\n", *r2_p);
}

/**
 * input : I, m_p, r1_p, r2_p, h
 * output: bool
 */
bool CH_KEF_CZK_2004::Verify(element_t *I, element_t *m_p, element_t *r1_p, element_t *r2_p, element_t *h) {
    hash_with_r(I, m_p, r1_p, r2_p, &this->tmp_G1);
    return element_cmp(*h, this->tmp_G1) == 0;
}

CH_KEF_CZK_2004::~CH_KEF_CZK_2004() {
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