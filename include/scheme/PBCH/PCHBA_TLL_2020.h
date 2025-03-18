#ifndef PCHBA_TLL_2020_H
#define PCHBA_TLL_2020_H

#include "../../utils/Hash.h"
#include "../../base/PbcScheme.h"
#include "../../base/PbcElements.h"
#include "../../utils/RandomGenerator.h"

#include "../../ABE/ABET.h"

class PCHBA_TLL_2020_sk{
    private:
        ABET_msk skABET;
        PbcElements skCHET;
    public:
        ABET_msk& get_skABET(){
            return skABET;
        }
        PbcElements& get_skCHET(){
            return skCHET;
        }
};

class PCHBA_TLL_2020_pk{
    private:
        ABET_mpk pkABET;
        PbcElements pkCHET;
    public:
        ABET_mpk& get_pkABET(){
            return pkABET;
        }
        PbcElements& get_pkCHET(){
            return pkCHET;
        }
};

class PCHBA_TLL_2020_sks{
    private:
        PbcElements skCHET;
        ABET_sks sksABET;
    public:
        PCHBA_TLL_2020_sks(){
            skCHET.init(1);
        }
        PbcElements& get_skCHET(){
            return skCHET;
        }
        ABET_sks& get_sksABET(){
            return sksABET;
        }
};

class PCHBA_TLL_2020_ID{
    private:
        ABET_ID IDABET;
    public:
        ABET_ID& get_IDABET(){
            return IDABET;
        }
};

class PCHBA_TLL_2020_r{
    private:
        PbcElements r;  // p, h_, c, epk, sigma
        ABET_ciphertext C;
    public:
        PCHBA_TLL_2020_r(){
            r.init(5);
        }
        PbcElements& get_r(){
            return r;
        }
        ABET_ciphertext& get_C(){
            return C;
        }
};

class PCHBA_TLL_2020_h{
    private:
        PbcElements h;
    public:
        PCHBA_TLL_2020_h(){
            h.init(1);
        }
        PbcElements& get_h(){
            return h;
        }
};

class PCHBA_TLL_2020: public PbcScheme{
    private:
        ABET abet;

        int k;
        element_t r;
        element_t s1,s2;
        element_t esk;

        element_t tmp_G, tmp_G_2, tmp_H, tmp_H_2, tmp_GT, tmp_GT_2, tmp_GT_3, tmp_Zn, tmp_Zn_2, tmp_Zn_3;

        bool swap;

        bool Pairing(element_t res, element_t a, element_t b);

        void H(element_t res, std::string str);

    public:
        PCHBA_TLL_2020(int curve, bool swap);

        void SetUp(PCHBA_TLL_2020_pk &pkPCHBA, PCHBA_TLL_2020_sk &skPCHBA, int k);

        void KeyGen(PCHBA_TLL_2020_sks &sksPCHBA, PCHBA_TLL_2020_pk &pkPCHBA, PCHBA_TLL_2020_sk &skPCHBA, std::vector<std::string> &attr_list, PCHBA_TLL_2020_ID &ID, int mi);

        void Hash(PCHBA_TLL_2020_h &h, PCHBA_TLL_2020_r &r, element_t m, PCHBA_TLL_2020_pk &pkPCHBA, PCHBA_TLL_2020_sk &skPCHBA, Element_t_matrix *MSP, PCHBA_TLL_2020_ID &ID, int oj);

        bool Check(PCHBA_TLL_2020_h &h, PCHBA_TLL_2020_r &random, element_t m, PCHBA_TLL_2020_pk &pkPCHBA);

        void Adapt(PCHBA_TLL_2020_r &random_p, element_t m_p, PCHBA_TLL_2020_h &h, PCHBA_TLL_2020_r &random, element_t m, Element_t_matrix *MSP, PCHBA_TLL_2020_ID &ID, int mi, PCHBA_TLL_2020_pk &pkPCHBA, PCHBA_TLL_2020_sk &skPCHBA, PCHBA_TLL_2020_sks &sksPCHBA);

        bool Verify(PCHBA_TLL_2020_h &h_p, PCHBA_TLL_2020_r &random_p, element_t m_p, PCHBA_TLL_2020_pk &pkPCHBA);

        ~PCHBA_TLL_2020();

        enum{
            x
        };

        enum{
            h_pow_x
        };

        enum{
            b
        };

        enum{
            h_, p, c, epk, sigma
        };
};


#endif //PCHBA_TLL_2020_H