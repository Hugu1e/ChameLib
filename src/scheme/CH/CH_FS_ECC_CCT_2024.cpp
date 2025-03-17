#include <scheme/CH/CH_FS_ECC_CCT_2024.h>

CH_FS_ECC_CCT_2024::CH_FS_ECC_CCT_2024(int curve, int group): PbcScheme(curve){
    this->group = group;
    this->curve = curve;
    switch(group){
        case Group::G1:
            element_init_G1(G1, pairing);
            break;
        case Group::G2:
            element_init_G2(G1, pairing);
            break;
        case Group::GT:
            element_init_GT(G1, pairing);
            break;
        default:
            throw CurveException(CurveException::INVALID_GROUP);
    }
    element_init_Zr(Zn, pairing);

    element_init_same_as(this->rho, Zn);
    element_init_same_as(this->t1, Zn);
    element_init_same_as(this->t2, Zn);
    element_init_same_as(this->T1, G1);
    element_init_same_as(this->T2, G1);
    element_init_same_as(this->c2, Zn);

    element_init_same_as(this->tmp_G, G1);
    element_init_same_as(this->tmp_G_2, G1);
    element_init_same_as(this->tmp_Zn, Zn);
}


void CH_FS_ECC_CCT_2024::SetUp(CH_FS_ECC_CCT_2024_pp &pp){
    element_random(tmp_G);
    pp.set(g, tmp_G);
}

/**
 * KeyGen() -> (pk, sk)
 * @param pk public key
 * @param sk secret key
 */
void CH_FS_ECC_CCT_2024::KeyGen(CH_FS_ECC_CCT_2024_pk &pk, CH_FS_ECC_CCT_2024_sk &sk, CH_FS_ECC_CCT_2024_pp &pp) {
    element_random(tmp_Zn);
    sk.set(x, tmp_Zn);

    // y = g^x
    element_pow_zn(tmp_G, pp[g], tmp_Zn);
    pk.set(y, tmp_G);
}


/**
 * Hash(pk, m, r, s) -> h
 * @param pk: public key
 * @param m: message
 * @param h: hash value
 * @param r: a NIZK proof
 */
void CH_FS_ECC_CCT_2024::Hash(element_t h, CH_FS_ECC_CCT_2024_r &r, CH_FS_ECC_CCT_2024_pk &pk, element_t m, CH_FS_ECC_CCT_2024_pp &pp) {
    // random 𝜌
    element_random(this->rho);

    // h = g^𝜌 * H(m)
    this->H(this->tmp_G, m);
    element_pow_zn(this->tmp_G_2, pp[g], this->rho);
    element_mul(h, this->tmp_G, this->tmp_G_2);

    // compute a NIZK proof r
    element_random(this->t2);
    element_random(tmp_Zn);
    r.set(z1, tmp_Zn);
    // T2 = g^t2
    element_pow_zn(this->T2, pp[g], this->t2);
    // c1 = H'(T2, pkch, g^rho, m)
    this->H(tmp_Zn, this->T2, pk[y], this->tmp_G_2, m);
    r.set(c1, tmp_Zn);
    // T1 = g^z1 * pkch^c1
    element_pow_zn(this->T1, pp[g], r[z1]);
    element_pow_zn(this->tmp_G, pk[y], r[c1]);
    element_mul(this->T1, this->T1, this->tmp_G);

    // c2 = H'(T1, pkch, g^pho, m)
    this->H(this->c2, this->T1, pk[y], this->tmp_G_2, m);
    // z2 = t2 - c2 * rho
    element_mul(this->tmp_Zn, this->c2, this->rho);
    element_sub(tmp_Zn, this->t2, this->tmp_Zn);
    r.set(z2, tmp_Zn);
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
    mpz_t ndonr;
    mpz_init(ndonr);
    Func::GetNdonr(ndonr, this->group, this->curve);

    element_pow_mpz(m1, m1, ndonr);

    HASH::hash(res, m1, m2, m3, m4);

    mpz_clear(ndonr);
}

/**
 * Check(pk, m, h, r) -> bool
 * @param pk: public key
 * @param m: message
 * @param h: hash value
 * @param r: random number r
 */
bool CH_FS_ECC_CCT_2024::Check(CH_FS_ECC_CCT_2024_pk &pk, element_t m, element_t h, CH_FS_ECC_CCT_2024_r &r, CH_FS_ECC_CCT_2024_pp &pp){
    // y' = h/H(m)
    this->H(this->tmp_G, m);
    element_div(this->tmp_G, h, this->tmp_G);

    // T1 = g^z1 * pkch^c1
    element_pow_zn(this->T1, pp[g], r[z1]);
    element_pow_zn(this->tmp_G_2, pk[y], r[c1]);
    element_mul(this->T1, this->T1, this->tmp_G_2);
    // c2 = H'(T1, pkch, y', m)
    this->H(this->c2, this->T1, pk[y], this->tmp_G, m);
    // T2 = g^z2 * y'^c2
    element_pow_zn(this->T2, pp[g], r[z2]);
    element_pow_zn(this->tmp_G_2, this->tmp_G, this->c2);
    element_mul(this->T2, this->T2, this->tmp_G_2);
    // c1 = H'(T2, pkch, y', m)
    this->H(this->tmp_Zn, this->T2, pk[y], this->tmp_G, m);

    return element_cmp(r[c1], this->tmp_Zn) == 0;
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
void CH_FS_ECC_CCT_2024::Adapt(CH_FS_ECC_CCT_2024_r &r_p, CH_FS_ECC_CCT_2024_pk &pk, CH_FS_ECC_CCT_2024_sk &sk, element_t m, element_t m_p, element_t h, CH_FS_ECC_CCT_2024_r &r, CH_FS_ECC_CCT_2024_pp &pp){
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
    r_p.set(z2, tmp_Zn);

    // T1' = g^t1'
    element_pow_zn(this->T1, pp[g], this->t1);

    // c2' = H'(T1', pkch, y', m')
    this->H(this->c2, this->T1, pk[y], this->tmp_G, m_p);

    // T2' = g^z2' * y'^c2' 
    element_pow_zn(this->T2, pp[g], r_p[z2]);
    element_pow_zn(this->tmp_G_2, this->tmp_G, this->c2);
    element_mul(this->T2, this->T2, this->tmp_G_2);

    // c1' = H'(T2', pkch, y', m')
    this->H(tmp_Zn, this->T2, pk[y], this->tmp_G, m_p);
    r_p.set(c1, tmp_Zn);

    // z1' = t1' - c1' * x
    element_mul(this->tmp_Zn, tmp_Zn, sk[x]);
    element_sub(tmp_Zn, this->t1, this->tmp_Zn);
    r_p.set(z1, tmp_Zn);
}


/**
 * Verify(pk, m_p, r_p, s_p, h) -> bool
 * @param pk: public key
 * @param m_p: modified message m'
 * @param h: hash value
 * @param r_p: a NIZK proof r'
 */
bool CH_FS_ECC_CCT_2024::Verify(CH_FS_ECC_CCT_2024_pk &pk, element_t m_p, element_t h, CH_FS_ECC_CCT_2024_r &r_p, CH_FS_ECC_CCT_2024_pp &pp){
    return this->Check(pk, m_p, h, r_p, pp);
}

CH_FS_ECC_CCT_2024::~CH_FS_ECC_CCT_2024() {
    element_clear(this->rho);
    element_clear(this->t1);
    element_clear(this->t2);
    element_clear(this->T1);
    element_clear(this->T2);
    element_clear(this->c2);

    element_clear(this->tmp_G);
    element_clear(this->tmp_G_2);
    element_clear(this->tmp_Zn);

    element_clear(G1);
    element_clear(Zn);
}
