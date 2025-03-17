#ifndef CH_KEF_MH_SDH_DL_AM_2004_H
#define CH_KEF_MH_SDH_DL_AM_2004_H

#include <stdexcept>
#include "../../base/PbcScheme.h"
#include "../../base/PbcElements.h"
#include "../../utils/Hash.h"

class CH_KEF_MH_SDH_DL_AM_2004_pp : public PbcElements{
    public:
        CH_KEF_MH_SDH_DL_AM_2004_pp(){
            init(1);
        }
};
class CH_KEF_MH_SDH_DL_AM_2004_pk : public PbcElements{
    public:
        CH_KEF_MH_SDH_DL_AM_2004_pk(){
            init(1);
        }
};
class CH_KEF_MH_SDH_DL_AM_2004_sk : public PbcElements{
    public:
        CH_KEF_MH_SDH_DL_AM_2004_sk(){
            init(1);
        }
};
class CH_KEF_MH_SDH_DL_AM_2004_h : public PbcElements{
    public:
        CH_KEF_MH_SDH_DL_AM_2004_h(){
            init(1);
        }
};
class CH_KEF_MH_SDH_DL_AM_2004_r : public PbcElements{
    public:
        CH_KEF_MH_SDH_DL_AM_2004_r(){
            init(1);
        }
};

class CH_KEF_MH_SDH_DL_AM_2004: public PbcScheme{
    private:
        element_t tmp_G, tmp_G_2, tmp_GT, tmp_GT_2, tmp_Zn, tmp_Zn_2, tmp_Zn_3;

        void H(element_t res, element_t m);

    public:
        CH_KEF_MH_SDH_DL_AM_2004(int curve);
        
        void SetUp(CH_KEF_MH_SDH_DL_AM_2004_pp &pp);

        void KeyGen(CH_KEF_MH_SDH_DL_AM_2004_pk &pk, CH_KEF_MH_SDH_DL_AM_2004_sk &sk, CH_KEF_MH_SDH_DL_AM_2004_pp &pp);

        void Hash(CH_KEF_MH_SDH_DL_AM_2004_h &h, CH_KEF_MH_SDH_DL_AM_2004_r &r, element_t m, element_t label, CH_KEF_MH_SDH_DL_AM_2004_pk &pk, CH_KEF_MH_SDH_DL_AM_2004_pp &pp);

        bool Check(CH_KEF_MH_SDH_DL_AM_2004_h &h, CH_KEF_MH_SDH_DL_AM_2004_r &r, element_t m, element_t label, CH_KEF_MH_SDH_DL_AM_2004_pk &pk, CH_KEF_MH_SDH_DL_AM_2004_pp &pp);

        void Adapt(CH_KEF_MH_SDH_DL_AM_2004_r &r_p, element_t m_p, CH_KEF_MH_SDH_DL_AM_2004_h &h, CH_KEF_MH_SDH_DL_AM_2004_r &r, element_t m, element_t label, CH_KEF_MH_SDH_DL_AM_2004_sk &sk, CH_KEF_MH_SDH_DL_AM_2004_pp &pp);

        bool Verify(CH_KEF_MH_SDH_DL_AM_2004_h &h, CH_KEF_MH_SDH_DL_AM_2004_r &r_p, element_t m_p, element_t label, CH_KEF_MH_SDH_DL_AM_2004_pk &pk, CH_KEF_MH_SDH_DL_AM_2004_pp &pp);

        ~CH_KEF_MH_SDH_DL_AM_2004();

        enum {
            g
        };

        enum {
            y
        };

        enum {
            x
        };
};


#endif //CH_KEF_MH_SDH_DL_AM_2004_H