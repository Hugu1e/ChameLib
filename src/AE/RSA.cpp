#include <AE/RSA.h>

/**
 * @param pk public key
 * @param sk private key
 * @param k length of n
 */
void AE_RSA::KeyGen(pk *pk, sk *sk, short k) {
    mpz_t p, q, phi;
    mpz_inits(p, q, phi, NULL);

    RandomGenerator::RandomInLength(&p, k / 2);
    mpz_nextprime(p, p); 

    RandomGenerator::RandomInLength(&q, k / 2);
    mpz_nextprime(q, q);

    // Logger::PrintMpz("p", &p);
    // Logger::PrintMpz("q", &q);
    
    // n = p * q
    mpz_mul(pk->n, p, q);

    // φ(n) = (p-1)*(q-1)
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(phi, p, q);

    // TODO
    // e = 65537
    mpz_set_ui(pk->e, 65537);
    // d = e^(-1) mod φ(n)
    mpz_invert(sk->d, pk->e, phi);    

    mpz_clears(p, q, phi, NULL);
}

/**
 * @brief Encrypt plaintext
 * 
 * @param ciphertext ciphertext
 * @param plaintext plaintext
 * @param pk public key
 */
void AE_RSA::Encrypt(mpz_t *ciphertext, const mpz_t *plaintext, pk *pk) {
    // c = m^e mod n
    mpz_powm(*ciphertext, *plaintext, pk->e, pk->n);
}

/**
 * @brief Decrypt ciphertext
 * 
 * @param plaintext plaintext
 * @param ciphertext ciphertext
 * @param sk private key
 * @param pk public key
 */
void AE_RSA::Decrypt(mpz_t *plaintext, const mpz_t *ciphertext, sk *sk, pk *pk) {
    // m = c^d mod n
    mpz_powm(*plaintext, *ciphertext, sk->d, pk->n);
}

