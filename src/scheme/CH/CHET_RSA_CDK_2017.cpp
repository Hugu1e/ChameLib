#include "scheme/CH/CHET_RSA_CDK_2017.h"

CHET_RSA_CDK_2017::CHET_RSA_CDK_2017(){}

/**
 * @brief 
 * 
 * 
 */
void CHET_RSA_CDK_2017::SetUp(){}

/**
 * @brief 
 * 
 * @param  pk[out]  
 * @param  sk[out]  
 * @param  k[in]    
 * 
 */
void CHET_RSA_CDK_2017::KeyGen(CHET_RSA_CDK_2017_pk &pk, CHET_RSA_CDK_2017_sk &sk, short k){
    // e > n^3
    rsa.KeyGen(sk[p], sk[q], pk[n], pk[e], k, 3);
}

void CHET_RSA_CDK_2017::H(mpz_t res ,mpz_t m, mpz_t n){
    HASH::hash_n(res, m, n);  
}

/**
 * @brief 
 * 
 * @param  h[out]    
 * @param  r[out]     
 * @param  etd[out]  
 * @param  m[in]     
 * @param  pk[in]   
 * 
 */
void CHET_RSA_CDK_2017::Hash(CHET_RSA_CDK_2017_h &h, mpz_t r, CHET_RSA_CDK_2017_etd &etd, mpz_t m, CHET_RSA_CDK_2017_pk &pk){
    mpz_t etd_p, etd_q, etd_n, gcd_result, phi_etd, tmp_etd_p, tmp_etd_q, gcd_result_2,  tmp_nn, g, tmp,  hash;
    mpz_inits(etd_p, etd_q, etd_n, gcd_result, phi_etd, tmp_etd_p, tmp_etd_q, gcd_result_2, tmp_nn, g, tmp, hash, NULL);
    
    rsa.KeyGen_E(etd_p, etd_q, etd_n, pk[e], 1024);
    mpz_gcd(gcd_result, pk[n], etd_n);
    while (mpz_cmp_ui(gcd_result, 1) != 0)
    {
        rsa.KeyGen_E(etd_p, etd_q, etd_n, pk[e], 1024);
        mpz_gcd(gcd_result, pk[n], etd_n);
    }
    etd.set(p, etd_p);
    etd.set(q, etd_q);

    // Draw r ← Znn'*
    mpz_mul(tmp_nn, pk[n], etd_n);
    RandomGenerator::RandomCoprimeN(r, tmp_nn);

    // Let g ← Hnn' (m), and h ← gr^e mod nn'.
    this->H(g, m, tmp_nn);
    
    // h ← gr^e mod nn'
    mpz_powm(tmp, r, pk[e], tmp_nn);
    mpz_mul(hash, g, tmp);
    mpz_mod(hash, hash, tmp_nn);

    h.set(h1, hash);
    h.set(n_, etd_n);

    mpz_clears(etd_p, etd_q, etd_n, gcd_result, phi_etd, tmp_etd_p, tmp_etd_q, gcd_result_2, tmp_nn, g, tmp, hash, NULL);
}

/**
 * @brief 
 * 
 * @param  h[in]   
 * @param  r[in]    
 * @param  m[in]    
 * @param  pk[in]  
 * 
 * @return 
 */
bool CHET_RSA_CDK_2017::Check(CHET_RSA_CDK_2017_h &h, mpz_t r, mpz_t m, CHET_RSA_CDK_2017_pk &pk){
    // If r !∈ Znn'*, return false
    mpz_t tmp_nn,gcd_result, g, tmp, tmp_2;
    mpz_inits(tmp_nn, gcd_result, g, tmp, tmp_2, NULL);
    mpz_mul(tmp_nn, pk[n], h[n_]);
    mpz_gcd(gcd_result, r, tmp_nn);
    if(mpz_cmp(r, tmp_nn) >= 0 || mpz_cmp_ui(gcd_result, 1) != 0){
        return false;
    }

    // Let g ← Hnn' (m), and h ← gr^e mod nn'.
    this->H(g, m, tmp_nn);
    
    // h ← gr^e mod n
    mpz_powm(tmp, r, pk[e], tmp_nn);
    mpz_mul(tmp_2, g, tmp);
    mpz_mod(tmp_2, tmp_2, tmp_nn);

    if(mpz_cmp(tmp_2, h[h1]) == 0){
        mpz_clears(tmp_nn, gcd_result, g, tmp, tmp_2, NULL);
        return true;
    }else{
        mpz_clears(tmp_nn, gcd_result, g, tmp, tmp_2, NULL);
        return false;
    }
}

/**
 * @brief 
 * 
 * @param  r_p[out]   
 * @param  m_p[in]   
 * @param  m[in]     
 * @param  r[in]     
 * @param  h[in]    
 * @param  sk[in]   
 * @param  etd[in]  
 * @param  pk[in]   
 * 
 */
void CHET_RSA_CDK_2017::Adapt(mpz_t r_p, mpz_t m_p, mpz_t m, mpz_t r, CHET_RSA_CDK_2017_h &h, CHET_RSA_CDK_2017_sk &sk, CHET_RSA_CDK_2017_etd &etd, CHET_RSA_CDK_2017_pk &pk){
    // Check that n0 = p0q0, where p0 and q0 is taken from etd. If this is not thecase, return ⊥.
    mpz_t tmp, tmp_nn,tmp_d, etd_p_minus_1, etd_q_minus_1, p_minus_1, q_minus_1,phi, g_p,tmp_1,tmp_2;
    mpz_inits(tmp, tmp_nn,tmp_d, etd_p_minus_1, etd_q_minus_1, p_minus_1, q_minus_1,phi, g_p,tmp_1,tmp_2, NULL);

    mpz_mul(tmp, etd[p], etd[q]);
    if(mpz_cmp(tmp, h[n_]) != 0){
        throw std::runtime_error("CHET_RSA_CDK_2017::Adapt(): invalid trapdoor");
    }
    
    // If CHashCheck(pkch, m, r, h) = false, return ⊥.
    if(this->Check(h, r, m, pk) == false){
        throw std::runtime_error("CHET_RSA_CDK_2017::Adapt(): hash check failed");
    }

    // Compute d s.t. de ≡ 1 mod ϕ(nn').
    mpz_mul(tmp_nn, pk[n], h[n_]);
    // ϕ(nn') = (p-1)(q-1)(etd_p-1)(etd_q-1)
    mpz_sub_ui(etd_p_minus_1, etd[p], 1);
    mpz_sub_ui(etd_q_minus_1, etd[q], 1);
    mpz_sub_ui(p_minus_1, sk[p], 1);
    mpz_sub_ui(q_minus_1, sk[q], 1);
    mpz_mul(phi, etd_p_minus_1, etd_q_minus_1);
    mpz_mul(phi, phi, p_minus_1);
    mpz_mul(phi, phi, q_minus_1);
    // compute d
    mpz_invert(tmp_d, pk[e], phi);

    // // 验证 d 计算是否正确
    // mpz_t tmp_test;
    // mpz_init(tmp_test);
    // mpz_mul(tmp_test, e, tmp_d);
    // mpz_mod(tmp_test, tmp_test, phi);
    // gmp_printf("tmp_test: %Zd\n", tmp_test);
    // if(mpz_cmp_ui(tmp_test, 1) != 0){
    //     return false;
    // }
    
    // gmp_printf("tmp_d: %Zd\n", tmp_d);
    // gmp_printf("phi: %Zd\n", phi);

    // Let g' ← Hnn' (m') and r' ← (h(g'−1))d mod nn0. 
    this->H(g_p, m_p, tmp_nn);
 
    mpz_invert(tmp_1, g_p, tmp_nn);  
    mpz_mul(tmp_2, h[h1], tmp_1);
    mpz_mod(tmp_2, tmp_2, tmp_nn);
    mpz_powm(r_p, tmp_2, tmp_d, tmp_nn);

    mpz_clears(tmp, tmp_nn,tmp_d, n,e, etd_p_minus_1, etd_q_minus_1, p_minus_1, q_minus_1,phi, g_p,tmp_1,tmp_2, NULL);
}

/**
 * @brief 
 * 
 * @param  h[in]   
 * @param  r_p[in]  
 * @param  m_p[in]  
 * @param  pk[in]  
 * 
 * @return 
 */
bool CHET_RSA_CDK_2017::Verify(CHET_RSA_CDK_2017_h &h, mpz_t r_p, mpz_t m_p, CHET_RSA_CDK_2017_pk &pk){
    return this->Check(h, r_p, m_p, pk);
}

CHET_RSA_CDK_2017::~CHET_RSA_CDK_2017(){}