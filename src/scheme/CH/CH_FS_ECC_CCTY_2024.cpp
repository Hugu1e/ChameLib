#include <scheme/CH_FS_ECC_CCTY_2024.h>

CH_FS_ECC_CCTY_2024::CH_FS_ECC_CCTY_2024(element_t *_G1, element_t *_G2, element_t *_GT, element_t *_Zn) {
    this->G1 = _G1;
    this->G2 = _G2;
    this->Zn = _Zn;
    this->GT = _GT;
    
    element_init_same_as(this->tmp_G1, *_G1);
    element_init_same_as(this->tmp_G1_2, *_G1);
    element_init_same_as(this->tmp_G2, *_G2);
    element_init_same_as(this->tmp_Zn, *_Zn);
    element_init_same_as(this->tmp_Zn_2, *_Zn);
    element_init_same_as(this->tmp_GT, *_GT);
    element_init_same_as(this->tmp_GT_2, *_GT);
    element_init_same_as(this->tmp_GT_3, *_GT);

    element_init_same_as(this->g, *_G1);
    element_init_same_as(this->rho, *_Zn);
    element_init_same_as(this->t1, *_Zn);
    element_init_same_as(this->t2, *_Zn);
    element_init_same_as(this->T1, *_G1);
    element_init_same_as(this->T2, *_G1);
    element_init_same_as(this->c2, *_Zn);
}


void CH_FS_ECC_CCTY_2024::ParamGen(){
    element_random(this->g);
}

/**
 * KeyGen() -> (pk, sk)
 * @param pk public key
 * @param sk secret key
 */
void CH_FS_ECC_CCTY_2024::KeyGen(pk *pk, sk *sk) {
    element_random(sk->x);

    // y = g^x
    element_pow_zn(pk->y, this->g, sk->x);
}


/**
 * Hash(pk, m, r, s) -> h
 * @param pk: public key
 * @param m: message
 * @param h: hash value
 * @param r: a NIZK proof
 */
void CH_FS_ECC_CCTY_2024::Hash(pk *pk, element_t *m, element_t *h, r *r) {
    // random 𝜌
    element_random(this->rho);

    // h = g^𝜌 * H(m)
    this->H(m, &this->tmp_G1);
    element_pow_zn(this->tmp_G1_2, this->g, this->rho);
    element_mul(*h, this->tmp_G1, this->tmp_G1_2);

    // compute a NIZK proof r
    element_random(this->t2);
    element_random(r->z1);
    // T2 = g^t2
    element_pow_zn(this->T2, this->g, this->t2);
    // c1 = H'(T2, pkch, g^rho, m)
    this->H(&this->T2, &pk->y, &this->tmp_G1_2, m, &r->c1);
    // T1 = g^z1 * pkch^c1
    element_pow_zn(this->T1, this->g, r->z1);
    element_pow_zn(this->tmp_G1, pk->y, r->c1);
    element_mul(this->T1, this->T1, this->tmp_G1);

    // c2 = H'(T1, pkch, g^pho, m)
    this->H(&this->T1, &pk->y, &this->tmp_G1_2, m, &this->c2);

    // z2 = t2 - c2 * rho
    element_mul(this->tmp_Zn, this->c2, this->rho);
    element_sub(r->z2, this->t2, this->tmp_Zn);
}

/**
 * H(m) -> res
 * @param m: message m
 * @param res: hash value
 */
void CH_FS_ECC_CCTY_2024::H(element_t *m, element_t *res){
    Hm_1(*m, *res);
}

/**
 * H'(m1,m2,m3,m4) -> res
 * @param m1: message m1
 * @param m2: message m2
 * @param m3: message m3
 * @param m4: message m4
 * @param res: hash value
 */
void CH_FS_ECC_CCTY_2024::H(element_t *m1, element_t *m2, element_t *m3, element_t *m4, element_t *res){
    Hm_5(*m1, *m2, *m3, *m4, *res);
}

/**
 * Check(pk, m, h, r) -> bool
 * @param pk: public key
 * @param m: message
 * @param h: hash value
 * @param r: random number r
 */
bool CH_FS_ECC_CCTY_2024::Check(pk *pk, element_t *m, element_t *h, r *r){
    // y' = h/H(m)
    this->H(m, &this->tmp_G1);
    element_div(this->tmp_G1, *h, this->tmp_G1);

    // T1 = g^z1 * pkch^c1
    element_pow_zn(this->T1, this->g, r->z1);
    element_pow_zn(this->tmp_G1_2, pk->y, r->c1);
    element_mul(this->T1, this->T1, this->tmp_G1_2);
    // c2 = H'(T1, pkch, y', m)
    this->H(&this->T1, &pk->y, &this->tmp_G1, m, &this->c2);
    // T2 = g^z2 * y'^c2
    element_pow_zn(this->T2, this->g, r->z2);
    element_pow_zn(this->tmp_G1_2, this->tmp_G1, this->c2);
    element_mul(this->T2, this->T2, this->tmp_G1_2);
    // c1 = H'(T2, pkch, y', m)
    this->H(&this->T2, &pk->y, &this->tmp_G1, m, &this->tmp_Zn);

    return element_cmp(r->c1, this->tmp_Zn) == 0;
}


/**
 * Forge(pk, sk, m, m_p, h, r) -> r_p
 * @param pk: public key
 * @param sk: secret key
 * @param m: message m
 * @param m_p: modified message m'
 * @param h: hash value
 * @param r: a NIZK proof r
 * @param r_p: a NIZK proof r'
 */
void CH_FS_ECC_CCTY_2024::Forge(pk *pk, sk *sk, element_t *m, element_t *m_p, element_t *h, CH_FS_ECC_CCTY_2024::r *r, CH_FS_ECC_CCTY_2024::r *r_p){
    if(!this->Check(pk, m, h, r)){
        printf("Forge failed: Hash Check failed\n");
        return;
    }

    // y' = h/H(m')
    this->H(m_p, &this->tmp_G1);
    element_div(this->tmp_G1, *h, this->tmp_G1);
    
    // random t1',z2'
    element_random(this->t1);
    element_random(r_p->z2);

    // T1' = g^t1'
    element_pow_zn(this->T1, this->g, this->t1);

    // c2' = H'(T1', pkch, y', m')
    this->H(&this->T1, &pk->y, &this->tmp_G1, m_p, &this->c2);

    // T2' = g^z2' * y'^c2' 
    element_pow_zn(this->T2, this->g, r_p->z2);
    element_pow_zn(this->tmp_G1_2, this->tmp_G1, this->c2);
    element_mul(this->T2, this->T2, this->tmp_G1_2);

    // c1' = H'(T2', pkch, y', m')
    this->H(&this->T2, &pk->y, &this->tmp_G1, m_p, &r_p->c1);

    // z1' = t1' - c1' * x
    element_mul(this->tmp_Zn, r_p->c1, sk->x);
    element_sub(r_p->z1, this->t1, this->tmp_Zn);
}


/**
 * Verify(pk, m_p, r_p, s_p, h) -> bool
 * @param pk: public key
 * @param m_p: modified message m'
 * @param h: hash value
 * @param r_p: a NIZK proof r'
 */
bool CH_FS_ECC_CCTY_2024::Verify(pk *pk, element_t *m_p, element_t *h, r *r_p){
    return this->Check(pk, m_p, h, r_p);
}

CH_FS_ECC_CCTY_2024::~CH_FS_ECC_CCTY_2024() {
    element_clear(this->tmp_G1);
    element_clear(this->tmp_G1_2);
    element_clear(this->tmp_G2);
    element_clear(this->tmp_Zn);
    element_clear(this->tmp_Zn_2);
    element_clear(this->tmp_GT);
    element_clear(this->tmp_GT_2);
    element_clear(this->tmp_GT_3);

    element_clear(this->g);
    element_clear(this->rho);
    element_clear(this->t1);
    element_clear(this->t2);
    element_clear(this->T1);
    element_clear(this->T2);
    element_clear(this->c2);
}
