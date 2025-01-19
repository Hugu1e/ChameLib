#ifndef CH_ET_BC_CDK_2017_H
#define CH_ET_BC_CDK_2017_H

#include <stdio.h>
#include <gmp.h>
#include <utils/func.h>
#include <RSA/RSA.h>

class CH_ET_BC_CDK_2017{
    private:
        MyRSA rsa;

    public:
        struct pp{
            int k;
        };
        struct pk{
            mpz_t n0,e0;
            pk(){
                mpz_inits(n0,e0,NULL);
            }
            void setValues(mpz_t *_n0, mpz_t *_e0){
                mpz_set(n0, *_n0);
                mpz_set(e0, *_e0);
            }
            void setValues(pk *_pk){
                mpz_set(n0, _pk->n0);
                mpz_set(e0, _pk->e0);
            }
            ~pk(){
                mpz_clears(n0,e0,NULL);
            }
        };
        struct sk{
            mpz_t d0;
            sk(){
                mpz_init(d0);
            }
            void setValues(mpz_t *_d0){
                mpz_set(d0, *_d0);
            }
            void setValues(sk *_sk){
                mpz_set(d0, _sk->d0);
            }
            ~sk(){
                mpz_clear(d0);
            }
        };
        struct h{
            mpz_t h0,h1;
            mpz_t n1,e1;
            h(){
                mpz_inits(h0,h1,n1,e1,NULL);
            }
            void setValues(mpz_t *_h0, mpz_t *_h1, mpz_t *_n1, mpz_t *_e1){
                mpz_set(h0, *_h0);
                mpz_set(h1, *_h1);
                mpz_set(n1, *_n1);
                mpz_set(e1, *_e1);
            }
            void setValues(h *_h){
                mpz_set(h0, _h->h0);
                mpz_set(h1, _h->h1);
                mpz_set(n1, _h->n1);
                mpz_set(e1, _h->e1);
            }
            ~h(){
                mpz_clears(h0,h1,n1,e1,NULL);
            }
        };
        struct r{
            mpz_t r0,r1;
            r(){
                mpz_inits(r0,r1,NULL);
            }
            ~r(){
                mpz_clears(r0,r1,NULL);
            }
        };
        struct etd{
            mpz_t d1;
            etd(){
                mpz_init(d1);
            }
            ~etd(){
                mpz_clear(d1);
            }
        };

        CH_ET_BC_CDK_2017();

        void SetUp(pp *pp, int k);

        void KeyGen(pk *pk, sk *sk, pp *pp);

        void H(mpz_t *res, string m, mpz_t *n);

        void Hash(h *h, r* r, etd *etd, pp *pp, pk *pk, string m);

        bool Check(h *h, r* r, pk *pk, string m);

        void Adapt(r *r_p, sk *sk, etd *etd, pk *pk, h *h, r* r, string m, string m_p);
        
        bool Verify(h *h, r* r_p, pk *pk, string m_p);

        ~CH_ET_BC_CDK_2017();

};




#endif  //CH_ET_BC_CDK_2017_H