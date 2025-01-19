#include <scheme/CH_ET_BC_CDK_2017.h>

CH_ET_BC_CDK_2017::CH_ET_BC_CDK_2017(){}


/**
 * @param pp: public parameters
 * @param k: length of n(RSA)
 */
void CH_ET_BC_CDK_2017::SetUp(pp *pp, int k){
    pp->k = k;
}

/**
 * @param pk: public key
 * @param sk: secret key
 * @param pp: public parameters
 */
void CH_ET_BC_CDK_2017::KeyGen(pk *pk, sk *sk, pp *pp){
    rsa.KeyGen(&pk->n0, &pk->e0, &sk->d0, pp->k);
}

/**
 * H(m) -> res mod n
 * @param res: result
 * @param m: message
 * @param n: modulus
 */
void CH_ET_BC_CDK_2017::H(mpz_t *res, string m, mpz_t *n){
    Hm_n(*res, m, *n);  
}

/**
 * @param h: hash value
 * @param r: random value
 * @param etd: ephemeral trapdoor
 * @param pp: public parameters
 * @param pk: public key
 * @param m: message
 */
void CH_ET_BC_CDK_2017::Hash(h *h, r* r, etd *etd, pp *pp, pk *pk, string m){
    mpz_t tmp1;
    mpz_inits(tmp1, NULL);

    rsa.KeyGen(&h->n1, &h->e1, &etd->d1, pp->k);

    // r0,r1
    GenerateRandomInZnStar(r->r0, pk->n0);
    GenerateRandomInZnStar(r->r1, h->n1);

    // h0 = H(m)r0^e0 mod n0
    H(&tmp1, m, &pk->n0);
    mpz_powm(h->h0, r->r0, pk->e0, pk->n0);
    mpz_mul(h->h0, h->h0, tmp1);
    mpz_mod(h->h0, h->h0, pk->n0);
    // h1 = H(m)r1^e1 mod n1
    H(&tmp1, m, &h->n1);
    mpz_powm(h->h1, r->r1, h->e1, h->n1);
    mpz_mul(h->h1, h->h1, tmp1);
    mpz_mod(h->h1, h->h1, h->n1);
    
    mpz_clears(tmp1, NULL);
}

/**
 * @param h: hash value
 * @param r: random value
 * @param pk: public key
 * @param m: message
 * @return bool
 */
bool CH_ET_BC_CDK_2017::Check(h *h, r* r, pk *pk, string m){
    mpz_t tmp1,tmp2;
    mpz_inits(tmp1, tmp2, NULL);

    // h0 = H(m)r0^e0 mod n0
    H(&tmp1, m, &pk->n0);
    mpz_powm(tmp2, r->r0, pk->e0, pk->n0);
    mpz_mul(tmp2, tmp2, tmp1);
    mpz_mod(tmp2, tmp2, pk->n0);
    if(mpz_cmp(tmp2, h->h0) != 0){
        mpz_clears(tmp1, tmp2, NULL);
        return false;
    }
    // h1 = H(m)r1^e1 mod n1
    H(&tmp1, m, &h->n1);
    mpz_powm(tmp2, r->r1, h->e1, h->n1);
    mpz_mul(tmp2, tmp2, tmp1);
    mpz_mod(tmp2, tmp2, h->n1);
    if(mpz_cmp(tmp2, h->h1) != 0){
        mpz_clears(tmp1, tmp2, NULL);
        return false;
    }
    
    mpz_clears(tmp1, tmp2, NULL);
    return true;
}

/**
 * @param r_p: adapted random value
 * @param sk: secret key
 * @param etd: ephemeral trapdoor
 * @param pk: public key
 * @param h: hash value
 * @param r: random value
 * @param m: message
 * @param m_p: adapted message
 */
void CH_ET_BC_CDK_2017::Adapt(r *r_p, sk *sk, etd *etd, pk *pk, h *h, r* r, string m, string m_p){
    if(!Check(h, r, pk, m)){
        throw std::runtime_error("Adapt(): Hash Check failed!");
    }
    mpz_t tmp1;
    mpz_inits(tmp1, NULL);

    // r0' = (h0 * (H(m')^(-1)))^d0 mod n0
    H(&tmp1, m_p, &pk->n0);
    mpz_invert(tmp1, tmp1, pk->n0);
    mpz_mul(tmp1, h->h0, tmp1);
    mpz_mod(tmp1, tmp1, pk->n0);
    mpz_powm(r_p->r0, tmp1, sk->d0, pk->n0);
    // r1' = (h1 * (H(m')^(-1)))^d1 mod n1
    H(&tmp1, m_p, &h->n1);
    mpz_invert(tmp1, tmp1, h->n1);
    mpz_mul(tmp1, h->h1, tmp1);
    mpz_mod(tmp1, tmp1, h->n1);
    mpz_powm(r_p->r1, tmp1, etd->d1, h->n1);

    mpz_clears(tmp1, NULL);
}

/**
 * @param h: hash value
 * @param r_p: adapted random value
 * @param pk: public key
 * @param m_p: adapted message
 * @return bool
 */
bool CH_ET_BC_CDK_2017::Verify(h *h, r* r_p, pk *pk, string m_p){
    return Check(h, r_p, pk, m_p);   
}

CH_ET_BC_CDK_2017::~CH_ET_BC_CDK_2017(){}
