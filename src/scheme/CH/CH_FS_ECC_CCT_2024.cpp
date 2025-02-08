#include <scheme/CH/CH_FS_ECC_CCT_2024.h>

CH_FS_ECC_CCT_2024::CH_FS_ECC_CCT_2024(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn): PbcScheme(_G1, _G2, _GT, _Zn){
    element_init_same_as(this->rho, Zn);
    element_init_same_as(this->t1, Zn);
    element_init_same_as(this->t2, Zn);
    element_init_same_as(this->T1, G1);
    element_init_same_as(this->T2, G1);
    element_init_same_as(this->c2, Zn);
}


void CH_FS_ECC_CCT_2024::SetUp(CH_FS_ECC_CCT_2024_pp *pp){
    element_random(tmp_G);
    pp->insert(PP_G, tmp_G);
}

/**
 * KeyGen() -> (pk, sk)
 * @param pk public key
 * @param sk secret key
 */
void CH_FS_ECC_CCT_2024::KeyGen(CH_FS_ECC_CCT_2024_pk *pk, CH_FS_ECC_CCT_2024_sk *sk, CH_FS_ECC_CCT_2024_pp *pp) {
    element_random(tmp_Zn);
    sk->insert(SK_X, tmp_Zn);

    // y = g^x
    element_pow_zn(tmp_G, pp->get(PP_G), tmp_Zn);
    pk->insert(PK_Y, tmp_G);
}


/**
 * Hash(pk, m, r, s) -> h
 * @param pk: public key
 * @param m: message
 * @param h: hash value
 * @param r: a NIZK proof
 */
void CH_FS_ECC_CCT_2024::Hash(element_t h, CH_FS_ECC_CCT_2024_r *r, CH_FS_ECC_CCT_2024_pk *pk, element_t m, CH_FS_ECC_CCT_2024_pp *pp) {
    // random 𝜌
    element_random(this->rho);

    // h = g^𝜌 * H(m)
    this->H(this->tmp_G, m);
    element_pow_zn(this->tmp_G_2, pp->get(PP_G), this->rho);
    element_mul(h, this->tmp_G, this->tmp_G_2);

    // compute a NIZK proof r
    element_random(this->t2);
    element_random(tmp_Zn);
    r->insert(R_Z1, tmp_Zn);
    // T2 = g^t2
    element_pow_zn(this->T2, pp->get(PP_G), this->t2);
    // c1 = H'(T2, pkch, g^rho, m)
    this->H(tmp_Zn, this->T2, pk->get(PK_Y), this->tmp_G_2, m);
    r->insert(R_C1, tmp_Zn);
    // T1 = g^z1 * pkch^c1
    element_pow_zn(this->T1, pp->get(PP_G), r->get(R_Z1));
    element_pow_zn(this->tmp_G, pk->get(PK_Y), r->get(R_C1));
    element_mul(this->T1, this->T1, this->tmp_G);

    // c2 = H'(T1, pkch, g^pho, m)
    this->H(this->c2, this->T1, pk->get(PK_Y), this->tmp_G_2, m);
    // z2 = t2 - c2 * rho
    element_mul(this->tmp_Zn, this->c2, this->rho);
    element_sub(tmp_Zn, this->t2, this->tmp_Zn);
    r->insert(R_Z2, tmp_Zn);
}

/**
 * H(m) -> res
 * @param m: message m
 * @param res: hash value
 */
void CH_FS_ECC_CCT_2024::H(element_t res, element_t m){
    HASH::hash(res, m);
}

/**
 * H'(m1,m2,m3,m4) -> res
 * @param m1: message m1
 * @param m2: message m2
 * @param m3: message m3
 * @param m4: message m4
 * @param res: hash value
 */
void CH_FS_ECC_CCT_2024::H(element_t res, element_t m1, element_t m2, element_t m3, element_t m4){
    HASH::hash(res, m1, m2, m3, m4);
}

/**
 * Check(pk, m, h, r) -> bool
 * @param pk: public key
 * @param m: message
 * @param h: hash value
 * @param r: random number r
 */
bool CH_FS_ECC_CCT_2024::Check(CH_FS_ECC_CCT_2024_pk *pk, element_t m, element_t h, CH_FS_ECC_CCT_2024_r *r, CH_FS_ECC_CCT_2024_pp *pp){
    // y' = h/H(m)
    this->H(this->tmp_G, m);
    element_div(this->tmp_G, h, this->tmp_G);

    // T1 = g^z1 * pkch^c1
    element_pow_zn(this->T1, pp->get(PP_G), r->get(R_Z1));
    element_pow_zn(this->tmp_G_2, pk->get(PK_Y), r->get(R_C1));
    element_mul(this->T1, this->T1, this->tmp_G_2);
    // c2 = H'(T1, pkch, y', m)
    this->H(this->c2, this->T1, pk->get(PK_Y), this->tmp_G, m);
    // T2 = g^z2 * y'^c2
    element_pow_zn(this->T2, pp->get(PP_G), r->get(R_Z2));
    element_pow_zn(this->tmp_G_2, this->tmp_G, this->c2);
    element_mul(this->T2, this->T2, this->tmp_G_2);
    // c1 = H'(T2, pkch, y', m)
    this->H(this->tmp_Zn, this->T2, pk->get(PK_Y), this->tmp_G, m);

    return element_cmp(r->get(R_C1), this->tmp_Zn) == 0;
}


/**
 * Adapt(pk, sk, m, m_p, h, r) -> r_p
 * @param pk: public key
 * @param sk: secret key
 * @param m: message m
 * @param m_p: modified message m'
 * @param h: hash value
 * @param r: a NIZK proof r
 * @param r_p: a NIZK proof r'
 */
void CH_FS_ECC_CCT_2024::Adapt(CH_FS_ECC_CCT_2024_r *r_p, CH_FS_ECC_CCT_2024_pk *pk, CH_FS_ECC_CCT_2024_sk *sk, element_t m, element_t m_p, element_t h, CH_FS_ECC_CCT_2024_r *r, CH_FS_ECC_CCT_2024_pp *pp){
    if(!this->Check(pk, m, h, r, pp)){
        printf("Adapt failed: Hash Check failed\n");
        return;
    }

    // y' = h/H(m')
    this->H(this->tmp_G, m_p);
    element_div(this->tmp_G, h, this->tmp_G);
    
    // random t1',z2'
    element_random(this->t1);
    element_random(tmp_Zn);
    r_p->insert(R_Z2, tmp_Zn);

    // T1' = g^t1'
    element_pow_zn(this->T1, pp->get(PP_G), this->t1);

    // c2' = H'(T1', pkch, y', m')
    this->H(this->c2, this->T1, pk->get(PK_Y), this->tmp_G, m_p);

    // T2' = g^z2' * y'^c2' 
    element_pow_zn(this->T2, pp->get(PP_G), r_p->get(R_Z2));
    element_pow_zn(this->tmp_G_2, this->tmp_G, this->c2);
    element_mul(this->T2, this->T2, this->tmp_G_2);

    // c1' = H'(T2', pkch, y', m')
    this->H(tmp_Zn, this->T2, pk->get(PK_Y), this->tmp_G, m_p);
    r_p->insert(R_C1, tmp_Zn);

    // z1' = t1' - c1' * x
    element_mul(this->tmp_Zn, tmp_Zn, sk->get(SK_X));
    element_sub(tmp_Zn, this->t1, this->tmp_Zn);
    r_p->insert(R_Z1, tmp_Zn);
}


/**
 * Verify(pk, m_p, r_p, s_p, h) -> bool
 * @param pk: public key
 * @param m_p: modified message m'
 * @param h: hash value
 * @param r_p: a NIZK proof r'
 */
bool CH_FS_ECC_CCT_2024::Verify(CH_FS_ECC_CCT_2024_pk *pk, element_t m_p, element_t h, CH_FS_ECC_CCT_2024_r *r_p, CH_FS_ECC_CCT_2024_pp *pp){
    return this->Check(pk, m_p, h, r_p, pp);
}

CH_FS_ECC_CCT_2024::~CH_FS_ECC_CCT_2024() {
    element_clear(this->rho);
    element_clear(this->t1);
    element_clear(this->t2);
    element_clear(this->T1);
    element_clear(this->T2);
    element_clear(this->c2);
}
