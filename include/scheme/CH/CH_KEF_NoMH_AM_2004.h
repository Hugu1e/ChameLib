#ifndef CH_KEF_NoMH_AM_2004_H
#define CH_KEF_NoMH_AM_2004_H

#include <stdio.h>
#include "base/ElementList.h"
#include <utils/func.h>

class CH_KEF_NoMH_AM_2004{
    private:
        element_t *G1, *G2, *Zn, *GT;
        element_t tmp_G1, tmp_G1_2, tmp_G2, tmp_Zn,tmp_Zn_2, tmp_GT,tmp_GT_2,tmp_GT_3;
        

    public:
        struct pk{
            element_t g,y;

            void Init(element_t *_Zn){
                element_init_same_as(g, *_Zn);
                element_init_same_as(y, *_Zn);
            }
            ~pk(){
                element_clear(g);
                element_clear(y);
            }
        };

        struct sk{
            element_t x;

            void Init(element_t *_Zn){
                element_init_same_as(x, *_Zn);
            }
            ~sk(){
                element_clear(x);
            }
        };

        CH_KEF_NoMH_AM_2004(element_t *G1, element_t *G2, element_t *GT, element_t *Zn);

        ~CH_KEF_NoMH_AM_2004();

        void KeyGen(pk *pk, sk *sk);

        void Hash(pk *pk, element_t *m, element_t *r, element_t *s, element_t *h);

        void H(element_t *m1, element_t *m2, element_t *res);

        void Forge(pk *pk,sk *sk, element_t *m_p, element_t *h, element_t *r_p, element_t *s_p);

        bool Check(pk *pk, element_t *m, element_t *r, element_t *s, element_t *h);

        bool Verify(pk *pk, element_t *m_p, element_t *r_p, element_t *s_p, element_t *h);
};

#endif  //CH_KEF_NoMH_AM_2004_H