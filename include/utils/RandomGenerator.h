#ifndef CHAMELIB_UTILS_RANDOMGENERATOR_H
#define CHAMELIB_UTILS_RANDOMGENERATOR_H

#include <gmp.h>
#include <sys/time.h>

class RandomGenerator{
    public:
        static void RandomInLength(mpz_t res, short length);
        static void RandomN(mpz_t res, mpz_t n);
        static void RandomCoprimeN(mpz_t res, mpz_t n);
};

#endif  // CHAMELIB_UTILS_RANDOMGENERATOR_H