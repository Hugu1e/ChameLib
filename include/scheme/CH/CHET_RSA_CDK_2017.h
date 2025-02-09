#ifndef CHET_RSA_CDK_2017_H
#define CHET_RSA_CDK_2017_H

#include <utils/Hash.h>
#include <base/GmpElements_copy.h>
#include <AE/RSA.h>
#include <stdexcept>

class CHET_RSA_CDK_2017_pk: public GmpElements_copy{};
class CHET_RSA_CDK_2017_sk: public GmpElements_copy{};
class CHET_RSA_CDK_2017_h: public GmpElements_copy{};
class CHET_RSA_CDK_2017_etd: public GmpElements_copy{};

class CHET_RSA_CDK_2017{
    private:
        AE_RSA rsa;
        mpz_t phi;

        void H(mpz_t res ,mpz_t m, mpz_t n);

    public:
        CHET_RSA_CDK_2017();

        void SetUp(CHET_RSA_CDK_2017_pk &pk, CHET_RSA_CDK_2017_sk &sk, CHET_RSA_CDK_2017_h &h, CHET_RSA_CDK_2017_etd &etd);

        void KeyGen(CHET_RSA_CDK_2017_pk &pk, CHET_RSA_CDK_2017_sk &sk, short k);

        void Hash(CHET_RSA_CDK_2017_h &h, mpz_t r, CHET_RSA_CDK_2017_etd &etd, mpz_t m, CHET_RSA_CDK_2017_pk &pk);

        bool Check(CHET_RSA_CDK_2017_h &h, mpz_t r, mpz_t m, CHET_RSA_CDK_2017_pk &pk);

        void Adapt(mpz_t r_p, mpz_t m_p, mpz_t m, mpz_t r, CHET_RSA_CDK_2017_h &h, CHET_RSA_CDK_2017_sk &sk, CHET_RSA_CDK_2017_etd &etd, CHET_RSA_CDK_2017_pk &pk);

        bool Verify(CHET_RSA_CDK_2017_h &h, mpz_t r_p, mpz_t m_p, CHET_RSA_CDK_2017_pk &pk);

        ~CHET_RSA_CDK_2017();

        enum {
            n,
            e
        };

        enum  {
            p,
            q
        };

        enum  {
            h1,
            n_
        };
};;




#endif  //CHET_RSA_CDK_2017_H