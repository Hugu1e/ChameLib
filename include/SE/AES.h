#ifndef CHAMELIB_AES_H
#define CHAMELIB_AES_H

#include <pbc/pbc.h>
#include <string.h>
#include <openssl/evp.h>
#include <iostream>


class AES{
    public:
        AES();

        void KGen(element_t key);
        void KGen(element_t key, int k);

        void KGen(unsigned char *key, int k);

        unsigned char *Enc(int *ciphertext_len, element_t key, mpz_t plaintext, int k = 256);
        void Dec(mpz_t decrypted_plaintext, element_t key, unsigned char *ciphertext_bytes, int ciphertext_len, int k = 256);

        unsigned char *Enc(int *ciphertext_len, unsigned char *key, mpz_t plaintext, int k = 256);
        void Dec(mpz_t decrypted_plaintext, unsigned char *key, unsigned char *ciphertext_bytes, int ciphertext_len, int k = 256);

        ~AES();
};


#endif  // CHAMELIB_AES_H