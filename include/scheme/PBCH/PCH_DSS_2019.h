#ifndef PCH_DSS_2019_H
#define PCH_DSS_2019_H

#include <stdexcept>  // 包含 std::invalid_argument
#include "utils/func.h"
#include <RSA/RSA.h>
#include <ABE/CP_ABE.h>
#include <SE/AES.h>


class PCH_DSS_2019 {
    protected:
        MyRSA rsa;
        CP_ABE cp_abe;
        AES aes;

        mpz_t *n,e,d;
        element_t *G, *H, *GT, *Zn;
        element_t tmp_G,tmp_G_2,tmp_G_3,tmp_G_4,tmp_H,tmp_H_2,tmp_H_3,tmp_GT,tmp_GT_2,tmp_GT_3,tmp_Zn,tmp_Zn_2,tmp_Zn_3;
    
        int k;
        element_t u1,u2;
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
            int k;
            mpz_t N1;
            mpz_t e;
            void Init(){
                mpz_inits(N1,e,NULL);
            }
            ~pkCHET(){
                mpz_clears(N1,e,NULL);
            }
        };
        struct skPCH{
            CP_ABE::msk mskABE;
            PCH_DSS_2019::skCHET skCHET;
            void Init(element_t *_G, element_t *_H, element_t *_Zn){
                mskABE.Init(_G, _H, _Zn);
                skCHET.Init();
            }
        };
        struct pkPCH{
            CP_ABE::mpk mpkABE;
            PCH_DSS_2019::pkCHET pkCHET;
            void Init(element_t *_H, element_t *_GT){
                mpkABE.Init(_H, _GT);
                pkCHET.Init();
            }
        };
        struct sksPCH{
            PCH_DSS_2019::skCHET skCHET;
            CP_ABE::sks sksABE;
            void Init(element_t *_G, element_t *_H, int y_size){
                skCHET.Init();
                sksABE.Init(_G, _H, y_size);
            }
        };
        struct r{
            mpz_t r1,r2;
            void Init(){
                mpz_inits(r1,r2,NULL);
            }
            ~r(){
                mpz_clears(r1,r2,NULL);
            }
        };
        struct h{
            mpz_t h1,h2;
            mpz_t N2;
            CP_ABE::ciphertext ct;
            mpz_t ct_;
            void Init(element_t *_G, element_t *_H, element_t *_GT, int rows){
                mpz_inits(h1,h2,N2,ct_,NULL);
                ct.Init(_G, _H, _GT, rows);
            }
            ~h(){
                mpz_clears(h1,h2,N2,ct_,NULL);
            }
        };


        PCH_DSS_2019(mpz_t *_n,mpz_t *_e, mpz_t *_d, element_t *_G, element_t *_H, element_t *_Zn, element_t *_GT);

        void PG(int k, skPCH *skPCH, pkPCH *pkPCH);

        void KG(skPCH *skPCH, pkPCH *pkPCH, std::vector<std::string> *attr_list, sksPCH *sksPCH);

        void H1(mpz_t *m, mpz_t *N1, mpz_t *N2, mpz_t * n, mpz_t *res);
        void H2(mpz_t *m, mpz_t *N1, mpz_t *N2, mpz_t * n, mpz_t *res);
        void H4(mpz_t *r, string A, element_t *u1, element_t *u2);

        void Hash(pkPCH *pkPCH, mpz_t *m, string policy_str, h *h, r *r);

        bool Check(pkPCH *pkPCH, mpz_t *m, h *h, r *r);

        void Forge(pkPCH * pkPCH, sksPCH *sksPCH, mpz_t *m, mpz_t *m_p, h *h, PCH_DSS_2019::r *r, PCH_DSS_2019::r *r_p);

        bool Verify(pkPCH *pkPCH, mpz_t *m_p, h *h, r *r_p);


        ~PCH_DSS_2019();
};


#endif //PCH_DSS_2019_H