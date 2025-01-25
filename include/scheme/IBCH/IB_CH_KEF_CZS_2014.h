#ifndef IB_CH_KEF_CZS_2014_H
#define IB_CH_KEF_CZS_2014_H

#include <base/PbcElements.h>
#include <base/PbcScheme.h>
#include <utils/Hash.h>

class IB_CH_KEF_CZS_2014_pp: public PbcElements {};
class IB_CH_KEF_CZS_2014_td: public PbcElements {};  // trapdoor key
class IB_CH_KEF_CZS_2014_h: public PbcElements {};
class IB_CH_KEF_CZS_2014_r: public PbcElements {};


class IB_CH_KEF_CZS_2014 : public PbcScheme {
    private:
        element_t x;

    public:
        IB_CH_KEF_CZS_2014(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void SetUp(IB_CH_KEF_CZS_2014_pp *pp);

        void Extract(IB_CH_KEF_CZS_2014_td *td, std::string ID);

        void H(element_t res, std::string m);

        void Hash(IB_CH_KEF_CZS_2014_h *h, IB_CH_KEF_CZS_2014_r *r, std::string ID, std::string L, element_t m, IB_CH_KEF_CZS_2014_pp *pp);

        bool Check(IB_CH_KEF_CZS_2014_h *h, IB_CH_KEF_CZS_2014_r *r, std::string L,element_t m, IB_CH_KEF_CZS_2014_td *td);

        void Adapt(IB_CH_KEF_CZS_2014_r *r_p, element_t m_p, IB_CH_KEF_CZS_2014_h *h, IB_CH_KEF_CZS_2014_r *r, std::string L, element_t m, IB_CH_KEF_CZS_2014_td *td);

        bool Verify(IB_CH_KEF_CZS_2014_h *h, IB_CH_KEF_CZS_2014_r *r, std::string L,element_t m, IB_CH_KEF_CZS_2014_td *td);

        ~IB_CH_KEF_CZS_2014();
};


#endif //IB_CH_KEF_CZS_2014_H