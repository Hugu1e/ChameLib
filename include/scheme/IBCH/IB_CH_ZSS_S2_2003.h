#ifndef IB_CH_ZSS_S2_2003_H
#define IB_CH_ZSS_S2_2003_H

#include "../../base/PbcElements.h"
#include "../../base/PbcScheme.h"
#include "../../utils/Hash.h"

class IB_CH_ZSS_S2_2003_msk:public PbcElements{
    public:
        IB_CH_ZSS_S2_2003_msk(){
            init(1);
        }
};

class IB_CH_ZSS_S2_2003_pp:public PbcElements{
    public:
        IB_CH_ZSS_S2_2003_pp(){
            init(3);
        }
};

class IB_CH_ZSS_S2_2003_sk:public PbcElements{
    public:
        IB_CH_ZSS_S2_2003_sk(){
            init(1);
        }
};

class IB_CH_ZSS_S2_2003_h:public PbcElements{
    public:
        IB_CH_ZSS_S2_2003_h(){
            init(1);
        }
};
  
class IB_CH_ZSS_S2_2003_r:public PbcElements{
    public:
        IB_CH_ZSS_S2_2003_r(){
            init(1);
        }
};

class IB_CH_ZSS_S2_2003: public PbcScheme{
    private:
        void H1(element_t res, element_t m);

        element_t tmp_G, tmp_G_2, tmp_GT, tmp_GT_2, tmp_Zn, tmp_Zn_2, tmp_Zn_3;
        
    public:
        IB_CH_ZSS_S2_2003(int curve);

        void SetUp(IB_CH_ZSS_S2_2003_pp &pp, IB_CH_ZSS_S2_2003_msk &msk);

        void Extract(IB_CH_ZSS_S2_2003_sk &sk, IB_CH_ZSS_S2_2003_msk &msk, element_t ID, IB_CH_ZSS_S2_2003_pp &pp);

        void Hash(IB_CH_ZSS_S2_2003_h &h, IB_CH_ZSS_S2_2003_r &r, element_t m, element_t ID, IB_CH_ZSS_S2_2003_pp &pp);

        bool Check(IB_CH_ZSS_S2_2003_h &h, element_t m, IB_CH_ZSS_S2_2003_r &r, element_t ID, IB_CH_ZSS_S2_2003_pp &pp);

        void Adapt(IB_CH_ZSS_S2_2003_r &r_p, element_t m_p, IB_CH_ZSS_S2_2003_h &h, element_t m, IB_CH_ZSS_S2_2003_r &r, element_t ID, IB_CH_ZSS_S2_2003_sk &sk, IB_CH_ZSS_S2_2003_pp &pp);

        bool Verify(IB_CH_ZSS_S2_2003_h &h_p, element_t m_p, IB_CH_ZSS_S2_2003_r &r_p, element_t ID, IB_CH_ZSS_S2_2003_pp &pp);

        ~IB_CH_ZSS_S2_2003();

        enum {
            s
        };

        enum {
            P, Ppub, ePP
        };

        enum {
            SID
        };

        enum {
            r1
        };

        enum {
            h1
        };
};

#endif  // IB_CH_ZSS_S2_2003_H