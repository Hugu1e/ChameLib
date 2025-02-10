#ifndef FCR_CH_PreQA_DKS_2020_H
#define FCR_CH_PreQA_DKS_2020_H

#include <base/PbcScheme.h>
#include <base/PbcElements.h>
#include <utils/Hash.h>
#include <stdexcept>

class FCR_CH_PreQA_DKS_2020_pp: public PbcElements{};

class FCR_CH_PreQA_DKS_2020_pk: public PbcElements{};

class FCR_CH_PreQA_DKS_2020_sk: public PbcElements{};

class FCR_CH_PreQA_DKS_2020_h: public PbcElements{};

class FCR_CH_PreQA_DKS_2020_r: public PbcElements{};

class FCR_CH_PreQA_DKS_2020: public PbcScheme{
    private:
        element_t xi,k11,k12,k2;
        element_t u1,u2,e;
        element_t a;

        void H(element_t res, element_t y, element_t h, element_t m, element_t u1,element_t u2);

    public:
        FCR_CH_PreQA_DKS_2020(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void SetUp(FCR_CH_PreQA_DKS_2020_pp &pp, FCR_CH_PreQA_DKS_2020_pk &pk, FCR_CH_PreQA_DKS_2020_sk &sk, FCR_CH_PreQA_DKS_2020_h &h, FCR_CH_PreQA_DKS_2020_r &r, FCR_CH_PreQA_DKS_2020_r &r_p);

        void KeyGen(FCR_CH_PreQA_DKS_2020_pk &pk, FCR_CH_PreQA_DKS_2020_sk &sk, FCR_CH_PreQA_DKS_2020_pp &pp);

        void Hash(FCR_CH_PreQA_DKS_2020_h &h, FCR_CH_PreQA_DKS_2020_r &r, element_t m, FCR_CH_PreQA_DKS_2020_pk &pk, FCR_CH_PreQA_DKS_2020_pp &pp);

        bool Check(FCR_CH_PreQA_DKS_2020_h &h, FCR_CH_PreQA_DKS_2020_r &r, element_t m, FCR_CH_PreQA_DKS_2020_pk &pk, FCR_CH_PreQA_DKS_2020_pp &pp);

        void Adapt(FCR_CH_PreQA_DKS_2020_r &r_p, element_t m_p, element_t m, FCR_CH_PreQA_DKS_2020_h &h, FCR_CH_PreQA_DKS_2020_r &r, FCR_CH_PreQA_DKS_2020_sk &sk, FCR_CH_PreQA_DKS_2020_pp &pp);

        bool Verify(FCR_CH_PreQA_DKS_2020_h &h, FCR_CH_PreQA_DKS_2020_r &r_p, element_t m_p, FCR_CH_PreQA_DKS_2020_pk &pk, FCR_CH_PreQA_DKS_2020_pp &pp);

        ~FCR_CH_PreQA_DKS_2020();

        enum {
            g1, g2
        };

        enum {
            x
        };

        enum {
            y
        };

        enum {
            h1
        };

        enum {
            e1, e2, s11, s12, s2
        };
};


#endif //FCR_CH_PreQA_DKS_2020_H