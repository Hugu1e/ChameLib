#ifndef CHAMELIB_AES_H
#define CHAMELIB_AES_H

#include <pbc.h>
#include <gmp.h>
#include <string.h>
#include <openssl/evp.h>
#include <iostream>


class AES{
    public:
        AES();

        void KGen(element_t key);
        void KGen(element_t key, int k);

        void Enc(mpz_t ciphertext, element_t key, mpz_t plaintext, int k = 256);

        void Dec(mpz_t decrypted_plaintext, element_t key, mpz_t ciphertext, int k = 256);

        ~AES();
};


#endif  // CHAMELIB_AES_H