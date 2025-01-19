#include <scheme/IB_CH_MD_LSX_2022.h>

IB_CH_MD_LSX_2022::IB_CH_MD_LSX_2022(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT) {
    this->G1 = _G1;
    this->G2 = _G2;
    this->Zn = _Zn;
    this->GT = _GT;

    element_init_same_as(this->g, *this->G1); 

    element_init_same_as(this->a, *this->Zn); 
    element_init_same_as(this->b, *this->Zn); 

    element_init_same_as(this->g1, *this->G1);
    element_init_same_as(this->g2, *this->G1);

    element_init_same_as(this->egg, *this->GT);
    element_init_same_as(this->eg2g, *this->GT);

    element_init_same_as(this->tmp_G1, *this->G1);  
    element_init_same_as(this->tmp_G2, *this->G2);
    element_init_same_as(this->tmp_G1_2, *this->G1); 

    element_init_same_as(this->tmp_GT, *this->GT);
    element_init_same_as(this->tmp_GT_2, *this->GT);
    element_init_same_as(this->tmp_GT_3, *this->GT);
    element_init_same_as(this->tmp_GT_hash, *this->GT);

    element_init_same_as(this->tmp_Zn, *this->Zn);  
    element_init_same_as(this->tmp_Zn_2, *this->Zn);  
    element_init_same_as(this->tmp_Zn_3, *this->Zn);
}

void IB_CH_MD_LSX_2022::PG() {
    element_random(this->g);  // G1生成元g, order p

    element_random(this->a);
    element_random(this->b);

    element_pow_zn(this->g1, this->g, this->a);
    element_pow_zn(this->g2, this->g, this->b);

    element_pairing(this->egg, this->g, this->g);
    element_pairing(this->eg2g, this->g2, this->g);
}

void IB_CH_MD_LSX_2022::KG(element_t *L, element_t *t, element_t *td1, element_t *td2) {
    element_set(*td1, *t);

    element_sub(this->tmp_Zn, this->b, *t);
    element_sub(this->tmp_Zn_2, this->a, *L);
    element_div(this->tmp_Zn_3, this->tmp_Zn, this->tmp_Zn_2);

    element_pow_zn(*td2, this->g, this->tmp_Zn_3);
}

void IB_CH_MD_LSX_2022::Hash(element_t *h, element_t *L, element_t *m, element_t *r_1, element_t *r_2) {
    element_pow_zn(this->tmp_GT, this->eg2g, *m);
    element_pow_zn(this->tmp_GT_2, this->egg, *r_1);
    //g1 / g^ID
    element_pow_zn(this->tmp_G1, this->g, *L);
    element_div(this->tmp_G1_2, this->g1, this->tmp_G1);
    element_pairing(this->tmp_GT_3, *r_2, this->tmp_G1_2);

    element_mul(*h, this->tmp_GT, this->tmp_GT_2);
    element_mul(*h, *h, this->tmp_GT_3);
}

void IB_CH_MD_LSX_2022::Forge(element_t *h, element_t *m, element_t *r_1, element_t *r_2, element_t *m_p, element_t *r_1_p, element_t *r_2_p, element_t *td1, element_t *td2) {
    element_sub(this->tmp_Zn, *m, *m_p);
    element_mul(this->tmp_Zn_2, this->tmp_Zn, *td1);
    element_add(*r_1_p, *r_1, this->tmp_Zn_2);

    element_sub(this->tmp_Zn, *m, *m_p);
    element_pow_zn(this->tmp_G1, *td2, this->tmp_Zn);
    element_mul(*r_2_p, *r_2, this->tmp_G1);
}

bool IB_CH_MD_LSX_2022::Verify(element_t *h, element_t *m_p, element_t *r_1_p, element_t *r_2_p, element_t *L) {
    this->Hash(&this->tmp_GT_hash,L,m_p,r_1_p,r_2_p);

    return element_cmp(*h, this->tmp_GT_hash) == 0;
}

IB_CH_MD_LSX_2022::~IB_CH_MD_LSX_2022() {
    element_clear(this->g);

    element_clear(this->a);
    element_clear(this->b);

    element_clear(this->g1);
    element_clear(this->g2);

    element_clear(this->egg);
    element_clear(this->eg2g);

    element_clear(this->tmp_G1);
    element_clear(this->tmp_G2);
    element_clear(this->tmp_G1_2);

    element_clear(this->tmp_GT);
    element_clear(this->tmp_GT_2);
    element_clear(this->tmp_GT_3);
    element_clear(this->tmp_GT_hash);
    
    element_clear(this->tmp_Zn);
    element_clear(this->tmp_Zn_2);
    element_clear(this->tmp_Zn_3);
}