#ifndef CH_FS_ECC_CCTY_2024_H
#define CH_FS_ECC_CCTY_2024_H

#include <stdio.h>
#include "base/ElementList.h"
#include <utils/func.h>

class CH_FS_ECC_CCTY_2024{
    private:
        element_t *G1, *G2, *Zn, *GT;
        element_t tmp_G1, tmp_G1_2, tmp_G2, tmp_Zn,tmp_Zn_2, tmp_GT,tmp_GT_2,tmp_GT_3;
        
        element_t g,rho;
        element_t t1,t2,T1,T2,c2;

    public:
        struct pk{
            element_t y;

            void Init(element_t *_G1){
                element_init_same_as(y, *_G1);
            }
            ~pk(){
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

        struct r{
            element_t z1,z2,c1;

            void Init(element_t *_Zn){
                element_init_same_as(z1, *_Zn);
                element_init_same_as(z2, *_Zn);
                element_init_same_as(c1, *_Zn);
            }
            ~r(){
                element_clear(z1);
                element_clear(z2);
                element_clear(c1);
            }
        };

        CH_FS_ECC_CCTY_2024(element_t *_G1, element_t *_G2, element_t *_GT, element_t *_Zn);

        ~CH_FS_ECC_CCTY_2024();

        void ParamGen();

        void KeyGen(pk *pk, sk *sk);

        void Hash(pk *pk, element_t *m, element_t *h, r *r);

        void H(element_t *m, element_t *res);
        void H(element_t *m1, element_t *m2, element_t *m3, element_t *m4, element_t *res);

        bool Check(pk *pk, element_t *m, element_t *h, r *r);
        
        void Forge(pk *pk, sk *sk, element_t *m, element_t *m_p, element_t *h, CH_FS_ECC_CCTY_2024::r *r, CH_FS_ECC_CCTY_2024::r *r_p);

        bool Verify(pk *pk, element_t *m_p, element_t *h, r *r_p);
};

#endif  //CH_FS_ECC_CCTY_2024_H