#ifndef CH_FS_ECC_CCT_2024_H
#define CH_FS_ECC_CCT_2024_H

#include <stdio.h>
#include "../../base/PbcElements.h"
#include "../../base/PbcScheme.h"
#include "../../utils/Hash.h"
#include "../../utils/Func.h"

class CH_FS_ECC_CCT_2024_pp: public PbcElements{
    public:
        CH_FS_ECC_CCT_2024_pp(){
            init(1);  
        };
};
class CH_FS_ECC_CCT_2024_pk: public PbcElements{
    public:
        CH_FS_ECC_CCT_2024_pk(){
            init(1);  
        };
};
class CH_FS_ECC_CCT_2024_sk: public PbcElements{
    public:
        CH_FS_ECC_CCT_2024_sk(){
            init(1);  
        };
};
class CH_FS_ECC_CCT_2024_r: public PbcElements{
    public:
        CH_FS_ECC_CCT_2024_r(){
            init(3);  
        };
};

class CH_FS_ECC_CCT_2024: public PbcScheme{
    private:
        int curve;
        int group;
    
        element_t rho;
        element_t t1,t2,T1,T2,c2;
        element_t tmp_G, tmp_G_2, tmp_Zn;

        void H(element_t res, element_t m);
        void H(element_t res, element_t m1, element_t m2, element_t m3, element_t m4);

    public:
        CH_FS_ECC_CCT_2024(int curve, int group);

        void SetUp(CH_FS_ECC_CCT_2024_pp &pp);
        
        void KeyGen(CH_FS_ECC_CCT_2024_pk &pk, CH_FS_ECC_CCT_2024_sk &sk, CH_FS_ECC_CCT_2024_pp &pp);

        void Hash(element_t h, CH_FS_ECC_CCT_2024_r &r, CH_FS_ECC_CCT_2024_pk &pk, element_t m, CH_FS_ECC_CCT_2024_pp &pp);

        bool Check(CH_FS_ECC_CCT_2024_pk &pk, element_t m, element_t h, CH_FS_ECC_CCT_2024_r &r, CH_FS_ECC_CCT_2024_pp &pp);
        
        void Adapt(CH_FS_ECC_CCT_2024_r &r_p, CH_FS_ECC_CCT_2024_pk &pk, CH_FS_ECC_CCT_2024_sk &sk, element_t m, element_t m_p, element_t h, CH_FS_ECC_CCT_2024_r &r, CH_FS_ECC_CCT_2024_pp &pp);

        bool Verify(CH_FS_ECC_CCT_2024_pk &pk, element_t m_p, element_t h, CH_FS_ECC_CCT_2024_r &r_p, CH_FS_ECC_CCT_2024_pp &pp);

        ~CH_FS_ECC_CCT_2024();

        enum{
            g
        };

        enum{
            y
        };

        enum{
            x
        };

        enum{
            z1, 
            z2, 
            c1
        };
};

#endif  //CH_FS_ECC_CCT_2024_H