#ifndef CR_CH_DSS_2020_H
#define CR_CH_DSS_2020_H

#include "../../base/PbcScheme.h"
#include "../../base/PbcElements.h"
#include "../../utils/Hash.h"
#include "../../utils/Func.h"

class CR_CH_DSS_2020_pp: public PbcElements{};
class CR_CH_DSS_2020_pk: public PbcElements{};
class CR_CH_DSS_2020_sk: public PbcElements{};
class CR_CH_DSS_2020_h: public PbcElements{};
class CR_CH_DSS_2020_r: public PbcElements{};

class CR_CH_DSS_2020: public PbcScheme{
    private:
        int curve; 
        int group;
        element_t xi,k1;
        element_t u11,u12,u2,e;
        element_t k2;

        element_t tmp_G, tmp_G_2, tmp_G_3, tmp_G_4, tmp_Zn, tmp_Zn_2;

        void H(element_t res, element_t y, element_t h1, element_t h2, element_t m, element_t u11,element_t u12,element_t u2);

    public:
        CR_CH_DSS_2020(int curve, int group);

        void SetUp(CR_CH_DSS_2020_pp &pp, CR_CH_DSS_2020_pk &pk, CR_CH_DSS_2020_sk &sk, CR_CH_DSS_2020_h &h, CR_CH_DSS_2020_r &r, CR_CH_DSS_2020_r &r_p);

        void KeyGen(CR_CH_DSS_2020_pk &pk, CR_CH_DSS_2020_sk &sk, CR_CH_DSS_2020_pp &pp);

        void Hash(CR_CH_DSS_2020_h &h, CR_CH_DSS_2020_r &r,  element_t m, CR_CH_DSS_2020_pk &pk, CR_CH_DSS_2020_pp &pp);

        bool Check(CR_CH_DSS_2020_h &h, CR_CH_DSS_2020_r &r,  element_t m, CR_CH_DSS_2020_pk &pk, CR_CH_DSS_2020_pp &pp);

        void Adapt(CR_CH_DSS_2020_r &r_p, CR_CH_DSS_2020_h &h, CR_CH_DSS_2020_r &r, element_t m, element_t m_p, CR_CH_DSS_2020_sk &sk, CR_CH_DSS_2020_pk &pk, CR_CH_DSS_2020_pp &pp);
                        
        bool Verify(CR_CH_DSS_2020_h &h, CR_CH_DSS_2020_r &r_p,  element_t m_p, CR_CH_DSS_2020_pk &pk, CR_CH_DSS_2020_pp &pp);

        ~CR_CH_DSS_2020();

        enum {
            x // sk
        };

        enum {
            y // pk
        };

        enum {
            g // pp
        };

        enum {
            h1, h2 // h
        };

        enum {
            e1, e2, s1, s2 // r
        };
};


#endif //CR_CH_DSS_2020_H