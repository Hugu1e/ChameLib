#ifndef CH_CDK_2017_H
#define CH_CDK_2017_H

#include <pbc/pbc.h>
#include "../../utils/Hash.h"
#include "../../AE/RSA.h"
#include "../../base/GmpElements.h"

class CH_CDK_2017_pk{
    private:
        RSA_pk rsa_pk;
    public:
        RSA_pk& get_rsa_pk(){
            return rsa_pk;
        }
};

class CH_CDK_2017_sk{
    private:
        RSA_sk rsa_sk;
    public:
        RSA_sk& get_rsa_sk(){
            return rsa_sk;
        }
};

class CH_CDK_2017_h: public GmpElements{
    public:
        CH_CDK_2017_h(){
            init(1);
        }
};

class CH_CDK_2017_r: public GmpElements{
    public:
        CH_CDK_2017_r(){
            init(1);
        }
};

class CH_CDK_2017{
    private:
        AE_RSA rsa;

        void H(mpz_t res, mpz_t m1, mpz_t m2, mpz_t n);

    public:
        CH_CDK_2017();

        void SetUp();

        void KeyGen(CH_CDK_2017_pk &pk, CH_CDK_2017_sk &sk, short k);

        void Hash(CH_CDK_2017_h &h, CH_CDK_2017_r &r, mpz_t m, mpz_t tag, CH_CDK_2017_pk &pk);

        bool Check(CH_CDK_2017_h &h, CH_CDK_2017_r &r, mpz_t m, mpz_t tag, CH_CDK_2017_pk &pk);

        void Adapt(CH_CDK_2017_r &r_p, mpz_t tag_p, CH_CDK_2017_h &h, CH_CDK_2017_r &r, mpz_t m_p, CH_CDK_2017_sk &sk, CH_CDK_2017_pk &pk);

        bool Verify(CH_CDK_2017_h &h, CH_CDK_2017_r &r_p, mpz_t m_p, mpz_t tag_p, CH_CDK_2017_pk &pk);

        ~CH_CDK_2017();

        enum{
            r1
        };

        enum{
            h1
        };
};

#endif  //CH_CDK_2017_H