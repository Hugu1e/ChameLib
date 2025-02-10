#ifndef IB_CH_ZSS_S2_2003_H
#define IB_CH_ZSS_S2_2003_H
#include <base/PbcElements.h>
#include <base/PbcScheme.h>
#include <utils/Hash.h>

class IB_CH_ZSS_S2_2003_msk:public PbcElements{};

class IB_CH_ZSS_S2_2003_pp:public PbcElements{};

class IB_CH_ZSS_S2_2003_sk:public PbcElements{};

class IB_CH_ZSS_S2_2003_h{
    private:
        PbcElements h;
        PbcElements r;
    public:
        PbcElements& get_h(){
            return h;
        }
        PbcElements& get_r(){
            return r;
        }
};

class IB_CH_ZSS_S2_2003: public PbcScheme{
    private:
        void H1(element_t res, element_t m);
    public:
        IB_CH_ZSS_S2_2003(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void SetUp(IB_CH_ZSS_S2_2003_msk &msk, IB_CH_ZSS_S2_2003_pp &pp, IB_CH_ZSS_S2_2003_sk &sk, IB_CH_ZSS_S2_2003_h &h, IB_CH_ZSS_S2_2003_h &h_p);

        void Extract(IB_CH_ZSS_S2_2003_sk &sk, IB_CH_ZSS_S2_2003_msk &msk, element_t ID, IB_CH_ZSS_S2_2003_pp &pp);

        void Hash(IB_CH_ZSS_S2_2003_h &h, element_t m, element_t ID, IB_CH_ZSS_S2_2003_pp &pp);

        bool Check(IB_CH_ZSS_S2_2003_h &h, element_t m, element_t ID, IB_CH_ZSS_S2_2003_pp &pp);

        void Adapt(IB_CH_ZSS_S2_2003_h &h_p, element_t m_p, IB_CH_ZSS_S2_2003_h &h, element_t m, element_t ID, IB_CH_ZSS_S2_2003_sk &sk, IB_CH_ZSS_S2_2003_pp &pp);

        bool Verify(IB_CH_ZSS_S2_2003_h &h_p, element_t m_p, element_t ID, IB_CH_ZSS_S2_2003_pp &pp);

        ~IB_CH_ZSS_S2_2003();

        enum {
            s
        };

        enum {
            P,
            Ppub
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