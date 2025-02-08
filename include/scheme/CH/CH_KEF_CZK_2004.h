#ifndef CH_KEF_CZK_2004_H
#define CH_KEF_CZK_2004_H

#include <stdexcept>
#include <base/PbcElements_copy.h>
#include <base/PbcScheme.h>

class CH_KEF_CZK_2004_pp : public PbcElements_copy{};
class CH_KEF_CZK_2004_pk : public PbcElements_copy{};
class CH_KEF_CZK_2004_sk : public PbcElements_copy{};
class CH_KEF_CZK_2004_r : public PbcElements_copy{};

class CH_KEF_CZK_2004: public PbcScheme {
    public:
        CH_KEF_CZK_2004(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void SetUp(CH_KEF_CZK_2004_pp &pp, CH_KEF_CZK_2004_pk &pk, CH_KEF_CZK_2004_sk &sk, CH_KEF_CZK_2004_r &r, CH_KEF_CZK_2004_r &r_p);

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
            r1,
            r2
        };

        enum{
            g
        };
};

#endif //CH_KEF_CZK_2004_H
