#include "AE/RSA.h"

void AE_RSA::SetUp(){}

void AE_RSA::SetUp(RSA_pk &pk, RSA_sk &sk){
    pk.init(2);
    sk.init(1);
}

void AE_RSA::SetUp(RSA_pk &pk, RSA_sk &sk, RSA_m &m){
    pk.init(2);
    sk.init(1);
    m.init(1);
}

/**
 * @param pk public key
 * @param sk private key
 * @param k length of n
 */
void AE_RSA::KeyGen(RSA_pk &pk, RSA_sk &sk, short k) {
    mpz_t p, q, phi, gcdResult;
    mpz_inits(p, q, phi, gcdResult, NULL);

    // TODO
    // e = 65537
    mpz_set_ui(pk[e], 65537);

    RandomGenerator::RandomInLength(p, k / 2);
    mpz_nextprime(p, p); 
    RandomGenerator::RandomInLength(q, k / 2);
    mpz_nextprime(q, q);
    // n = p * q
    mpz_mul(pk[n], p, q);
    // φ(n) = (p-1)*(q-1)
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(phi, p, q);

    // gcd(phi, e) == 1
    mpz_gcd(gcdResult, pk[e], phi);
    while(mpz_cmp_ui(gcdResult, 1) != 0){
        RandomGenerator::RandomInLength(p, k / 2);
        mpz_nextprime(p, p); 
        RandomGenerator::RandomInLength(q, k / 2);
        mpz_nextprime(q, q);
        // n = p * q
        mpz_mul(pk[n], p, q);
        // φ(n) = (p-1)*(q-1)
        mpz_sub_ui(p, p, 1);
        mpz_sub_ui(q, q, 1);
        mpz_mul(phi, p, q);

        mpz_gcd(gcdResult, pk[e], phi);
    }

    // d = e^(-1) mod φ(n)
    mpz_invert(sk[d], pk[e], phi);    
    sk.set(d, sk[d]);

    mpz_clears(p, q, phi, gcdResult, NULL);
}

/**
 * @brief Generate RSA key pair s.t. e > n^exponent
 */
void AE_RSA::KeyGen(mpz_t p, mpz_t q, mpz_t n, mpz_t e, short k, short exponent) {
    mpz_t nk, phi, gcdResult, tmp_p, tmp_q;
    mpz_inits(nk, phi, gcdResult, tmp_p, tmp_q, NULL);

    RandomGenerator::RandomInLength(p, k / 2);
    mpz_nextprime(p, p); 
    RandomGenerator::RandomInLength(q, k / 2);
    mpz_nextprime(q, q);
    // n = p * q
    mpz_mul(n, p, q);
    // φ(n) = (p-1)*(q-1)
    mpz_sub_ui(tmp_p, p, 1);
    mpz_sub_ui(tmp_q, q, 1);
    mpz_mul(phi, tmp_p, tmp_q);

    mpz_pow_ui(nk, n, exponent);
    mpz_nextprime(e, nk);

    // gcd(phi, e) == 1
    mpz_gcd(gcdResult, e, phi);
    while(mpz_cmp_ui(gcdResult, 1) != 0){
        RandomGenerator::RandomInLength(p, k / 2);
        mpz_nextprime(p, p); 
        RandomGenerator::RandomInLength(q, k / 2);
        mpz_nextprime(q, q);
        // n = p * q
        mpz_mul(n, p, q);
        // φ(n) = (p-1)*(q-1)
        mpz_sub_ui(tmp_p, p, 1);
        mpz_sub_ui(tmp_q, q, 1);
        mpz_mul(phi, tmp_p, tmp_q);

        mpz_pow_ui(nk, n, exponent);
        mpz_nextprime(e, nk);

        mpz_gcd(gcdResult, e, phi);
    }

    mpz_clears(nk, phi, gcdResult, tmp_p, tmp_q, NULL);
}

/**
 * @brief Generate RSA key pair s.t. e > n^exponent
 */
void AE_RSA::KeyGen(mpz_t n, mpz_t e, mpz_t d, short k, short exponent) {
    mpz_t nk,p,q,phi, gcdResult;
    mpz_inits(nk,p,q,phi, gcdResult,NULL);

    RandomGenerator::RandomInLength(p, k / 2);
    mpz_nextprime(p, p); 

    RandomGenerator::RandomInLength(q, k / 2);
    mpz_nextprime(q, q);
    
    // n = p * q
    mpz_mul(n, p, q);

    // φ(n) = (p-1)*(q-1)
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(phi, p, q);

    mpz_pow_ui(nk, n, exponent);
    mpz_nextprime(e, nk);

    // gcd(phi, e) == 1
    mpz_gcd(gcdResult, e, phi);
    while(mpz_cmp_ui(gcdResult, 1) != 0){
        mpz_nextprime(e, e);

        mpz_gcd(gcdResult, e, phi);
    }

    // d = e^(-1) mod φ(n)
    mpz_invert(d, e, phi); 

    mpz_clears(nk,p,q,phi, gcdResult,NULL);
}

/**
 * @brief Generate RSA key pair with bit of e and p
 * 
 * @param pk[out] public key
 * @param sk[out] private key
 * @param ebit[in] bit of e
 * @param pbit[in] bit of p
 */
void AE_RSA::KeyGen(RSA_pk &pk, RSA_sk &sk, short ebit, short pbit){
    mpz_t e_, p, q, phi, gcdResult, n_, d_;
    mpz_inits(e_, p, q, phi, gcdResult, n_, d_, NULL);

    RandomGenerator::RandomPrimeInLength(e_, ebit);
    RandomGenerator::RandomPrimeInLength(p, pbit);
    RandomGenerator::RandomPrimeInLength(q, pbit);

    ComputePhi(phi, p, q);
    mpz_gcd(gcdResult, e_, phi);
    while(mpz_cmp_ui(gcdResult, 1) != 0){
        RandomGenerator::RandomPrimeInLength(e_, ebit);
        RandomGenerator::RandomPrimeInLength(p, pbit);
        RandomGenerator::RandomPrimeInLength(q, pbit);

        ComputePhi(phi, p, q);
        mpz_gcd(gcdResult, e_, phi);
    }

    mpz_mul(n_, p, q);
    mpz_invert(d_, e_, phi);

    pk.set(e, e_);
    pk.set(n, n_);
    sk.set(d, d_);

    mpz_clears(e_, p, q, phi, gcdResult, n_, d_, NULL);
}

/**
 * @brief Compute φ(n) = (p-1)*(q-1)
 */
void AE_RSA::ComputePhi(mpz_t phi, mpz_t p, mpz_t q){
    mpz_t p_1, q_1;
    mpz_inits(p_1, q_1, NULL);
    mpz_sub_ui(p_1, p, 1);
    mpz_sub_ui(q_1, q, 1);
    mpz_mul(phi, p_1, q_1);
    mpz_clears(p_1, q_1, NULL);
}

/**
 * @brief Generate RSA key pair with e
 * 
 * @param n[out] n = p * q
 * @param d[out] d = e^(-1) mod φ(n)
 * @param e[in] public key e
 * @param k[in] length of n
 */
void AE_RSA::KeyGen_E(mpz_t n, mpz_t d, mpz_t e, short k){
    mpz_t p, q, phi, gcdResult;
    mpz_inits(p, q, phi, gcdResult, NULL);

    RandomGenerator::RandomInLength(p, k / 2);
    mpz_nextprime(p, p); 
    RandomGenerator::RandomInLength(q, k / 2);
    mpz_nextprime(q, q);
    // n = p * q
    mpz_mul(n, p, q);
    // φ(n) = (p-1)*(q-1)
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(phi, p, q);

    // gcd(phi, e) == 1
    mpz_gcd(gcdResult, e, phi);
    if(mpz_cmp_ui(gcdResult, 1) != 0){
        RandomGenerator::RandomInLength(p, k / 2);
        mpz_nextprime(p, p); 
        RandomGenerator::RandomInLength(q, k / 2);
        mpz_nextprime(q, q);
        // n = p * q
        mpz_mul(n, p, q);
        // φ(n) = (p-1)*(q-1)
        mpz_sub_ui(p, p, 1);
        mpz_sub_ui(q, q, 1);
        mpz_mul(phi, p, q);

        mpz_gcd(gcdResult, e, phi);
    }
    
    mpz_invert(d, e, phi);  

    mpz_clears(p, q, phi, gcdResult, NULL);
}

/**
 * @brief Generate RSA key pair with e
 * 
 * @param p[out] prime number p
 * @param q[out] prime number q
 * @param n[out] n = p * q
 * @param e[in] public key e
 * @param k[in] length of n
 */
void AE_RSA::KeyGen_E(mpz_t p, mpz_t q, mpz_t n, mpz_t e, short k){
    mpz_t tmp_p, tmp_q, phi, gcdResult;
    mpz_inits(tmp_p, tmp_q, phi, gcdResult, NULL);

    RandomGenerator::RandomInLength(p, k / 2);
    mpz_nextprime(p, p); 
    RandomGenerator::RandomInLength(q, k / 2);
    mpz_nextprime(q, q);
    // n = p * q
    mpz_mul(n, p, q);
    // φ(n) = (p-1)*(q-1)
    mpz_sub_ui(tmp_p, p, 1);
    mpz_sub_ui(tmp_q, q, 1);
    mpz_mul(phi, tmp_p, tmp_q);

    // gcd(phi, e) == 1
    mpz_gcd(gcdResult, e, phi);
    while(mpz_cmp_ui(gcdResult, 1) != 0){
        RandomGenerator::RandomInLength(p, k / 2);
        mpz_nextprime(p, p); 
        RandomGenerator::RandomInLength(q, k / 2);
        mpz_nextprime(q, q);
        // n = p * q
        mpz_mul(n, p, q);
        // φ(n) = (p-1)*(q-1)
        mpz_sub_ui(tmp_p, p, 1);
        mpz_sub_ui(tmp_q, q, 1);
        mpz_mul(phi, tmp_p, tmp_q);

        mpz_gcd(gcdResult, e, phi);
    }
    
    mpz_clears(tmp_p, tmp_q, phi, gcdResult, NULL);
}

/**
 * @brief Encrypt plaintext
 * 
 * @param ciphertext ciphertext
 * @param plaintext plaintext
 * @param pk public key
 */
void AE_RSA::Encrypt(mpz_t ciphertext, mpz_t plaintext, RSA_pk &pk) {
    mpz_t c;
    mpz_init(c);
    // c = m^e mod n
    mpz_powm(c, plaintext, pk[e], pk[n]);
    mpz_set(ciphertext, c);
    mpz_clear(c);
}

void AE_RSA::Encrypt(RSA_m &ciphertext, mpz_t plaintext, RSA_pk &pk){
    Encrypt(ciphertext[0], plaintext, pk);
}

/**
 * @brief Decrypt ciphertext
 * 
 * @param plaintext plaintext
 * @param ciphertext ciphertext
 * @param sk private key
 * @param pk public key
 */
void AE_RSA::Decrypt(mpz_t plaintext, mpz_t ciphertext, RSA_sk &sk, RSA_pk &pk) {
    mpz_t m;
    mpz_init(m);
    // m = c^d mod n
    mpz_powm(m, ciphertext, sk[d], pk[n]);
    mpz_set(plaintext, m);
    mpz_clear(m);
}

void AE_RSA::Decrypt(mpz_t plaintext, RSA_m &ciphertext, RSA_sk &sk, RSA_pk &pk){
    Decrypt(plaintext, ciphertext[0], sk, pk);
}
