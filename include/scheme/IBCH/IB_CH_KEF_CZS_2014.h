#ifndef IB_CH_KEF_CZS_2014_H
#define IB_CH_KEF_CZS_2014_H

#include "../../base/PbcElements.h"
#include "../../base/PbcScheme.h"
#include "../../utils/Hash.h"
#include <stdexcept>

class IB_CH_KEF_CZS_2014_pp: public PbcElements {};
class IB_CH_KEF_CZS_2014_msk: public PbcElements {};
class IB_CH_KEF_CZS_2014_td: public PbcElements {};
class IB_CH_KEF_CZS_2014_h: public PbcElements {};
class IB_CH_KEF_CZS_2014_r: public PbcElements {};


class IB_CH_KEF_CZS_2014 : public PbcScheme {
    private:
        element_t tmp_G, tmp_G_2, tmp_H, tmp_GT, tmp_GT_2, tmp_GT_3, tmp_Zn;

        bool swap;

        void H(element_t res, std::string m);

        void Pairing(element_t res, element_t g1, element_t g2);

    public:
        IB_CH_KEF_CZS_2014(int curve, bool swap);

        void SetUp(IB_CH_KEF_CZS_2014_pp &pp, IB_CH_KEF_CZS_2014_msk &msk, IB_CH_KEF_CZS_2014_td &td, IB_CH_KEF_CZS_2014_h &h, IB_CH_KEF_CZS_2014_r &r, IB_CH_KEF_CZS_2014_r &r_p);

        void Extract(IB_CH_KEF_CZS_2014_td &td, std::string ID, IB_CH_KEF_CZS_2014_msk &msk);

        void Hash(IB_CH_KEF_CZS_2014_h &h, IB_CH_KEF_CZS_2014_r &r, std::string ID, std::string L, element_t m, IB_CH_KEF_CZS_2014_pp &pp);

        bool Check(IB_CH_KEF_CZS_2014_h &h, IB_CH_KEF_CZS_2014_r &r, std::string L, element_t m, IB_CH_KEF_CZS_2014_td &td);

        void Adapt(IB_CH_KEF_CZS_2014_r &r_p, element_t m_p, IB_CH_KEF_CZS_2014_h &h, IB_CH_KEF_CZS_2014_r &r, std::string L, element_t m, IB_CH_KEF_CZS_2014_td &td);

        bool Verify(IB_CH_KEF_CZS_2014_h &h, IB_CH_KEF_CZS_2014_r &r, std::string L, element_t m, IB_CH_KEF_CZS_2014_td &td);

        ~IB_CH_KEF_CZS_2014();

        enum {
            P,
            Ppub
        };

        enum {
            SID
        };

        enum{
            r1,
            r2
        };

        enum{
            h1
        };
};


#endif //IB_CH_KEF_CZS_2014_H