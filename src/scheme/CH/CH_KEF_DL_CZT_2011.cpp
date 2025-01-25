#include <scheme/CH/CH_KEF_DL_CZT_2011.h>

CH_KEF_DL_CZT_2011::CH_KEF_DL_CZT_2011(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn) : PbcScheme(_G1, _G2, _GT, _Zn) {}

void CH_KEF_DL_CZT_2011::SetUp(CH_KEF_DL_CZT_2011_pp *pp){
    element_random(tmp_G);
    pp->insertElement("g", "G1", tmp_G);
}

void CH_KEF_DL_CZT_2011::KeyGen(CH_KEF_DL_CZT_2011_pk *pk, CH_KEF_DL_CZT_2011_sk *sk, CH_KEF_DL_CZT_2011_pp *pp) {
    element_random(tmp_Zn);
    sk->insertElement("x", "Zn", tmp_Zn);

    element_pow_zn(tmp_G, pp->getElement("g"), tmp_Zn);
    pk->insertElement("y", "G1", tmp_G);
}

void CH_KEF_DL_CZT_2011::H(element_t res, element_t m1, element_t m2) {
    HASH::hash(res, m1, m2);
}

void CH_KEF_DL_CZT_2011::Hash(element_t h, CH_KEF_DL_CZT_2011_r *r, element_t I, element_t m, CH_KEF_DL_CZT_2011_pk *pk, CH_KEF_DL_CZT_2011_pp *pp) {
    element_random(tmp_Zn);
    
    element_pow_zn(tmp_G, pp->getElement("g"), tmp_Zn);
    r->insertElement("r1", "G1", tmp_G);
    element_pow_zn(tmp_G_3, pk->getElement("y"), tmp_Zn);
    r->insertElement("r2", "G1", tmp_G_3);
   

    this->H(this->tmp_G_3, pk->getElement("y"), I);
        
    element_pow_zn(this->tmp_G_2, this->tmp_G_3, m);
    element_mul(h, tmp_G, this->tmp_G_2);
}

bool CH_KEF_DL_CZT_2011::Check(element_t h, CH_KEF_DL_CZT_2011_r *r, element_t I, element_t m, CH_KEF_DL_CZT_2011_pk *pk, CH_KEF_DL_CZT_2011_pp *pp){
    this->H(this->tmp_G, pk->getElement("y"), I);
        
    element_pow_zn(this->tmp_G_2, this->tmp_G, m);
    element_mul(tmp_G_2, r->getElement("r1"), this->tmp_G_2);

    return element_cmp(h, tmp_G_2) == 0;
}


void CH_KEF_DL_CZT_2011::Adapt(CH_KEF_DL_CZT_2011_r *r_p, CH_KEF_DL_CZT_2011_sk *sk, element_t h, element_t m, CH_KEF_DL_CZT_2011_r *r,  element_t m_p, element_t I, CH_KEF_DL_CZT_2011_pp *pp) {
    // y
    element_pow_zn(this->tmp_G, pp->getElement("g"), sk->getElement("x"));
    // tmp_h
    this->H(this->tmp_G_2, this->tmp_G, I);
    // g^a  r_1
    // h^(m-m')
    element_sub(this->tmp_Zn, m, m_p);
    element_pow_zn(this->tmp_G_3, this->tmp_G_2, this->tmp_Zn);
    element_mul(tmp_G_3, r->getElement("r1"), this->tmp_G_3);
    r_p->insertElement("r1", "G1", tmp_G_3);
    // y^a r_2
    // x(m-m')
    // element_sub(this->tmp_Zn, *m, *m_p);
    element_mul(this->tmp_Zn_2, sk->getElement("x"), this->tmp_Zn);
    element_pow_zn(this->tmp_G, this->tmp_G_2, this->tmp_Zn_2);
    element_mul(tmp_G, r->getElement("r2"), this->tmp_G);
    r_p->insertElement("r2", "G1", tmp_G);
}

bool CH_KEF_DL_CZT_2011::Verify(element_t h, CH_KEF_DL_CZT_2011_r *r_p, element_t I, element_t m_p, CH_KEF_DL_CZT_2011_pk *pk, CH_KEF_DL_CZT_2011_pp *pp) {
    return this->Check(h, r_p, I, m_p, pk, pp);
}

CH_KEF_DL_CZT_2011::~CH_KEF_DL_CZT_2011() {}