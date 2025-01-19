#ifndef MCH_CDK_2017_H
#define MCH_CDK_2017_H

#include <stdio.h>
#include <gmp.h>
#include <utils/func.h>
#include <RSA/RSA.h>

class MCH_CDK_2017{
    private:
        MyRSA *rsa;

    public:
        MCH_CDK_2017(mpz_t *n, mpz_t *e, mpz_t *d);

        void H(mpz_t *m, mpz_t *res, mpz_t *n);

        void CParGen(mpz_t *n, mpz_t *e, mpz_t *d);
        void CKGen(mpz_t *n, mpz_t *e, mpz_t *d);
        void CHash(mpz_t *h, mpz_t *r, mpz_t *n,mpz_t *e, mpz_t *m);
        bool CHashCheck(mpz_t *h_, mpz_t *m, mpz_t *n, mpz_t *e, mpz_t *r);
        void Adapt(mpz_t *r_p, mpz_t *m_p, mpz_t *m, mpz_t *r, mpz_t *h, mpz_t *n,mpz_t *e,mpz_t *d);

        void MCH_CDK_2017_clear();

};




#endif  //MCH_CDK_2017_H