#ifndef CH_KEF_DL_CZT_2011_H
#define CH_KEF_DL_CZT_2011_H

#include "../../base/PbcElements.h"
#include "../../base/PbcScheme.h"
#include "../../utils/Hash.h"

class CH_KEF_DL_CZT_2011_pp : public PbcElements{
    public:
        CH_KEF_DL_CZT_2011_pp(){
            init(1);
        }
};
class CH_KEF_DL_CZT_2011_pk : public PbcElements{
    public:
        CH_KEF_DL_CZT_2011_pk(){
            init(1);
        }
};
class CH_KEF_DL_CZT_2011_sk : public PbcElements{
    public:
        CH_KEF_DL_CZT_2011_sk(){
            init(1);
        }
};
class CH_KEF_DL_CZT_2011_r : public PbcElements{
    public:
        CH_KEF_DL_CZT_2011_r(){
            init(2);
        }
};

class CH_KEF_DL_CZT_2011 : public PbcScheme {
    private:
        element_t tmp_G, tmp_G_2, tmp_G_3, tmp_Zn, tmp_Zn_2, tmp_Zn_3;

        void H(element_t res, element_t m1, element_t m2);

    public:
        CH_KEF_DL_CZT_2011(int curve, int group);

        void SetUp(CH_KEF_DL_CZT_2011_pp &pp);

        void KeyGen(CH_KEF_DL_CZT_2011_pk &pk, CH_KEF_DL_CZT_2011_sk &sk, CH_KEF_DL_CZT_2011_pp &pp);

        void Hash(element_t h, CH_KEF_DL_CZT_2011_r &r, element_t I, element_t m, CH_KEF_DL_CZT_2011_pk &pk, CH_KEF_DL_CZT_2011_pp &pp);

        bool Check(element_t h, CH_KEF_DL_CZT_2011_r &r, element_t I, element_t m, CH_KEF_DL_CZT_2011_pk &pk, CH_KEF_DL_CZT_2011_pp &pp);

        void Adapt(CH_KEF_DL_CZT_2011_r &r_p, CH_KEF_DL_CZT_2011_sk &sk, element_t h, element_t m, CH_KEF_DL_CZT_2011_r &r,  element_t m_p, element_t I, CH_KEF_DL_CZT_2011_pp &pp);

        bool Verify(element_t h, CH_KEF_DL_CZT_2011_r &r_p, element_t I, element_t m_p, CH_KEF_DL_CZT_2011_pk &pk, CH_KEF_DL_CZT_2011_pp &pp);

        ~CH_KEF_DL_CZT_2011();

        enum {
            g
        };

        enum {
            y
        };

        enum {
            x
        };

        enum {
            r1,
            r2
        };
};

#endif //CH_KEF_DL_CZT_2011_H
