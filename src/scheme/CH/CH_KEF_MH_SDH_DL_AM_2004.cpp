#include <scheme/CH/CH_KEF_MH_SDH_DL_AM_2004.h>

CH_KEF_MH_SDH_DL_AM_2004::CH_KEF_MH_SDH_DL_AM_2004(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn): PbcScheme(_G1, _G2, _GT, _Zn) {}

/**
 * input : 
 * output: 
 */
void CH_KEF_MH_SDH_DL_AM_2004::SetUp(CH_KEF_MH_SDH_DL_AM_2004_pp *pp) {
    element_random(tmp_G);
    pp->insertElement("g", "G1", tmp_G);
}

/**
 * input : 
 * output: y
 */
void CH_KEF_MH_SDH_DL_AM_2004::KeyGen(CH_KEF_MH_SDH_DL_AM_2004_pk *pk, CH_KEF_MH_SDH_DL_AM_2004_sk *sk, CH_KEF_MH_SDH_DL_AM_2004_pp *pp) {  
    element_random(tmp_Zn);
    sk->insertElement("x", "Zn", tmp_Zn);
    
    element_pow_zn(tmp_G , pp->getElement("g"), tmp_Zn);
    pk->insertElement("y", "G1", tmp_G);
}
 
void CH_KEF_MH_SDH_DL_AM_2004::H(element_t res, element_t m) {
    HASH::hash(res, m);
}

/**
 * input : label, m, r, y
 * output: h
 */
void CH_KEF_MH_SDH_DL_AM_2004::Hash(element_t h, element_t r, element_t label, element_t m, CH_KEF_MH_SDH_DL_AM_2004_pk *pk, CH_KEF_MH_SDH_DL_AM_2004_pp *pp) {
    element_random(r);
    
    // H(m)
    this->H(this->tmp_Zn, m);
    // e = H(label)
    this->H(this->tmp_Zn_2, label);
    // g^H(m)
    element_pow_zn(this->tmp_G, pp->getElement("g"), this->tmp_Zn);
    // g^e
    element_pow_zn(this->tmp_G_2, pp->getElement("g"), this->tmp_Zn_2);
    // g^e * y
    element_mul(this->tmp_G_2, this->tmp_G_2, pk->getElement("y"));
    // (g^e * y)^r
    element_pow_zn(this->tmp_G_2, this->tmp_G_2, r);
    // h = g^H(m) * (g^e * y)^r
    element_mul(h, this->tmp_G, this->tmp_G_2);
}

bool CH_KEF_MH_SDH_DL_AM_2004::Check(element_t m, element_t r, CH_KEF_MH_SDH_DL_AM_2004_pk *pk, element_t h, element_t label, CH_KEF_MH_SDH_DL_AM_2004_pp *pp) {
    // H(m)
    this->H(this->tmp_Zn, m);
    // e = H(label)
    this->H(this->tmp_Zn_2, label);
    // g^H(m)
    element_pow_zn(this->tmp_G, pp->getElement("g"), this->tmp_Zn);
    // g^e
    element_pow_zn(this->tmp_G_2, pp->getElement("g"), this->tmp_Zn_2);
    // g^e * y
    element_mul(this->tmp_G_2, this->tmp_G_2, pk->getElement("y"));
    // (g^e * y)^r
    element_pow_zn(this->tmp_G_2, this->tmp_G_2, r);
    // h = g^H(m) * (g^e * y)^r
    element_mul(tmp_G_3, this->tmp_G, this->tmp_G_2);

    return element_cmp(h, tmp_G_3) == 0;
}

/**
 * input : (this->x) ,h, m, label, r, m_p
 * output: r_p
 */
void CH_KEF_MH_SDH_DL_AM_2004::Adapt(element_t r_p, element_t h, element_t m, element_t label, element_t r, element_t m_p, CH_KEF_MH_SDH_DL_AM_2004_sk *sk) {
    // H(m) - H(m')
    this->H(this->tmp_Zn, m);
    this->H(this->tmp_Zn_2, m_p);
    element_sub(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    // x + e
    this->H(this->tmp_Zn_2, label);
    element_add(this->tmp_Zn_2, sk->getElement("x"), this->tmp_Zn_2);
    // ( H(m) - H(m')) / (x + e)
    element_div(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    // r_p = r + ( H(m) - H(m')) / (x + e)
    element_add(r_p, r, this->tmp_Zn);
}

/**
 * input : label, m_p, r_p, y, h
 * output: bool
 */
bool CH_KEF_MH_SDH_DL_AM_2004::Verify(element_t m_p, element_t r_p, CH_KEF_MH_SDH_DL_AM_2004_pk *pk, element_t h, element_t label, CH_KEF_MH_SDH_DL_AM_2004_pp *pp) {
    return this->Check(m_p, r_p, pk, h, label, pp);
}

CH_KEF_MH_SDH_DL_AM_2004::~CH_KEF_MH_SDH_DL_AM_2004() {}
