#include <AE/RSA.h>

/**
 * @param pk public key
 * @param sk private key
 * @param k length of n
 */
void AE_RSA::KeyGen(RSA_pk *pk, RSA_sk *sk, short k) {
    mpz_t p, q, phi, n, e, d;
    mpz_inits(p, q, phi, n, e, d, NULL);

    RandomGenerator::RandomInLength(p, k / 2);
    mpz_nextprime(p, p); 

    RandomGenerator::RandomInLength(q, k / 2);
    mpz_nextprime(q, q);

    // Logger::PrintMpz("p", &p);
    // Logger::PrintMpz("q", &q);
    
    // n = p * q
    mpz_mul(n, p, q);
    pk->insertElement("n", n);

    // φ(n) = (p-1)*(q-1)
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(phi, p, q);

    // TODO
    // e = 65537
    mpz_set_ui(e, 65537);
    pk->insertElement("e", e);

    // d = e^(-1) mod φ(n)
    mpz_invert(d, e, phi);    
    sk->insertElement("d", d);

    mpz_clears(p, q, phi, n, e, d, NULL);
}

void AE_RSA::KeyGen(mpz_t p, mpz_t q, mpz_t n, mpz_t e, short k) {
    RandomGenerator::RandomInLength(p, k / 2);
    mpz_nextprime(p, p); 

    RandomGenerator::RandomInLength(q, k / 2);
    mpz_nextprime(q, q);
    
    // n = p * q
    mpz_mul(n, p, q);

    // TODO
    // e = 65537
    mpz_set_ui(e, 65537);
}

/**
 * @brief Generate RSA p,q only
 */
void AE_RSA::KeyGen(mpz_t p, mpz_t q, short k) {
    RandomGenerator::RandomInLength(p, k / 2);
    mpz_nextprime(p, p); 

    RandomGenerator::RandomInLength(q, k / 2);
    mpz_nextprime(q, q);
}

/**
 * @brief Generate RSA key pair s.t. e > n^exponent
 */
void AE_RSA::KeyGen(mpz_t p, mpz_t q, mpz_t n, mpz_t e, short k, short exponent) {
    mpz_t nk;
    mpz_init(nk);

    RandomGenerator::RandomInLength(p, k / 2);
    mpz_nextprime(p, p); 

    RandomGenerator::RandomInLength(q, k / 2);
    mpz_nextprime(q, q);
    
    // n = p * q
    mpz_mul(n, p, q);

    mpz_pow_ui(nk, n, exponent);
    mpz_nextprime(e, nk);

    mpz_clear(nk);
}

/**
 * @brief Generate RSA key pair s.t. e > n^exponent
 */
void AE_RSA::KeyGen(mpz_t n, mpz_t e, mpz_t d, short k, short exponent) {
    mpz_t nk,p,q,phi;
    mpz_inits(nk,p,q,phi,NULL);

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

    // d = e^(-1) mod φ(n)
    mpz_invert(d, e, phi); 

    mpz_clears(nk,p,q,phi,NULL);
}


void AE_RSA::KeyGen_E(mpz_t n, mpz_t d, mpz_t e, short k){
    mpz_t p, q, phi;
    mpz_inits(p, q, phi, NULL);

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

    mpz_invert(d, e, phi);  

    mpz_clears(p, q, phi, NULL);
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

