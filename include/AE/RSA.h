#ifndef CHAMELIB_RSA_H
#define CHAMELIB_RSA_H

#include <base/GmpElements.h>
#include <utils/RandomGenerator.h>

class RSA_pk: public GmpElements{};

class RSA_sk: public GmpElements{};

class AE_RSA{
    public:
        void KeyGen(RSA_pk *pk, RSA_sk *sk, short k);
        void Encrypt(mpz_t ciphertext, mpz_t plaintext, RSA_pk *pk);
        void Decrypt(mpz_t plaintext, mpz_t ciphertext, RSA_sk *sk, RSA_pk *pk);
};

#endif  // CHAMELIB_RSA_H