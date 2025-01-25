#include <scheme/CH/CH_KEF_CZK_2004.h>

CH_KEF_CZK_2004::CH_KEF_CZK_2004(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn): PbcScheme(_G1, _G2, _GT, _Zn){}

/**
 * input : 
 * output: 
 */
void CH_KEF_CZK_2004::SetUp(CH_KEF_CZK_2004_pp *pp) {
    element_random(tmp_G);
    pp->insertElement("g", "G1", tmp_G);
}

/**
 * input : 
 * output: y
 */
void CH_KEF_CZK_2004::KeyGen(CH_KEF_CZK_2004_pk *pk, CH_KEF_CZK_2004_sk *sk, CH_KEF_CZK_2004_pp *pp) {  
    element_random(tmp_Zn);
    sk->insertElement("x", "Zn", tmp_Zn);

    element_pow_zn(tmp_G, pp->getElement("g"), sk->getElement("x"));
    pk->insertElement("y", "G1", tmp_G);
}
 

/**
 * input : I, m, y
 * output: h, r1, r2
 */
void CH_KEF_CZK_2004::Hash(element_t h, CH_KEF_CZK_2004_r *r, element_t m, element_t I, CH_KEF_CZK_2004_pk *pk, CH_KEF_CZK_2004_pp *pp) {
    element_random(tmp_Zn);

    element_pow_zn(tmp_G, pp->getElement("g"), tmp_Zn);
    r->insertElement("r1", "G1", tmp_G);
    element_pow_zn(tmp_G_2, pk->getElement("y"), tmp_Zn);
    r->insertElement("r2", "G1", tmp_G_2);

    // compute h
    element_mul(this->tmp_G, pp->getElement("g"), I);
    element_pow_zn(this->tmp_G, this->tmp_G, m);
    element_mul(h, this->tmp_G, tmp_G_2);
}


bool CH_KEF_CZK_2004::Check(element_t I, element_t m, CH_KEF_CZK_2004_r *r, element_t h, CH_KEF_CZK_2004_pp *pp){
    element_mul(this->tmp_G, pp->getElement("g"), I);
    element_pow_zn(this->tmp_G, this->tmp_G, m);
    element_mul(tmp_G_2, this->tmp_G, r->getElement("r2"));

    return element_cmp(h, tmp_G_2) == 0;
}

/**
 * input : (sk->getElement("x")) ,h, m, r1, r2, m_p, I
 * output: r1_p, r2_p
 */
void CH_KEF_CZK_2004::Adapt(CH_KEF_CZK_2004_r *r_p, CH_KEF_CZK_2004_sk *sk, element_t h, element_t m, CH_KEF_CZK_2004_r *r, element_t m_p, element_t I, CH_KEF_CZK_2004_pp *pp) {
    // compute r1_p
    element_sub(this->tmp_Zn, m, m_p);
    // x^-1
    element_invert(this->tmp_Zn_2, sk->getElement("x"));
    element_mul(this->tmp_Zn, this->tmp_Zn_2, this->tmp_Zn);
    element_mul(this->tmp_G, pp->getElement("g"), I);
    element_pow_zn(this->tmp_G, this->tmp_G, this->tmp_Zn);
    element_mul(tmp_G, r->getElement("r1"), this->tmp_G);
    r_p->insertElement("r1", "G1", tmp_G);

    // compute r2_p
    element_sub(this->tmp_Zn, m, m_p);
    element_mul(this->tmp_G, pp->getElement("g"), I);
    element_pow_zn(this->tmp_G, this->tmp_G, this->tmp_Zn);
    element_mul(tmp_G, r->getElement("r2"), this->tmp_G);
    r_p->insertElement("r2", "G1", tmp_G);
}

/**
 * input : I, m_p, r1_p, r2_p, h
 * output: bool
 */
bool CH_KEF_CZK_2004::Verify(element_t I, element_t m_p, CH_KEF_CZK_2004_r *r_p, element_t h, CH_KEF_CZK_2004_pp *pp) {
    return this->Check(I, m_p, r_p, h, pp);
}

CH_KEF_CZK_2004::~CH_KEF_CZK_2004() {}