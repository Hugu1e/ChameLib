#ifndef RPCH_XNM_2021_H
#define RPCH_XNM_2021_H

#include <stdexcept>  // 包含 std::invalid_argument
#include "utils/func.h"
#include <RSA/RSA.h>
#include <ABE/RABE.h>
#include <SE/AES.h>


class RPCH_XNM_2021 {
    protected:
        MyRSA rsa;
        RABE rabe;
        AES aes;

        mpz_t *n,e,d;
        element_t *G, *H, *GT, *Zn;
        element_t tmp_G,tmp_G_2,tmp_G_3,tmp_G_4,tmp_H,tmp_H_2,tmp_H_3,tmp_GT,tmp_GT_2,tmp_GT_3,tmp_Zn,tmp_Zn_2,tmp_Zn_3;
    
        int k;
        element_t s1,s2;
        element_t K;

    public:
        struct skCHET{
            mpz_t d1;
            void Init(){
                mpz_init(d1);
            }
            ~skCHET(){
                mpz_clear(d1);
            }
        };

        struct pkCHET{
            mpz_t N1;
            mpz_t e;
            void Init(){
                mpz_inits(N1,e,NULL);
            }
            ~pkCHET(){
                mpz_clears(N1,e,NULL);
            }
        };

        struct skRPCH{
            RABE::msk mskRABE;
            RPCH_XNM_2021::skCHET skCHET;
            void Init(element_t *_G, element_t *_H, element_t *_Zn){
                mskRABE.Init(_G, _H, _Zn);
                skCHET.Init();
            }
        };  // msk

        struct pkRPCH{
            RABE::mpk mpkRABE;
            RPCH_XNM_2021::pkCHET pkCHET;
            void Init(element_t *_H, element_t *_GT){
                mpkRABE.Init(_H, _GT);
                pkCHET.Init();
            }
        };  // mpk

        struct skidRPCH{
            RPCH_XNM_2021::skCHET skCHET;
            RABE::skid skidRABE;
            void Init(element_t *_G, element_t *_H, int y_size){
                skCHET.Init();
                skidRABE.Init(_G, _H, y_size);
            }
        };  // skid

        struct dkidtRPCH{
            RPCH_XNM_2021::skCHET skCHET;
            RABE::dkidt dkidtRABE;
            void Init(element_t *_G, element_t *_H, int y_size){
                skCHET.Init();
                dkidtRABE.Init(_G, _H, y_size);
            }
        };

        struct h{
            mpz_t h1,h2;
            mpz_t N2;
            RABE::ciphertext ct;
            mpz_t cSE;
            void Init(element_t *_G, element_t *_H, element_t *_GT, int rows){
                mpz_inits(h1,h2,N2,cSE,NULL);
                ct.Init(_G, _H, _GT, rows);
            }
            ~h(){
                mpz_clears(h1,h2,N2,cSE,NULL);
            }
        };  // hash

        struct r{
            mpz_t r1,r2;
            void Init(){
                mpz_inits(r1,r2,NULL);
            }
            ~r(){
                mpz_clears(r1,r2,NULL);
            }
        };


        RPCH_XNM_2021(mpz_t *_n,mpz_t *_e, mpz_t *_d, element_t *_G, element_t *_H, element_t *_Zn, element_t *_GT);

        void PG(int k, int n, skRPCH *skRPCH, pkRPCH *pkRPCH, vector<RABE::revokedPreson *> *rl, binary_tree_RABE *&st);

        void KG(pkRPCH *pkRPCH, skRPCH *skRPCH, binary_tree_RABE *st, element_t *id, vector<string> *attr_list, skidRPCH *skidRPCH);

        void H1(mpz_t *m, mpz_t *N1, mpz_t *N2, mpz_t * n, mpz_t *res);
        void H2(mpz_t *m, mpz_t *N1, mpz_t *N2, mpz_t * n, mpz_t *res);
        void H4(mpz_t *r, string A, element_t *u1, element_t *u2);

        void KUpt(pkRPCH *pkRPCH, binary_tree_RABE *st, vector<RABE::revokedPreson *> *rl, time_t t, RABE::kut *kut);
        void DKGen(pkRPCH *pkRPCH, skidRPCH *skidRPCH, RABE::kut *kut, dkidtRPCH *dkidtRPCH);
        void Rev(vector<RABE::revokedPreson *> *rl, element_t *id, time_t t);

        void Hash(pkRPCH *pkRPCH, mpz_t *m, string policy_str, time_t t, h *h, r *r);

        bool Check(pkRPCH *pkRPCH, mpz_t *m, h *h, r *r);

        void Forge(pkRPCH * pkRPCH, dkidtRPCH *dkidtRPCH, mpz_t *m, mpz_t *m_p, h *h, RPCH_XNM_2021::r *r, RPCH_XNM_2021::r *r_p);

        bool Verify(pkRPCH *pkRPCH, mpz_t *m_p, h *h, r *r_p);


        ~RPCH_XNM_2021();
};


#endif //RPCH_XNM_2021_H