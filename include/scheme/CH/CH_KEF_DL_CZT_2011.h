#ifndef CH_KEF_DL_CZT_2011_H
#define CH_KEF_DL_CZT_2011_H

#include <base/PbcElements.h>
#include <base/PbcScheme.h>
#include <utils/Hash.h>

class CH_KEF_DL_CZT_2011_pp : public PbcElements{};
class CH_KEF_DL_CZT_2011_pk : public PbcElements{};
class CH_KEF_DL_CZT_2011_sk : public PbcElements{};
class CH_KEF_DL_CZT_2011_r : public PbcElements{};

class CH_KEF_DL_CZT_2011 : public PbcScheme {
    public:
        CH_KEF_DL_CZT_2011(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void SetUp(CH_KEF_DL_CZT_2011_pp *pp);

        void KeyGen(CH_KEF_DL_CZT_2011_pk *pk, CH_KEF_DL_CZT_2011_sk *sk, CH_KEF_DL_CZT_2011_pp *pp);

        void H(element_t res, element_t m1, element_t m2);

        void Hash(element_t h, CH_KEF_DL_CZT_2011_r *r, element_t I, element_t m, CH_KEF_DL_CZT_2011_pk *pk, CH_KEF_DL_CZT_2011_pp *pp);

        bool Check(element_t h, CH_KEF_DL_CZT_2011_r *r, element_t I, element_t m, CH_KEF_DL_CZT_2011_pk *pk, CH_KEF_DL_CZT_2011_pp *pp);

        void Adapt(CH_KEF_DL_CZT_2011_r *r_p, CH_KEF_DL_CZT_2011_sk *sk, element_t h, element_t m, CH_KEF_DL_CZT_2011_r *r,  element_t m_p, element_t I, CH_KEF_DL_CZT_2011_pp *pp);

        bool Verify(element_t h, CH_KEF_DL_CZT_2011_r *r_p, element_t I, element_t m_p, CH_KEF_DL_CZT_2011_pk *pk, CH_KEF_DL_CZT_2011_pp *pp);

        ~CH_KEF_DL_CZT_2011();
};


#endif //CH_KEF_DL_CZT_2011_H