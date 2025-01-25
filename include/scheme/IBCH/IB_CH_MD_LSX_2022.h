#ifndef IB_CH_MD_LSX_2022_H
#define IB_CH_MD_LSX_2022_H

#include <base/PbcElements.h>
#include <base/PbcScheme.h>
#include <utils/Hash.h>

class IB_CH_MD_LSX_2022_pp: public PbcElements {};
class IB_CH_MD_LSX_2022_msk: public PbcElements {};
class IB_CH_MD_LSX_2022_td : public PbcElements {};
class IB_CH_MD_LSX_2022_h : public PbcElements {};
class IB_CH_MD_LSX_2022_r : public PbcElements {};

class IB_CH_MD_LSX_2022: public PbcScheme {
    public:
        IB_CH_MD_LSX_2022(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void SetUp(IB_CH_MD_LSX_2022_pp *pp, IB_CH_MD_LSX_2022_msk *msk);

        void KeyGen(IB_CH_MD_LSX_2022_td *td, element_t ID, IB_CH_MD_LSX_2022_msk *msk, IB_CH_MD_LSX_2022_pp *pp);

        void Hash(IB_CH_MD_LSX_2022_h *h, IB_CH_MD_LSX_2022_r *r, element_t ID, element_t m, IB_CH_MD_LSX_2022_pp *pp);

        bool Check(IB_CH_MD_LSX_2022_h h, IB_CH_MD_LSX_2022_r *r, element_t ID, element_t m, IB_CH_MD_LSX_2022_pp *pp);

        void Adapt(IB_CH_MD_LSX_2022_r *r_p, IB_CH_MD_LSX_2022_h *h, element_t m, IB_CH_MD_LSX_2022_r *r, element_t m_p, IB_CH_MD_LSX_2022_td *td);

        bool Verify(IB_CH_MD_LSX_2022_h h, IB_CH_MD_LSX_2022_r *r_p, element_t ID, element_t m_p, IB_CH_MD_LSX_2022_pp *pp);

        ~IB_CH_MD_LSX_2022();
};


#endif //IB_CH_MD_LSX_2022_H