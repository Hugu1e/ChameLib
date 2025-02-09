#include <scheme/CH/CH_ET_BC_CDK_2017.h>

CH_ET_BC_CDK_2017::CH_ET_BC_CDK_2017(){}


/**
 * @param pp: public parameters
 * @param pk: public key
 * @param sk: secret key
 * @param h: hash value
 * @param r: random value
 * @param r_p: adapted random value
 * @param etd: ephemeral trapdoor
 * @param k: length of n(RSA)
 */
void CH_ET_BC_CDK_2017::SetUp(CH_ET_BC_CDK_2017_pp &pp, CH_ET_BC_CDK_2017_pk &pk, CH_ET_BC_CDK_2017_sk &sk, CH_ET_BC_CDK_2017_h &h, CH_ET_BC_CDK_2017_r &r, CH_ET_BC_CDK_2017_r &r_p, CH_ET_BC_CDK_2017_etd &etd, int k){
    pk.init(2);
    sk.init(1);
    h.init(4);
    r.init(2);
    r_p.init(2);
    etd.init(1);
    
    pp.setK(k);
}

/**
 * @param pk: public key
 * @param sk: secret key
 * @param pp: public parameters
 */
void CH_ET_BC_CDK_2017::KeyGen(CH_ET_BC_CDK_2017_pk &pk, CH_ET_BC_CDK_2017_sk &sk, CH_ET_BC_CDK_2017_pp &pp){
    RSA_pk rsa_pk;
    RSA_sk rsa_sk;
    rsa.SetUp(rsa_pk, rsa_sk);
    rsa.KeyGen(rsa_pk, rsa_sk, pp.getK());

    pk.set(n0, rsa_pk[AE_RSA::n]);
    pk.set(e0, rsa_pk[AE_RSA::e]);
    sk.set(d0, rsa_sk[AE_RSA::d]);
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
void CH_ET_BC_CDK_2017::Hash(CH_ET_BC_CDK_2017_h &h, CH_ET_BC_CDK_2017_r &r, CH_ET_BC_CDK_2017_etd &etd, CH_ET_BC_CDK_2017_pp &pp, CH_ET_BC_CDK_2017_pk &pk, std::string m){
    mpz_t tmp1;
    mpz_inits(tmp1, NULL);

    RSA_pk rsa_pk;
    RSA_sk rsa_sk;
    rsa.SetUp(rsa_pk, rsa_sk);
    rsa.KeyGen(rsa_pk, rsa_sk, pp.getK());
    h.set(n1, rsa_pk[AE_RSA::n]);
    h.set(e1, rsa_pk[AE_RSA::e]);
    etd.set(d1, rsa_sk[AE_RSA::d]);

    // r0,r1
    RandomGenerator::RandomCoprimeN(r[r0], pk[n0]);
    RandomGenerator::RandomCoprimeN(r[r1], h[n1]);

    // h0 = H(m)r0^e0 mod n0
    H(tmp1, m, pk[n0]);
    mpz_powm(h[h0], r[r0], pk[e0], pk[n0]);
    mpz_mul(h[h0], h[h0], tmp1);
    mpz_mod(h[h0], h[h0], pk[n0]);
    // h1 = H(m)r1^e1 mod n1
    H(tmp1, m, h[n1]);
    mpz_powm(h[h1], r[r1], h[e1], h[n1]);
    mpz_mul(h[h1], h[h1], tmp1);
    mpz_mod(h[h1], h[h1], h[n1]);
    
    mpz_clears(tmp1, NULL);
}

/**
 * @param h: hash value
 * @param r: random value
 * @param pk: public key
 * @param m: message
 * @return bool
 */
bool CH_ET_BC_CDK_2017::Check(CH_ET_BC_CDK_2017_h &h, CH_ET_BC_CDK_2017_r &r, CH_ET_BC_CDK_2017_pk &pk, std::string m){
    bool CheckResult = false;
    mpz_t tmp1,tmp2;
    mpz_inits(tmp1, tmp2, NULL);

    // h0 = H(m)r0^e0 mod n0
    H(tmp1, m, pk[n0]);
    mpz_powm(tmp2, r[r0], pk[e0], pk[n0]);
    mpz_mul(tmp2, tmp2, tmp1);
    mpz_mod(tmp2, tmp2, pk[n0]);
    if(mpz_cmp(tmp2, h[h0]) != 0){
        mpz_clears(tmp1, tmp2, NULL);
        return false;
    }
    // h1 = H(m)r1^e1 mod n1
    H(tmp1, m, h[n1]);
    mpz_powm(tmp2, r[r1], h[e1], h[n1]);
    mpz_mul(tmp2, tmp2, tmp1);
    mpz_mod(tmp2, tmp2, h[n1]);
    if(mpz_cmp(tmp2, h[h1]) != 0){
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
void CH_ET_BC_CDK_2017::Adapt(CH_ET_BC_CDK_2017_r &r_p, CH_ET_BC_CDK_2017_sk &sk, CH_ET_BC_CDK_2017_etd &etd, CH_ET_BC_CDK_2017_pk &pk, CH_ET_BC_CDK_2017_h &h, CH_ET_BC_CDK_2017_r &r, std::string m, std::string m_p){
    if(!Check(h, r, pk, m)){
        throw std::runtime_error("CH_ET_BC_CDK_2017::Adapt(): Hash Check failed");
    }
    mpz_t tmp1;
    mpz_inits(tmp1, NULL);


    // r0' = (h0 * (H(m')^(-1)))^d0 mod n0
    H(tmp1, m_p, pk[n0]);
    mpz_invert(tmp1, tmp1, pk[n0]);
    mpz_mul(tmp1, h[h0], tmp1);
    mpz_mod(tmp1, tmp1, pk[n0]);
    mpz_powm(r_p[r0], tmp1, sk[d0], pk[n0]);
    // r1' = (h1 * (H(m')^(-1)))^d1 mod n1
    H(tmp1, m_p, h[n1]);
    mpz_invert(tmp1, tmp1, h[n1]);
    mpz_mul(tmp1, h[h1], tmp1);
    mpz_mod(tmp1, tmp1, h[n1]);
    mpz_powm(r_p[r1], tmp1, etd[d1], h[n1]);

    mpz_clears(tmp1, NULL);
}

/**
 * @param h: hash value
 * @param r_p: adapted random value
 * @param pk: public key
 * @param m_p: adapted message
 * @return bool
 */
bool CH_ET_BC_CDK_2017::Verify(CH_ET_BC_CDK_2017_h &h, CH_ET_BC_CDK_2017_r &r_p, CH_ET_BC_CDK_2017_pk &pk, std::string m_p){
    return Check(h, r_p, pk, m_p);   
}

CH_ET_BC_CDK_2017::~CH_ET_BC_CDK_2017(){}

