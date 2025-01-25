#include <scheme/CH/CH_ET_BC_CDK_2017.h>

CH_ET_BC_CDK_2017::CH_ET_BC_CDK_2017(){}


/**
 * @param pp: public parameters
 * @param k: length of n(RSA)
 */
void CH_ET_BC_CDK_2017::SetUp(CH_ET_BC_CDK_2017_pp *pp, int k){
    pp->setK(k);
}

/**
 * @param pk: public key
 * @param sk: secret key
 * @param pp: public parameters
 */
void CH_ET_BC_CDK_2017::KeyGen(CH_ET_BC_CDK_2017_pk *pk, CH_ET_BC_CDK_2017_sk *sk, CH_ET_BC_CDK_2017_pp *pp){
    RSA_pk rsa_pk;
    RSA_sk rsa_sk;
    rsa.KeyGen(&rsa_pk, &rsa_sk, pp->getK());

    pk->insertElement("n0", rsa_pk.getElement("n"));
    pk->insertElement("e0", rsa_pk.getElement("e"));
    sk->insertElement("d0", rsa_sk.getElement("d"));
}

/**
 * H(m) -> res mod n
 * @param res: result
 * @param m: message
 * @param n: modulus
 */
void CH_ET_BC_CDK_2017::H(mpz_t res, std::string m, mpz_t n){
    HASH::hash_n(res, m, n);
}

/**
 * @param h: hash value
 * @param r: random value
 * @param etd: ephemeral trapdoor
 * @param pp: public parameters
 * @param pk: public key
 * @param m: message
 */
void CH_ET_BC_CDK_2017::Hash(CH_ET_BC_CDK_2017_h *h, CH_ET_BC_CDK_2017_r *r, CH_ET_BC_CDK_2017_etd *etd, CH_ET_BC_CDK_2017_pp *pp, CH_ET_BC_CDK_2017_pk *pk, std::string m){
    mpz_t tmp1, n1,e1,d1, n0,e0,d0, r0,r1, h0,h1;
    mpz_inits(tmp1, n1, e1, d1, n0, e0, d0, r0, r1, h0, h1, NULL);

    RSA_pk rsa_pk;
    RSA_sk rsa_sk;
    rsa.KeyGen(&rsa_pk, &rsa_sk, pp->getK());
    h->insertElement("n1", rsa_pk.getElement("n"));
    h->insertElement("e1", rsa_pk.getElement("e"));
    etd->insertElement("d1", rsa_sk.getElement("d"));

    mpz_set(n0, pk->getElement("n0"));
    mpz_set(n1, h->getElement("n1"));
    mpz_set(e0, pk->getElement("e0"));
    mpz_set(e1, h->getElement("e1"));

    // r0,r1
    RandomGenerator::RandomCoprimeN(r0, n0);
    r->insertElement("r0", r0);
    RandomGenerator::RandomCoprimeN(r1, n1);
    r->insertElement("r1", r1);

    // h0 = H(m)r0^e0 mod n0
    H(tmp1, m, n0);
    mpz_powm(h0, r0, e0, n0);
    mpz_mul(h0, h0, tmp1);
    mpz_mod(h0, h0, n0);
    h->insertElement("h0", h0);
    // h1 = H(m)r1^e1 mod n1
    H(tmp1, m, n1);
    mpz_powm(h1, r1, e1, n1);
    mpz_mul(h1, h1, tmp1);
    mpz_mod(h1, h1, n1);
    h->insertElement("h1", h1);
    
    mpz_clears(tmp1, n1, e1, d1, n0, e0, d0, r0, r1, h0, h1, NULL);
}

/**
 * @param h: hash value
 * @param r: random value
 * @param pk: public key
 * @param m: message
 * @return bool
 */
bool CH_ET_BC_CDK_2017::Check(CH_ET_BC_CDK_2017_h *h, CH_ET_BC_CDK_2017_r* r, CH_ET_BC_CDK_2017_pk *pk, std::string m){
    bool CheckResult = false;
    mpz_t tmp1,tmp2, n0, r0, e0,h0, n1,r1,e1,h1;
    mpz_inits(tmp1, tmp2, n0, r0, e0, h0, n1, r1, e1, h1, NULL);

    mpz_set(n0, pk->getElement("n0"));
    mpz_set(r0, r->getElement("r0"));
    mpz_set(e0, pk->getElement("e0"));
    mpz_set(h0, h->getElement("h0"));
    mpz_set(n1, h->getElement("n1"));
    mpz_set(r1, r->getElement("r1"));
    mpz_set(e1, h->getElement("e1"));
    mpz_set(h1, h->getElement("h1"));

    // h0 = H(m)r0^e0 mod n0
    H(tmp1, m, n0);
    mpz_powm(tmp2, r0, e0, n0);
    mpz_mul(tmp2, tmp2, tmp1);
    mpz_mod(tmp2, tmp2, n0);
    if(mpz_cmp(tmp2, h0) != 0){
        mpz_clears(tmp1, tmp2, n0, r0, e0, h0, n1, r1, e1, h1, NULL);
        return false;
    }
    // h1 = H(m)r1^e1 mod n1
    H(tmp1, m, n1);
    mpz_powm(tmp2, r1, e1, n1);
    mpz_mul(tmp2, tmp2, tmp1);
    mpz_mod(tmp2, tmp2, n1);
    if(mpz_cmp(tmp2, h1) != 0){
        mpz_clears(tmp1, tmp2, n0, r0, e0, h0, n1, r1, e1, h1, NULL);
        return false;
    }
    
    mpz_clears(tmp1, tmp2, n0, r0, e0, h0, n1, r1, e1, h1, NULL);
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
void CH_ET_BC_CDK_2017::Adapt(CH_ET_BC_CDK_2017_r *r_p, CH_ET_BC_CDK_2017_sk *sk, CH_ET_BC_CDK_2017_etd *etd, CH_ET_BC_CDK_2017_pk *pk, CH_ET_BC_CDK_2017_h *h, CH_ET_BC_CDK_2017_r* r, std::string m, std::string m_p){
    if(!Check(h, r, pk, m)){
        throw std::runtime_error("Adapt(): Hash Check failed!");
    }
    mpz_t tmp1,n0, h0,r0,d0, n1,h1,r1,d1, r0_p,r1_p;
    mpz_inits(tmp1, n0, h0, r0, d0, n1, h1, r1, d1, r0_p, r1_p, NULL);

    mpz_set(n0, pk->getElement("n0"));
    mpz_set(h0, h->getElement("h0"));
    mpz_set(d0, sk->getElement("d0"));
    mpz_set(n1, h->getElement("n1"));
    mpz_set(h1, h->getElement("h1"));
    mpz_set(d1, etd->getElement("d1"));

    // r0' = (h0 * (H(m')^(-1)))^d0 mod n0
    H(tmp1, m_p, n0);
    mpz_invert(tmp1, tmp1, n0);
    mpz_mul(tmp1, h0, tmp1);
    mpz_mod(tmp1, tmp1, n0);
    mpz_powm(r0_p, tmp1, d0, n0);
    r_p->insertElement("r0", r0_p);
    // r1' = (h1 * (H(m')^(-1)))^d1 mod n1
    H(tmp1, m_p, n1);
    mpz_invert(tmp1, tmp1, n1);
    mpz_mul(tmp1, h1, tmp1);
    mpz_mod(tmp1, tmp1, n1);
    mpz_powm(r1_p, tmp1, d1, n1);
    r_p->insertElement("r1", r1_p);

    mpz_clears(tmp1, n0, h0, r0, d0, n1, h1, r1, d1, r0_p, r1_p, NULL);
}

/**
 * @param h: hash value
 * @param r_p: adapted random value
 * @param pk: public key
 * @param m_p: adapted message
 * @return bool
 */
bool CH_ET_BC_CDK_2017::Verify(CH_ET_BC_CDK_2017_h *h, CH_ET_BC_CDK_2017_r* r_p, CH_ET_BC_CDK_2017_pk *pk, std::string m_p){
    return Check(h, r_p, pk, m_p);   
}

CH_ET_BC_CDK_2017::~CH_ET_BC_CDK_2017(){}

