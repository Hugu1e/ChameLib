#include "scheme/CH/CH_KEF_MH_RSA_F_AM_2004.h"

CH_KEF_MH_RSA_F_AM_2004::CH_KEF_MH_RSA_F_AM_2004(){}

/**
 * @brief 
 * 
 * 
 */
void CH_KEF_MH_RSA_F_AM_2004::SetUp(){}

/**
 * @brief 
 * 
 * @param  pk[out]  
 * @param  sk[out]  
 * @param  k[in]    
 * @param  t[in]    
 * 
 */
void CH_KEF_MH_RSA_F_AM_2004::KeyGen(CH_KEF_MH_RSA_F_AM_2004_pk &pk, CH_KEF_MH_RSA_F_AM_2004_sk &sk, int k, int t){
    this->k = k;
    this->t = t;
    // generate e s.t. e > 2^t
    RandomGenerator::RandomInLength(pk[e], t+1);
    mpz_nextprime(pk[e], pk[e]);
    
    rsa.KeyGen_E(pk[n],sk[d],pk[e],k);
}

void CH_KEF_MH_RSA_F_AM_2004::H(mpz_t res, mpz_t m, int t){
    mpz_t n;
    mpz_init(n);
    // n = 2^t
    mpz_ui_pow_ui(n, 2, t);
    HASH::hash_n(res, m, n);
    mpz_clear(n);  
}

/**
 * @brief 
 * 
 * @param  h[out]    
 * @param  r[out]    
 * @param  B[out]    secret trapdoor
 * @param  pk[in]  
 * @param  sk[in]  
 * @param  m[in]    
 * @param  tag[in]  
 * 
 */
void CH_KEF_MH_RSA_F_AM_2004::Hash(mpz_t h, mpz_t r, mpz_t B, CH_KEF_MH_RSA_F_AM_2004_pk &pk, CH_KEF_MH_RSA_F_AM_2004_sk &sk, mpz_t m, mpz_t tag){
    mpz_t J,tmp,tmp_2;
    mpz_inits(J,tmp,tmp_2,NULL);
    // J = C(L), C : {0, 1}∗ → {0, · · · , 2^(2κ−1)}
    this->H(J, tag, 2 * this->k - 1);

    // secret trapdoor B = J ^ d mod n
    mpz_powm(B, J, sk[d], pk[n]);

    // r
    RandomGenerator::RandomN(r, pk[n]);

    // tmp = H(m)
    this->H(tmp, m, this->t);
    // J^H(m) * r^e mod n
    mpz_powm(tmp, J, tmp, pk[n]);
    mpz_powm(tmp_2, r, pk[e], pk[n]);
    mpz_mul(h, tmp, tmp_2);
    mpz_mod(h, h, pk[n]);

    mpz_clears(J,tmp,tmp_2,NULL);
}

/**
 * @brief 
 * 
 * @param  pk[in]  
 * @param  m[in]    
 * @param  tag[in]  
 * @param  h[in]    
 * @param  r[in]    
 * 
 * @return 
 */
bool CH_KEF_MH_RSA_F_AM_2004::Check(CH_KEF_MH_RSA_F_AM_2004_pk &pk, mpz_t m, mpz_t tag, mpz_t h, mpz_t r){
    mpz_t J,tmp,tmp_2,tmp_3;
    mpz_inits(J,tmp,tmp_2,tmp_3,NULL);
    // J = C(L), C : {0, 1}∗ → {0, · · · , 2^(2κ−1)}
    this->H(J, tag, 2 * this->k - 1);

    // tmp = H(m)
    this->H(tmp, m, this->t);
    // J^H(m) * r^e mod n
    mpz_powm(tmp, J, tmp, pk[n]);
    mpz_powm(tmp_2, r, pk[e], pk[n]);
    mpz_mul(tmp_3, tmp, tmp_2);
    mpz_mod(tmp_3, tmp_3, pk[n]);

    if(mpz_cmp(h, tmp_3) == 0){
        mpz_clears(J,tmp,tmp_2,tmp_3,NULL);
        return true;
    }else{
        mpz_clears(J,tmp,tmp_2,tmp_3,NULL);
        return false;
    }
}

/**
 * @brief 
 * 
 * @param  r_p[out]  
 * @param  pk[in]  
 * @param  m[in]    
 * @param  m_p[in]  
 * @param  tag[in]  
 * @param  h[in]    
 * @param  B[in]    
 * @param  r[in]    
 * 
 */
void CH_KEF_MH_RSA_F_AM_2004::Adapt(mpz_t r_p, CH_KEF_MH_RSA_F_AM_2004_pk &pk, mpz_t m, mpz_t m_p, mpz_t tag, mpz_t h, mpz_t B, mpz_t r){
    mpz_t tmp,tmp_2;
    mpz_inits(tmp,tmp_2,NULL);

    // r' = r * B^(H(m) - H(m')) mod n
    this->H(tmp, m, this->t);
    this->H(tmp_2, m_p, this->t);
    mpz_sub(tmp, tmp, tmp_2);
    mpz_powm(tmp, B, tmp, pk[n]);
    mpz_mul(r_p, r, tmp);
    mpz_mod(r_p, r_p, pk[n]);

    mpz_clears(tmp,tmp_2,NULL);
}

/**
 * @brief 
 * 
 * @param  pk[in]  
 * @param  m_p[in]  
 * @param  tag[in]  
 * @param  h[in]    
 * @param  r_p[in]  
 * 
 * @return 
 */
bool CH_KEF_MH_RSA_F_AM_2004::Verify(CH_KEF_MH_RSA_F_AM_2004_pk &pk, mpz_t m_p, mpz_t tag, mpz_t h, mpz_t r_p){
    return this->Check(pk, m_p, tag, h, r_p);
}

CH_KEF_MH_RSA_F_AM_2004::~CH_KEF_MH_RSA_F_AM_2004(){}
