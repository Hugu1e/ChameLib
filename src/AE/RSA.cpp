#include <AE/RSA.h>

/**
 * @param pk public key
 * @param sk private key
 * @param k length of n
 */
void AE_RSA::KeyGen(RSA_pk *pk, RSA_sk *sk, short k) {
    mpz_t p, q, phi, n, e, d, gcdResult;
    mpz_inits(p, q, phi, n, e, d, gcdResult, NULL);

    // TODO
    // e = 65537
    mpz_set_ui(e, 65537);
    pk->insertElement("e", e);

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
    while(mpz_cmp_ui(gcdResult, 1) != 0){
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
    pk->insertElement("n", n);

    // d = e^(-1) mod φ(n)
    mpz_invert(d, e, phi);    
    sk->insertElement("d", d);

    mpz_clears(p, q, phi, n, e, d, gcdResult, NULL);
}


// /**
//  * @brief Generate RSA p,q only
//  */
// void AE_RSA::KeyGen(mpz_t p, mpz_t q, short k) {
//     RandomGenerator::RandomInLength(p, k / 2);
//     mpz_nextprime(p, p); 

//     RandomGenerator::RandomInLength(q, k / 2);
//     mpz_nextprime(q, q);
// }

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
    if(mpz_cmp_ui(gcdResult, 1) != 0){
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
void AE_RSA::Encrypt(mpz_t ciphertext, mpz_t plaintext, RSA_pk *pk) {
    mpz_t c;
    mpz_init(c);
    // c = m^e mod n
    mpz_powm(c, plaintext, pk->getElement("e"), pk->getElement("n"));
    mpz_set(ciphertext, c);
    mpz_clear(c);
}

/**
 * @brief Decrypt ciphertext
 * 
 * @param plaintext plaintext
 * @param ciphertext ciphertext
 * @param sk private key
 * @param pk public key
 */
void AE_RSA::Decrypt(mpz_t plaintext, mpz_t ciphertext, RSA_sk *sk, RSA_pk *pk) {
    mpz_t m;
    mpz_init(m);
    // m = c^d mod n
    mpz_powm(m, ciphertext, sk->getElement("d"), pk->getElement("n"));
    mpz_set(plaintext, m);
    mpz_clear(m);
}

