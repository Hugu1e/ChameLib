#ifndef CH_KEF_MH_RSA_F_AM_2004_H
#define CH_KEF_MH_RSA_F_AM_2004_H

#include <stdio.h>
#include <gmp.h>
#include <utils/func.h>
#include <RSA/RSA.h>

class CH_KEF_MH_RSA_F_AM_2004{
    private:
        MyRSA *rsa;
        int k,t;

    public:
        struct pk{
            mpz_t n,e;
            void Init(){
                mpz_inits(n,e,NULL);
            }
            ~pk(){
                mpz_clears(n,e,NULL);
            }
        };

        struct sk{
            mpz_t d;
            void Init(){
                mpz_inits(d,NULL);
            }
            ~sk(){
                mpz_clears(d,NULL);
            }
        };

        CH_KEF_MH_RSA_F_AM_2004(pk *pk, sk *sk);

        void KGen(int k, int t, pk *pk, sk *sk);

        void H(mpz_t *m, mpz_t *res, int t);
        void Hash(pk *pk, sk *sk, mpz_t *m, mpz_t *tag, mpz_t *h, mpz_t *B, mpz_t *r);

        bool Check(pk *pk, mpz_t *m, mpz_t *tag, mpz_t *h,mpz_t *r);

        void Adapt(pk *pk, mpz_t *m,  mpz_t *m_p, mpz_t *tag, mpz_t *h, mpz_t *B, mpz_t *r, mpz_t *r_p);
        
        bool Verify(pk *pk, mpz_t *m_p, mpz_t *tag, mpz_t *h, mpz_t *r_p);

        ~CH_KEF_MH_RSA_F_AM_2004();
};

#endif  //CH_KEF_MH_RSA_F_AM_2004_H