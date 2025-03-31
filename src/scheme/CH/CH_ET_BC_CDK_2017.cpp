#include "scheme/CH/CH_ET_BC_CDK_2017.h"

CH_ET_BC_CDK_2017::CH_ET_BC_CDK_2017(){}

/**
 * @brief 
 * 
 * @param  pp[out]  public parameters
 * @param  k[in]    length of n(RSA)
 * 
 */
void CH_ET_BC_CDK_2017::SetUp(CH_ET_BC_CDK_2017_pp &pp, int k){
    pp.setK(k);
}

/**
 * @brief 
 * 
 * @param  pk[out]  public key
 * @param  sk[out]  secret key
 * @param  pp[in]  public parameters
 * 
 */
void CH_ET_BC_CDK_2017::KeyGen(CH_ET_BC_CDK_2017_pk &pk, CH_ET_BC_CDK_2017_sk &sk, CH_ET_BC_CDK_2017_pp &pp){
    RSA_pk rsa_pk;
    RSA_sk rsa_sk;
    rsa.KeyGen(rsa_pk, rsa_sk, pp.getK());

    pk.set(n0, rsa_pk[AE_RSA::n]);
    pk.set(e0, rsa_pk[AE_RSA::e]);
    sk.set(d0, rsa_sk[AE_RSA::d]);
}

void CH_ET_BC_CDK_2017::H(mpz_t res, std::string m, mpz_t n){
    HASH::hash_n(res, m, n);
}

/**
 * @brief 
 * 
 * @param  h[out]    hash value
 * @param  r[out]    random value
 * @param  etd[out]  ephemeral trapdoor
 * @param  pp[in]   public parameters
 * @param  pk[in]   public key
 * @param  m[in]     message
 * 
 */
void CH_ET_BC_CDK_2017::Hash(CH_ET_BC_CDK_2017_h &h, CH_ET_BC_CDK_2017_r &r, CH_ET_BC_CDK_2017_etd &etd, CH_ET_BC_CDK_2017_pp &pp, CH_ET_BC_CDK_2017_pk &pk, std::string m){
    mpz_t tmp1;
    mpz_inits(tmp1, NULL);

    RSA_pk rsa_pk;
    RSA_sk rsa_sk;
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
 * @brief 
 * 
 * @param  h[in]   hash value
 * @param  r[in]   random value
 * @param  pk[in]  public key
 * @param  m[in]    message
 * 
 * @return 
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
 * @brief 
 * 
 * @param  r_p[out]  adapted random value
 * @param  sk[in]   secret key
 * @param  etd[in]  ephemeral trapdoor
 * @param  pk[in]   public key
 * @param  h[in]    hash value
 * @param  r[in]    random value
 * @param  m[in]     message
 * @param  m_p[in]   adapted message
 * 
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
 * @brief 
 * 
 * @param  h[in]    hash value
 * @param  r_p[in]  adapted random value
 * @param  pk[in]   public key
 * @param  m_p[in]   adapted message
 * 
 * @return 
 */
bool CH_ET_BC_CDK_2017::Verify(CH_ET_BC_CDK_2017_h &h, CH_ET_BC_CDK_2017_r &r_p, CH_ET_BC_CDK_2017_pk &pk, std::string m_p){
    return Check(h, r_p, pk, m_p);   
}

CH_ET_BC_CDK_2017::~CH_ET_BC_CDK_2017(){}

