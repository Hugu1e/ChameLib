#ifndef CHAMELIB_RSA_H
#define CHAMELIB_RSA_H

#include <base/MpzElements.h>
#include <utils/RandomGenerator.h>

class RSA_pk: public MpzElements{};

class RSA_sk: public MpzElements{};

class AE_RSA{
    public:
        void KeyGen(RSA_pk *pk, RSA_sk *sk, short k);
        void Encrypt(mpz_t *ciphertext, const mpz_t *plaintext, RSA_pk *pk);
        void Decrypt(mpz_t *plaintext, const mpz_t *ciphertext, RSA_sk *sk, RSA_pk *pk);
};

#endif  // CHAMELIB_RSA_H