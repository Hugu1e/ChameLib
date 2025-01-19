#include <scheme/CH_KEF_NoMH_AM_2004.h>

CH_KEF_NoMH_AM_2004::CH_KEF_NoMH_AM_2004(element_t *G1, element_t *G2, element_t *GT, element_t *Zn) {
    this->G1 = G1;
    this->G2 = G2;
    this->Zn = Zn;
    this->GT = GT;
    
    element_init_same_as(this->tmp_G1, *G1);
    element_init_same_as(this->tmp_G1_2, *G1);
    element_init_same_as(this->tmp_G2, *G2);
    element_init_same_as(this->tmp_Zn, *Zn);
    element_init_same_as(this->tmp_Zn_2, *Zn);
    element_init_same_as(this->tmp_GT, *GT);
    element_init_same_as(this->tmp_GT_2, *GT);
    element_init_same_as(this->tmp_GT_3, *GT);

    
}

/**
 * GenKey() -> (pk, sk)
 * @param pk: public key
 * @param sk: secret key
 */
void CH_KEF_NoMH_AM_2004::KeyGen(pk *pk, sk *sk) {
    element_random(sk->x);
    element_random(pk->g);

    // y = g^x
    element_pow_zn(pk->y, pk->g, sk->x);
}


/**
 * Hash(pk, m, r, s) -> h
 * @param pk: public key
 * @param m: message
 * @param r: random number r
 * @param s: random number s
 * @param h: hash value
 */
void CH_KEF_NoMH_AM_2004::Hash(pk *pk, element_t *m, element_t *r, element_t *s, element_t *h) {
    // random r,s
    element_random(*r);
    element_random(*s);

    // e = H(m,r)
    this->H(m, r, &this->tmp_Zn);

    // h = r - ((y^e)(g^s) mod p)mod q
    element_pow_zn(this->tmp_G1, pk->y, this->tmp_Zn);
    element_pow_zn(this->tmp_G1_2, pk->g, *s);
    element_mul(this->tmp_G1, this->tmp_G1, this->tmp_G1_2);
    element_sub(*h, *r, this->tmp_G1);
}

/**
 * H(m1,m2) -> res
 * @param m1: message 1
 * @param m2: message 2
 * @param res: hash value
 */
void CH_KEF_NoMH_AM_2004::H(element_t *m1, element_t *m2, element_t *res){
    Hgsm_1(*m1, *m2, *res);
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
void CH_KEF_NoMH_AM_2004::Forge(pk *pk,sk *sk, element_t *m_p, element_t *h, element_t *r_p, element_t *s_p){
    // k'
    element_random(this->tmp_Zn);
    // r_p = h + (g^k_p mod p) mod q
    element_pow_zn(this->tmp_G1, pk->g, this->tmp_Zn);
    element_add(*r_p, *h, this->tmp_G1);

    // e_p = H(m_p, r_p)
    this->H(m_p, r_p, &this->tmp_Zn_2);

    // s_p = k_p - e_p*x mod q
    element_mul(this->tmp_Zn_2, this->tmp_Zn_2, sk->x);
    element_sub(*s_p, this->tmp_Zn, this->tmp_Zn_2);
}


/**
 * Check(pk, m, r, s, h) -> bool
 * @param pk: public key
 * @param m: message
 * @param r: random number r
 * @param s: random number s
 * @param h: hash value
 */
bool CH_KEF_NoMH_AM_2004::Check(pk *pk, element_t *m, element_t *r, element_t *s, element_t *h){
    // e = H(m,r)
    this->H(m, r, &this->tmp_Zn);

    // h = r - ((y^e)(g^s) mod p)mod q
    element_pow_zn(this->tmp_G1, pk->y, this->tmp_Zn);
    element_pow_zn(this->tmp_G1_2, pk->g, *s);
    element_mul(this->tmp_G1, this->tmp_G1, this->tmp_G1_2);
    element_sub(this->tmp_Zn, *r, this->tmp_G1);

    return element_cmp(*h, this->tmp_Zn) == 0;
}


/**
 * input: pk, m_p, r_p, s_p, h
 * ouput: bool
 */
bool CH_KEF_NoMH_AM_2004::Verify(pk *pk, element_t *m_p, element_t *r_p, element_t *s_p, element_t *h){
    return this->Check(pk, m_p, r_p, s_p, h);
}

CH_KEF_NoMH_AM_2004::~CH_KEF_NoMH_AM_2004() {
    element_clear(this->tmp_G1);
    element_clear(this->tmp_G1_2);
    element_clear(this->tmp_G2);
    element_clear(this->tmp_Zn);
    element_clear(this->tmp_Zn_2);
    element_clear(this->tmp_GT);
    element_clear(this->tmp_GT_2);
    element_clear(this->tmp_GT_3);
}