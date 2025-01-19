#ifndef RPCH_TMM_2022_H
#define RPCH_TMM_2022_H

#include <stdexcept>  // 包含 std::invalid_argument
#include "utils/func.h"
#include <RSA/RSA.h>
#include <ABE/RABE_TMM.h>
#include <SE/AES.h>


class RPCH_TMM_2022 {
    protected:
        RABE_TMM rabe;

        element_t *G, *H, *GT, *Zn;
        element_t tmp_G,tmp_G_2,tmp_G_3,tmp_G_4,tmp_H,tmp_H_2,tmp_H_3,tmp_GT,tmp_GT_2,tmp_GT_3,tmp_Zn,tmp_Zn_2,tmp_Zn_3;
    
        int k;
        element_t s1,s2;
        element_t K;
        element_t R;

    public:
        struct skCHET{
            element_t x;
            void Init(element_t *_Zn){
                element_init_same_as(x, *_Zn);
            }
            ~skCHET(){
                element_clear(x);
            }
        };

        struct pkCHET{
            element_t y;
            void Init(element_t *_G){
                element_init_same_as(y, *_G);
            }
            ~pkCHET(){
                element_clear(y);
            }
        };

        struct skRPCH{
            RABE_TMM::msk mskRABE;
            RPCH_TMM_2022::skCHET skCHET;
            void Init(element_t *_G, element_t *_H, element_t *_Zn){
                mskRABE.Init(_G, _Zn);
                skCHET.Init(_Zn);
            }
        };  // msk

        struct pkRPCH{
            RABE_TMM::mpk mpkRABE;
            RPCH_TMM_2022::pkCHET pkCHET;
            void Init(element_t *_G, element_t *_H, element_t *_GT){
                mpkRABE.Init(_G, _H, _GT);
                pkCHET.Init(_G);
            }
        };  // mpk

        struct skidRPCH{
            RPCH_TMM_2022::skCHET skCHET;
            RABE_TMM::skid skidRABE;
            void Init(element_t *_G, element_t *_H, element_t *_Zn, int y_size){
                skCHET.Init(_Zn);
                skidRABE.Init(_G, _H, y_size);
            }
        };  // skid

        struct dkidtRPCH{
            RPCH_TMM_2022::skCHET skCHET;
            RABE_TMM::dkidt dkidtRABE;
            void Init(element_t *_G, element_t *_H,element_t *_Zn, int y_size){
                skCHET.Init(_Zn);
                dkidtRABE.Init(_G, _H, y_size);
            }
        };



        RPCH_TMM_2022(mpz_t *_n,mpz_t *_e, mpz_t *_d, element_t *_G, element_t *_H, element_t *_Zn, element_t *_GT);

        void PG(int k, int n, skRPCH *skRPCH, pkRPCH *pkRPCH, std::vector<RABE_TMM::revokedPreson *> *rl, binary_tree_RABE *&st);

        void KG(pkRPCH *pkRPCH, skRPCH *skRPCH, binary_tree_RABE *st, element_t *id, vector<string> *attr_list, skidRPCH *skidRPCH);

        void H1(mpz_t *m, mpz_t *N1, mpz_t *N2, mpz_t * n, mpz_t *res);
        void H2(mpz_t *m, mpz_t *N1, mpz_t *N2, mpz_t * n, mpz_t *res);
        void H4(mpz_t *r, string A, element_t *u1, element_t *u2);

        void KUpt(pkRPCH *pkRPCH, binary_tree_RABE *st, std::vector<RABE_TMM::revokedPreson *> *rl, time_t t, RABE_TMM::kut *kut);
        void DKGen(pkRPCH *pkRPCH, skidRPCH *skidRPCH, RABE_TMM::kut *kut, dkidtRPCH *dkidtRPCH);
        void Rev(std::vector<RABE_TMM::revokedPreson *> *rl, element_t *id, time_t t);

        void Hash(pkRPCH *pkRPCH, element_t *m, string policy_str, time_t t, 
                            element_t *b, element_t *r, element_t *h, RABE_TMM::ciphertext *C);

        bool Check(pkRPCH *pkRPCH, element_t *m, element_t *b, element_t *r, element_t *h);

        void Forge(pkRPCH * pkRPCH, dkidtRPCH *dkidtRPCH, element_t *m, element_t *m_p, element_t *b, element_t *r, element_t *h, RABE_TMM::ciphertext *C, element_t *r_p);

        bool Verify(pkRPCH *pkRPCH, element_t *m_p, element_t *b, element_t *r_p, element_t *h);


        ~RPCH_TMM_2022();
};


#endif //RPCH_TMM_2022_H