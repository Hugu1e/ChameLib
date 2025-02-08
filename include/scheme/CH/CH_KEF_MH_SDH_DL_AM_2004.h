#ifndef CH_KEF_MH_SDH_DL_AM_2004_H
#define CH_KEF_MH_SDH_DL_AM_2004_H

#include <stdexcept>
#include <base/PbcScheme.h>
#include <base/PbcElements_copy.h>
#include <utils/Hash.h>

class CH_KEF_MH_SDH_DL_AM_2004_pp : public PbcElements_copy{};
class CH_KEF_MH_SDH_DL_AM_2004_pk : public PbcElements_copy{};
class CH_KEF_MH_SDH_DL_AM_2004_sk : public PbcElements_copy{};

class CH_KEF_MH_SDH_DL_AM_2004: public PbcScheme{
    private:
        void H(element_t res, element_t m);

    public:
        CH_KEF_MH_SDH_DL_AM_2004(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);
        
        void SetUp(CH_KEF_MH_SDH_DL_AM_2004_pp &pp, CH_KEF_MH_SDH_DL_AM_2004_pk &pk, CH_KEF_MH_SDH_DL_AM_2004_sk &sk);

        void KeyGen(CH_KEF_MH_SDH_DL_AM_2004_pk &pk, CH_KEF_MH_SDH_DL_AM_2004_sk &sk, CH_KEF_MH_SDH_DL_AM_2004_pp &pp);

        void Hash(element_t h, element_t r, element_t label, element_t m, CH_KEF_MH_SDH_DL_AM_2004_pk &pk,  CH_KEF_MH_SDH_DL_AM_2004_pp &pp);

        bool Check(element_t m, element_t r, CH_KEF_MH_SDH_DL_AM_2004_pk &pk, element_t h, element_t label, CH_KEF_MH_SDH_DL_AM_2004_pp &pp);

        void Adapt(element_t r_p, element_t h, element_t m, element_t label, element_t r, element_t m_p, CH_KEF_MH_SDH_DL_AM_2004_sk &sk);

        bool Verify(element_t m_p, element_t r_p, CH_KEF_MH_SDH_DL_AM_2004_pk &pk, element_t h, element_t label, CH_KEF_MH_SDH_DL_AM_2004_pp &pp);

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