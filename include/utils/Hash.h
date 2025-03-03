#ifndef CHAMELIB_HASH_H
#define CHAMELIB_HASH_H

#include <openssl/sha.h>
#include <pbc/pbc.h>
#include <string>

class HASH{
    public:
        static void hash(element_t res, element_t m);
        static void hash(element_t res, element_t m1, element_t m2);
        static void hash(element_t res, element_t m1, std::string m2);
        static void hash(element_t res, element_t m1, element_t m2, element_t m3);
        static void hash(element_t res, element_t m1, element_t m2, element_t m3, element_t m4);
        static void hash(element_t res, element_t m1, element_t m2, element_t m3, element_t m4, element_t m5);
        static void hash(element_t res, element_t m1, element_t m2, element_t m3, element_t m4, element_t m5, element_t m6, element_t m7);
        static void hash(element_t res, std::string m);
        static void hash(element_t res, unsigned char *m, int m_len);

        static void hash_n(mpz_t res, mpz_t m1, mpz_t m2, mpz_t n);
        static void hash_n(mpz_t res, mpz_t m1, mpz_t m2, mpz_t m3, mpz_t n);
        static void hash_n(mpz_t res, std::string m, mpz_t n);
        static void hash_n(mpz_t res, mpz_t m, mpz_t n);
};

#endif  // CHAMELIB_HASH_H