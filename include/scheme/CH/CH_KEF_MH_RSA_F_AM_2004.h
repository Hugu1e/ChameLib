#ifndef CH_KEF_MH_RSA_F_AM_2004_H
#define CH_KEF_MH_RSA_F_AM_2004_H

#include <stdio.h>
#include <gmp.h>
#include <utils/Hash.h>
#include <utils/RandomGenerator.h>
#include <AE/RSA.h>
#include <base/GmpElements.h>

class CH_KEF_MH_RSA_F_AM_2004_pk: public GmpElements{};

class CH_KEF_MH_RSA_F_AM_2004_sk: public GmpElements{};

class CH_KEF_MH_RSA_F_AM_2004{
    private:
        AE_RSA rsa;
        int k,t;

    public:
        CH_KEF_MH_RSA_F_AM_2004();

        void KeyGen(CH_KEF_MH_RSA_F_AM_2004_pk *pk, CH_KEF_MH_RSA_F_AM_2004_sk *sk, int k, int t);

        void H(mpz_t res, mpz_t m, int t);
        void Hash(mpz_t h, mpz_t r, mpz_t B, CH_KEF_MH_RSA_F_AM_2004_pk *pk, CH_KEF_MH_RSA_F_AM_2004_sk *sk, mpz_t m, mpz_t tag);

        bool Check(CH_KEF_MH_RSA_F_AM_2004_pk *pk, mpz_t m, mpz_t tag, mpz_t h, mpz_t r);

        void Adapt(mpz_t r_p, CH_KEF_MH_RSA_F_AM_2004_pk *pk, mpz_t m, mpz_t m_p, mpz_t tag, mpz_t h, mpz_t B, mpz_t r);
        
        bool Verify(CH_KEF_MH_RSA_F_AM_2004_pk *pk, mpz_t m_p, mpz_t tag, mpz_t h, mpz_t r_p);

        ~CH_KEF_MH_RSA_F_AM_2004();
};

#endif  //CH_KEF_MH_RSA_F_AM_2004_H