#include <utils/RandomGenerator.h>

/**
 * @brief Generate a random number in length
 * 
 * @param res random number
 * @param length length of random number
 */
void RandomGenerator::RandomInLength(mpz_t res, short length){
    gmp_randstate_t state;
    gmp_randinit_default(state);          
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long long seed = tv.tv_sec * 1000000 + tv.tv_usec;

    gmp_randseed_ui(state, seed);
    mpz_urandomb(res, state, length);
    gmp_randclear(state);
}

/**
 * @brief Generate a random number ∈ N
 * 
 * @param res random number
 * @param n max number
 */
void RandomGenerator::RandomN(mpz_t res, mpz_t n){
    gmp_randstate_t state;
    gmp_randinit_default(state);          
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long long seed = tv.tv_sec * 1000000 + tv.tv_usec;

    gmp_randseed_ui(state, seed);
    mpz_urandomm(res, state, n);
    gmp_randclear(state);
}

/**
 * @brief Generate a random number ∈ ZN*
 * 
 * @param res random number
 * @param n max number
 */
void RandomGenerator::RandomCoprimeN(mpz_t res, mpz_t n){
    RandomN(res, n);
    mpz_t gcd;
    mpz_inits(gcd, NULL);
    mpz_gcd(gcd, res, n);
    while(mpz_cmp_ui(gcd, 1) != 0) {
        RandomN(res, n);
        mpz_gcd(gcd, res, n);
    }
    mpz_clears(gcd, NULL);
}