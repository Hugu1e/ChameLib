#include <scheme/CH/CH_KEF_MH_SDH_DL_AM_2004.h>

CH_KEF_MH_SDH_DL_AM_2004::CH_KEF_MH_SDH_DL_AM_2004(int curve): PbcScheme(curve) {
    element_init_G1(G1, pairing);
    element_init_GT(GT, pairing);
    element_init_Zr(Zn, pairing);

    element_init_same_as(tmp_G, G1);
    element_init_same_as(tmp_G_2, G1);
    element_init_same_as(tmp_GT, GT);
    element_init_same_as(tmp_GT_2, GT);
    element_init_same_as(tmp_Zn, Zn);
    element_init_same_as(tmp_Zn_2, Zn);
    element_init_same_as(tmp_Zn_3, Zn);
}

/**
 * input : 
 * output: 
 */
void CH_KEF_MH_SDH_DL_AM_2004::SetUp(CH_KEF_MH_SDH_DL_AM_2004_pp &pp) {    
    element_random(tmp_G);
    pp.set(g, tmp_G);
}

/**
 * input : 
 * output: y
 */
void CH_KEF_MH_SDH_DL_AM_2004::KeyGen(CH_KEF_MH_SDH_DL_AM_2004_pk &pk, CH_KEF_MH_SDH_DL_AM_2004_sk &sk, CH_KEF_MH_SDH_DL_AM_2004_pp &pp) {  
    element_random(tmp_Zn);
    sk.set(x, tmp_Zn);
    
    element_pow_zn(tmp_G , pp[g], tmp_Zn);
    pk.set(y, tmp_G);
}
 
void CH_KEF_MH_SDH_DL_AM_2004::H(element_t res, element_t m) {
    HASH::hash(res, m);
}

/**
 * input : label, m, r, y
 * output: h
 */
void CH_KEF_MH_SDH_DL_AM_2004::Hash(CH_KEF_MH_SDH_DL_AM_2004_h &h, CH_KEF_MH_SDH_DL_AM_2004_r &r, element_t m, element_t label, CH_KEF_MH_SDH_DL_AM_2004_pk &pk, CH_KEF_MH_SDH_DL_AM_2004_pp &pp) {
    element_random(tmp_Zn_3);
    element_pow_zn(tmp_G, pp[g], tmp_Zn_3);
    r.set(0, tmp_G);
    
    // H(m)
    this->H(this->tmp_Zn, m);
    // e = H(label)
    this->H(this->tmp_Zn_2, label);
    // g^H(m)
    element_pow_zn(this->tmp_G, pp[g], this->tmp_Zn);
    // g^e
    element_pow_zn(this->tmp_G_2, pp[g], this->tmp_Zn_2);
    // g^e * y
    element_mul(this->tmp_G_2, this->tmp_G_2, pk[y]);
    // (g^e * y)^r
    element_pow_zn(this->tmp_G_2, this->tmp_G_2, tmp_Zn_3);
    // h = g^H(m) * (g^e * y)^r
    element_mul(tmp_G, this->tmp_G, this->tmp_G_2);
    h.set(0, tmp_G);
}

bool CH_KEF_MH_SDH_DL_AM_2004::Check(CH_KEF_MH_SDH_DL_AM_2004_h &h, CH_KEF_MH_SDH_DL_AM_2004_r &r, element_t m, element_t label, CH_KEF_MH_SDH_DL_AM_2004_pk &pk, CH_KEF_MH_SDH_DL_AM_2004_pp &pp) {
    // H(m)
    this->H(this->tmp_Zn, m);
    // e = H(label)
    this->H(this->tmp_Zn_2, label);
    // g^H(m)
    element_pow_zn(this->tmp_G, pp[g], this->tmp_Zn);
    // F = hash / (g^H(m))
    element_div(this->tmp_G_2, h[0], this->tmp_G);
    // e(g, F)
    element_pairing(this->tmp_GT, pp[g], this->tmp_G_2);
    // g^e
    element_pow_zn(this->tmp_G_2, pp[g], this->tmp_Zn_2);
    // g^e * y
    element_mul(this->tmp_G_2, this->tmp_G_2, pk[y]);
    // e(g^r, g^e * y)
    element_pairing(this->tmp_GT_2, r[0], this->tmp_G_2);

    return element_cmp(this->tmp_GT, this->tmp_GT_2) == 0;
}

/**
 * input : (this->x) ,h, m, label, r, m_p
 * output: r_p
 */
void CH_KEF_MH_SDH_DL_AM_2004::Adapt(CH_KEF_MH_SDH_DL_AM_2004_r &r_p, element_t m_p, CH_KEF_MH_SDH_DL_AM_2004_h &h, CH_KEF_MH_SDH_DL_AM_2004_r &r, element_t m, element_t label, CH_KEF_MH_SDH_DL_AM_2004_sk &sk, CH_KEF_MH_SDH_DL_AM_2004_pp &pp) {
    // H(m) - H(m')
    this->H(this->tmp_Zn, m);
    this->H(this->tmp_Zn_2, m_p);
    element_sub(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    // x + e
    this->H(this->tmp_Zn_2, label);
    element_add(this->tmp_Zn_2, sk[x], this->tmp_Zn_2);
    // ( H(m) - H(m')) / (x + e)
    element_div(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    // g^r_p = g^r * g^( H(m) - H(m')) / (x + e))
    element_pow_zn(this->tmp_G, pp[g], tmp_Zn);
    element_mul(this->tmp_G, this->tmp_G, r[0]);
    r_p.set(0, this->tmp_G);
}

/**
 * input : label, m_p, r_p, y, h
 * output: bool
 */
bool CH_KEF_MH_SDH_DL_AM_2004::Verify(CH_KEF_MH_SDH_DL_AM_2004_h &h, CH_KEF_MH_SDH_DL_AM_2004_r &r_p, element_t m_p, element_t label, CH_KEF_MH_SDH_DL_AM_2004_pk &pk, CH_KEF_MH_SDH_DL_AM_2004_pp &pp) {
    return this->Check(h, r_p, m_p, label, pk, pp);
}

CH_KEF_MH_SDH_DL_AM_2004::~CH_KEF_MH_SDH_DL_AM_2004() {
    element_clear(tmp_G);
    element_clear(tmp_G_2);
    element_clear(tmp_GT);
    element_clear(tmp_GT_2);
    element_clear(tmp_Zn);
    element_clear(tmp_Zn_2);
    element_clear(tmp_Zn_3);

    element_clear(G1);
    element_clear(GT);
    element_clear(Zn);
}
