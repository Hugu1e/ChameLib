#ifndef CHAMELIB_RSA_H
#define CHAMELIB_RSA_H

#include <libs/gmp/include/gmp.h>
#include <string>
#include <utils/RandomGenerator.h>

class AE_RSA{
    public:
        class pk{
            public:
                mpz_t n,e;
            
                pk(){
                    mpz_inits(n,e,NULL);
                }
                void SetVal(pk *_pk){
                    mpz_set(n, _pk->n);
                    mpz_set(e, _pk->e);
                }
                ~pk(){
                    mpz_clears(n,e,NULL);
                }
        };
        class sk{
            public:
                mpz_t d;
            
                sk(){
                    mpz_init(d);
                }
                void SetVal(sk *_sk){
                    mpz_set(d, _sk->d);
                }
                ~sk(){
                    mpz_clear(d);
                }
        };
    
        void KeyGen(pk *pk, sk *sk, short k);
        void Encrypt(mpz_t *ciphertext, const mpz_t *plaintext, pk *pk);
        void Decrypt(mpz_t *plaintext, const mpz_t *ciphertext, sk *sk, pk *pk);
};

#endif  // CHAMELIB_RSA_H