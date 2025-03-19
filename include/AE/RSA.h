#ifndef CHAMELIB_RSA_H
#define CHAMELIB_RSA_H

#include "../base/GmpElements.h"
#include "../utils/RandomGenerator.h"

class RSA_pk: public GmpElements{
    public:
        RSA_pk(){
            init(2);
        }
};

class RSA_sk: public GmpElements{
    public:
        RSA_sk(){
            init(1);
        }
};

class RSA_m: public GmpElements{
    public:
        RSA_m(){
            init(1);
        }
};

class AE_RSA{
    public:
        void SetUp();
        void SetUp(RSA_pk &pk, RSA_sk &sk);
        void KeyGen(RSA_pk &pk, RSA_sk &sk, short k);
        void Encrypt(mpz_t ciphertext, mpz_t plaintext, RSA_pk &pk);
        void Decrypt(mpz_t plaintext, mpz_t ciphertext, RSA_sk &sk, RSA_pk &pk);

        void SetUp(RSA_pk &pk, RSA_sk &sk, RSA_m &m);
        void Encrypt(RSA_m &ciphertext, mpz_t plaintext, RSA_pk &pk);
        void Decrypt(mpz_t plaintext, RSA_m &ciphertext, RSA_sk &sk, RSA_pk &pk);

        // void KeyGen(mpz_t p, mpz_t q, short k);
        void KeyGen(mpz_t p, mpz_t q, mpz_t n, mpz_t e, short k, short exponent);
        void KeyGen(mpz_t n, mpz_t e, mpz_t d, short k, short exponent);
        void KeyGen(RSA_pk &pk, RSA_sk &sk, short ebit, short pbit);
        void KeyGen_E(mpz_t n, mpz_t d, mpz_t e, short k);
        void KeyGen_E(mpz_t p, mpz_t q, mpz_t n, mpz_t e, short k);

        void ComputePhi(mpz_t phi, mpz_t p, mpz_t q);

        enum{
            n,
            e
        };

        enum{
            d
        };
};

#endif  // CHAMELIB_RSA_H