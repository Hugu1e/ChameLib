#ifndef PCHBA_TLL_2020_H
#define PCHBA_TLL_2020_H

#include <stdexcept>
#include "utils/func.h"
#include <ABE/ABET.h>

class PCHBA_TLL_2020 {
    protected:
        ABET abet;

        element_t *G, *H, *Zn, *GT;
        element_t tmp_G,tmp_G_2,tmp_G_3,tmp_G_4,tmp_H,tmp_H_2,tmp_H_3,tmp_GT,tmp_GT_2,tmp_GT_3,tmp_Zn,tmp_Zn_2,tmp_Zn_3;

        int k;
        element_t r,R;
        element_t s1,s2;
        element_t esk;

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
            element_t h_pow_x;
            void Init(element_t *_H){
                element_init_same_as(h_pow_x, *_H);
            }
            ~pkCHET(){
                element_clear(h_pow_x);
            }
        };

        struct skPCHBA{
            ABET::msk skABET;
            PCHBA_TLL_2020::skCHET skCHET;
            void Init(element_t *_G, element_t *_H, element_t *_Zn, int k){
                skABET.Init(_G, _H, _Zn, k);
                skCHET.Init(_Zn);
            }
        };

        struct pkPCHBA{
            ABET::mpk pkABET;
            PCHBA_TLL_2020::pkCHET pkCHET;
            void Init(element_t *_G, element_t *_H, element_t *_GT, int k){
                pkABET.Init(_G, _H, _GT, k);
                pkCHET.Init(_H);
            }
        };

        struct sksPCHBA{
            element_t x;
            ABET::sks sksABET;
            void Init(element_t *_G, element_t *_H, element_t *_Zn, int y_size, int I){
                element_init_same_as(x, *_Zn);
                sksABET.Init(_G, _H, y_size, I);
            }
            ~sksPCHBA(){
                element_clear(x);
            }
        };

        PCHBA_TLL_2020(element_t *_G, element_t *_H, element_t *_GT, element_t *_Zn);

        void PG(int k, skPCHBA *skPCHBA, pkPCHBA *pkPCHBA);

        void KG(skPCHBA *skPCHBA, pkPCHBA *pkPCHBA, std::vector<std::string> *attr_list, ABET::ID *ID, int mi, sksPCHBA *sksPCHBA);

        void Hash(pkPCHBA *pkPCHBA, skPCHBA *skPCHBA, element_t *m, string policy_str, ABET::ID *ID, int oj, 
                            element_t *p, element_t *h_, element_t *b, ABET::ciphertext *C, element_t *c, element_t *epk, element_t *sigma);

        bool Check(pkPCHBA *pkPCHBA, element_t *m, element_t *p, element_t *h_, element_t *b, ABET::ciphertext *C, element_t *c, element_t *epk, element_t *sigma);

        void Forge(pkPCHBA *pkPCHBA, skPCHBA* skPCHBA,sksPCHBA *sksPCHBA, element_t *m, element_t *p, element_t *h_, element_t *b, ABET::ciphertext *C, 
                            element_t *c, element_t *epk, element_t *sigma, string policy_str, ABET::ID *ID, int mi,
                            element_t *m_p, element_t *p_p, ABET::ciphertext *C_p, element_t *c_p, element_t *epk_p, element_t *sigma_p);

        bool Verify(pkPCHBA *pkPCHBA, element_t *m, element_t *p, element_t *h_, element_t *b, ABET::ciphertext *C, element_t *c, element_t *epk, element_t *sigma);

        bool Judge(pkPCHBA *pkPCHBA, skPCHBA *skPCHBA, 
                            element_t *m, element_t *p, element_t *h_, element_t *b, ABET::ciphertext *C, element_t *c, element_t *epk, element_t *sigma,
                            element_t *m_p, element_t *p_p, ABET::ciphertext *C_p, element_t *c_p, element_t *epk_p, element_t *sigma_p,
                            ABET::ID *ID, int mi);

        ~PCHBA_TLL_2020();
};


#endif //PCHBA_TLL_2020_H