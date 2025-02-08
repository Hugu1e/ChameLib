#include <scheme/CH/CH_KEF_NoMH_AM_2004.h>

CH_KEF_NoMH_AM_2004::CH_KEF_NoMH_AM_2004(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn): PbcScheme(_G1, _G2, _GT, _Zn) {}

void CH_KEF_NoMH_AM_2004::Setup(CH_KEF_NoMH_AM_2004_pk &pk, CH_KEF_NoMH_AM_2004_sk &sk){
    pk.init(2);
    sk.init(1); 
}


/**
 * GenKey() -> (pk, sk)
 * @param pk: public key
 * @param sk: secret key
 */
void CH_KEF_NoMH_AM_2004::KeyGen(CH_KEF_NoMH_AM_2004_pk &pk, CH_KEF_NoMH_AM_2004_sk &sk) {
    element_random(tmp_Zn);
    sk.set(x, tmp_Zn);
    element_random(tmp_G);
    pk.set(g, tmp_G);

    // y = g^x
    element_pow_zn(tmp_G, tmp_G, tmp_Zn);
    pk.set(y, tmp_G);
}


/**
 * H(m1,m2) -> res
 * @param m1: message 1
 * @param m2: message 2
 * @param res: hash value
 */
void CH_KEF_NoMH_AM_2004::H(element_t res, element_t m1, element_t m2){
    HASH::hash(res, m1, m2);
}

/**
 * Hash(pk, m, r, s) -> h
 * @param pk: public key
 * @param m: message
 * @param r: random number r
 * @param s: random number s
 * @param h: hash value
 */
void CH_KEF_NoMH_AM_2004::Hash(element_t h, element_t r, element_t s, CH_KEF_NoMH_AM_2004_pk &pk, element_t m) {
    // random r,s
    element_random(r);
    element_random(s);

    // e = H(m,r)
    this->H(this->tmp_Zn, m, r);

    // h = r - ((y^e)(g^s) mod p)mod q
    element_pow_zn(this->tmp_G, pk[y], this->tmp_Zn);
    element_pow_zn(this->tmp_G_2, pk[g], s);
    element_mul(this->tmp_G, this->tmp_G, this->tmp_G_2);
    element_sub(h, r, this->tmp_G);
}


/**
 * Check(pk, m, r, s, h) -> bool
 * @param pk: public key
 * @param m: message
 * @param r: random number r
 * @param s: random number s
 * @param h: hash value
 */
bool CH_KEF_NoMH_AM_2004::Check(CH_KEF_NoMH_AM_2004_pk &pk, element_t m, element_t r, element_t s, element_t h){
    // e = H(m,r)
    this->H(this->tmp_Zn, m, r);

    // h = r - ((y^e)(g^s) mod p)mod q
    element_pow_zn(this->tmp_G, pk[y], this->tmp_Zn);
    element_pow_zn(this->tmp_G_2, pk[g], s);
    element_mul(this->tmp_G, this->tmp_G, this->tmp_G_2);
    element_sub(this->tmp_G, r, this->tmp_G);

    return element_cmp(h, this->tmp_G) == 0;
}


/**
 * Forge(pk, sk, m_p, h) -> (r_p, s_p)
 * @param pk: public key
 * @param sk: secret key
 * @param m_p: modified message'
 * @param h: hash value
 * @param r_p: random number r'
 * @param s_p: random number s'
 */
void CH_KEF_NoMH_AM_2004::Adapt(element_t r_p,  element_t s_p, CH_KEF_NoMH_AM_2004_pk &pk, CH_KEF_NoMH_AM_2004_sk &sk, element_t m_p, element_t h){
    // k'
    element_random(this->tmp_Zn);
    // r_p = h + (g^k_p mod p) mod q
    element_pow_zn(this->tmp_G, pk[g], this->tmp_Zn);

    element_add(r_p, h, this->tmp_G);

    // e_p = H(m_p, r_p)
    this->H(this->tmp_Zn_2, m_p, r_p);

    // s_p = k_p - e_p*x mod q
    element_mul(this->tmp_Zn_3, this->tmp_Zn_2, sk[x]);
    element_sub(s_p, this->tmp_Zn, this->tmp_Zn_3);
}



/**
 * input: pk, m_p, r_p, s_p, h
 * ouput: bool
 */
bool CH_KEF_NoMH_AM_2004::Verify(CH_KEF_NoMH_AM_2004_pk &pk, element_t m_p, element_t r_p, element_t s_p, element_t h){
    return this->Check(pk, m_p, r_p, s_p, h);
}

CH_KEF_NoMH_AM_2004::~CH_KEF_NoMH_AM_2004() {}