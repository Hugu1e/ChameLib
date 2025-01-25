#ifndef CR_CH_DSS_2020_H
#define CR_CH_DSS_2020_H

#include <base/PbcScheme.h>
#include <base/PbcElements.h>
#include <utils/Hash.h>

class CR_CH_DSS_2020_pp: public PbcElements{};
class CR_CH_DSS_2020_pk: public PbcElements{};
class CR_CH_DSS_2020_sk: public PbcElements{};
class CR_CH_DSS_2020_h: public PbcElements{};
class CR_CH_DSS_2020_r: public PbcElements{};

class CR_CH_DSS_2020: public PbcScheme{
    private:
        element_t xi,k1,e2,s2;
        element_t u11,u12,u2,e,e1,s1;
        element_t k2;


    public:
        CR_CH_DSS_2020(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void SetUp(CR_CH_DSS_2020_pp *pp);

        void KeyGen(CR_CH_DSS_2020_pk *pk, CR_CH_DSS_2020_sk *sk, CR_CH_DSS_2020_pp *pp);

        void H(element_t res, element_t y, element_t h1, element_t h2, element_t m, element_t u11,element_t u12,element_t u2);

        void Hash(CR_CH_DSS_2020_h *h, CR_CH_DSS_2020_r *r,  element_t m, CR_CH_DSS_2020_pk *pk, CR_CH_DSS_2020_pp *pp);

        bool Check(CR_CH_DSS_2020_h *h, CR_CH_DSS_2020_r *r,  element_t m, CR_CH_DSS_2020_pk *pk, CR_CH_DSS_2020_pp *pp);

        void Adapt(CR_CH_DSS_2020_r *r_p, CR_CH_DSS_2020_h *h, CR_CH_DSS_2020_r *r, element_t m, element_t m_p, CR_CH_DSS_2020_sk *sk, CR_CH_DSS_2020_pk *pk, CR_CH_DSS_2020_pp *pp);
                                    
        bool Verify(CR_CH_DSS_2020_h *h, CR_CH_DSS_2020_r *r_p,  element_t m_p, CR_CH_DSS_2020_pk *pk, CR_CH_DSS_2020_pp *pp);

        ~CR_CH_DSS_2020();
};


#endif //CR_CH_DSS_2020_H