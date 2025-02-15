#ifndef CH_KEF_MH_RSANN_F_AM_2004_H
#define CH_KEF_MH_RSANN_F_AM_2004_H

#include "../../base/GmpElements.h"
#include "../../utils/RandomGenerator.h"
#include "../../utils/Hash.h"

class CH_KEF_MH_RSANN_F_AM_2004_pk: public GmpElements{};

class CH_KEF_MH_RSANN_F_AM_2004_sk: public GmpElements{};

class CH_KEF_MH_RSANN_F_AM_2004_h: public GmpElements{};

class CH_KEF_MH_RSANN_F_AM_2004_r: public GmpElements{};

class CH_KEF_MH_RSANN_F_AM_2004{
    private:
        void H(mpz_t res, mpz_t m, mpz_t n);

        void lcm(mpz_t res, mpz_t a, mpz_t b);

        void L(mpz_t res, mpz_t x, mpz_t n);
    public:
        CH_KEF_MH_RSANN_F_AM_2004();

        void SetUp(CH_KEF_MH_RSANN_F_AM_2004_pk &pk, CH_KEF_MH_RSANN_F_AM_2004_sk &sk, CH_KEF_MH_RSANN_F_AM_2004_h &h, CH_KEF_MH_RSANN_F_AM_2004_r &r, CH_KEF_MH_RSANN_F_AM_2004_r &r_p);

        void KeyGen(CH_KEF_MH_RSANN_F_AM_2004_pk &pk, CH_KEF_MH_RSANN_F_AM_2004_sk &sk, int k);

        void Hash(CH_KEF_MH_RSANN_F_AM_2004_h &h, CH_KEF_MH_RSANN_F_AM_2004_r &r, mpz_t m, mpz_t Label, CH_KEF_MH_RSANN_F_AM_2004_pk &pk);

        bool Check(CH_KEF_MH_RSANN_F_AM_2004_h &h, mpz_t m, CH_KEF_MH_RSANN_F_AM_2004_r &r, mpz_t Label, CH_KEF_MH_RSANN_F_AM_2004_pk &pk);

        void Adapt(CH_KEF_MH_RSANN_F_AM_2004_r &r_p, mpz_t m_p, CH_KEF_MH_RSANN_F_AM_2004_h &h, mpz_t m, CH_KEF_MH_RSANN_F_AM_2004_r &r, mpz_t Label, CH_KEF_MH_RSANN_F_AM_2004_sk &sk, CH_KEF_MH_RSANN_F_AM_2004_pk &pk);

        bool Verify(CH_KEF_MH_RSANN_F_AM_2004_h &h, mpz_t m_p, CH_KEF_MH_RSANN_F_AM_2004_r &r_p, mpz_t Label, CH_KEF_MH_RSANN_F_AM_2004_pk &pk);

        ~CH_KEF_MH_RSANN_F_AM_2004();

        enum{
            n
        };

        enum{
            p, q
        };

        enum{
            h1
        };

        enum{
            r1, r2
        };
};

#endif  //CH_KEF_MH_RSANN_F_AM_2004_H