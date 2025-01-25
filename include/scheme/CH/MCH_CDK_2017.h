#ifndef MCH_CDK_2017_H
#define MCH_CDK_2017_H

#include <utils/Hash.h>
#include <base/GmpElements.h>
#include <AE/RSA.h>

class MCH_CDK_2017_pk: public GmpElements{};
class MCH_CDK_2017_sk: public GmpElements{};
class MCH_CDK_2017_h: public GmpElements{};
class MCH_CDK_2017_r: public GmpElements{};


class MCH_CDK_2017{
    private:
        AE_RSA rsa;

    public:
        MCH_CDK_2017();

        void KeyGen(MCH_CDK_2017_pk *pk, MCH_CDK_2017_sk *sk, short k);

        void H(mpz_t res, mpz_t m, mpz_t n);

        void Hash(MCH_CDK_2017_h *h, MCH_CDK_2017_r *r, mpz_t m, MCH_CDK_2017_pk *pk);

        bool Check(MCH_CDK_2017_h *h, MCH_CDK_2017_r *r, mpz_t m, MCH_CDK_2017_pk *pk);

        void Adapt(MCH_CDK_2017_r *r_p, mpz_t m_p, mpz_t m, MCH_CDK_2017_r *r, MCH_CDK_2017_h *h, MCH_CDK_2017_sk *sk, MCH_CDK_2017_pk *pk);

        bool Verify(MCH_CDK_2017_h *h, MCH_CDK_2017_r *r_p, mpz_t m_p, MCH_CDK_2017_pk *pk);

        void MCH_CDK_2017_clear();
};




#endif  //MCH_CDK_2017_H