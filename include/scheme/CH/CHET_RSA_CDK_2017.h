#ifndef CHET_RSA_CDK_2017_H
#define CHET_RSA_CDK_2017_H

#include <stdio.h>
#include <gmp.h>
#include <utils/func.h>
#include <RSA/RSA.h>

class CHET_RSA_CDK_2017{
    private:
        MyRSA *rsa;
        mpz_t phi;

    public:
        CHET_RSA_CDK_2017(mpz_t *n, mpz_t *e, mpz_t *d);

        void H(mpz_t *m, mpz_t *res, mpz_t *n);

        void CParGen(mpz_t *n, mpz_t *e, mpz_t *d);
        void CKGen(mpz_t *n, mpz_t *e, mpz_t *d);
        void CHash(mpz_t *h, mpz_t *etd_n, mpz_t *r,mpz_t *etd_p, mpz_t *etd_q, mpz_t *n,mpz_t *e, mpz_t *m);
        bool CHashCheck(mpz_t *h_, mpz_t *m, mpz_t *n, mpz_t *etd_n,mpz_t *e, mpz_t *r);
        bool Adapt(mpz_t *r_p, mpz_t *m_p, mpz_t *m, mpz_t *r, mpz_t *h, mpz_t *n,mpz_t *etd_n,mpz_t *etd_p,mpz_t *etd_q,mpz_t *e);

        void CHET_RSA_CDK_2017_clear();

};




#endif  //CHET_RSA_CDK_2017_H