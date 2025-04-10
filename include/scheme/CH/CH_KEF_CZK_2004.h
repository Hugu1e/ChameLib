#ifndef CH_KEF_CZK_2004_H
#define CH_KEF_CZK_2004_H

#include <stdexcept>
#include "../../base/PbcElements.h"
#include "../../base/PbcScheme.h"

class CH_KEF_CZK_2004_pp : public PbcElements{
    public:
        CH_KEF_CZK_2004_pp(){
            init(1);
        }
};
class CH_KEF_CZK_2004_pk : public PbcElements{
    public:
        CH_KEF_CZK_2004_pk(){
            init(1);
        }
};
class CH_KEF_CZK_2004_sk : public PbcElements{
    public:
        CH_KEF_CZK_2004_sk(){
            init(1);
        }
};
class CH_KEF_CZK_2004_r : public PbcElements{
    public:
        CH_KEF_CZK_2004_r(){
            init(2);
        }
};

class CH_KEF_CZK_2004: public PbcScheme {
    private:
        element_t tmp_G, tmp_G_2, tmp_Zn, tmp_Zn_2, tmp_Zn_3, tmp_Zn_4;

    public:
        CH_KEF_CZK_2004(int curve, int group);

        void SetUp(CH_KEF_CZK_2004_pp &pp);

        void KeyGen(CH_KEF_CZK_2004_pk &pk, CH_KEF_CZK_2004_sk &sk, CH_KEF_CZK_2004_pp &pp);

        void Hash(element_t h, CH_KEF_CZK_2004_r &r, element_t m, element_t I, CH_KEF_CZK_2004_pk &pk, CH_KEF_CZK_2004_pp &pp);

        bool Check(element_t I, element_t m, CH_KEF_CZK_2004_r &r, element_t h, CH_KEF_CZK_2004_pp &pp);

        void Adapt(CH_KEF_CZK_2004_r &r_p, CH_KEF_CZK_2004_sk &sk, element_t h, element_t m, CH_KEF_CZK_2004_r &r, element_t m_p, element_t I, CH_KEF_CZK_2004_pp &pp);

        bool Verify(element_t I, element_t m_p, CH_KEF_CZK_2004_r &r_p, element_t h, CH_KEF_CZK_2004_pp &pp);

        ~CH_KEF_CZK_2004();

        enum{
            x
        };

        enum{
            y
        };

        enum{
            r1, r2
        };

        enum{
            g
        };
};

#endif //CH_KEF_CZK_2004_H
