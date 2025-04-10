#ifndef RPCH_XNM_2021_H
#define RPCH_XNM_2021_H

#include "../../utils/Hash.h"
#include "../../base/PbcScheme.h"
#include "../../base/GmpElements.h"

#include "../../AE/RSA.h"
#include "../../ABE/RABE_XNM.h"
#include "../../SE/AES.h"
#include "../CH/CH_ET_BC_CDK_2017.h"

class RPCH_XNM_2021_pp{
    private:
        CH_ET_BC_CDK_2017_pp ppCHET;
    public:
        CH_ET_BC_CDK_2017_pp& get_ppCHET(){
            return ppCHET;
        }
};

class RPCH_XNM_2021_sk{
    private:
        RABE_XNM_msk mskRABE;
        CH_ET_BC_CDK_2017_sk skCHET;
    public:
        RABE_XNM_msk& get_mskRABE(){
            return mskRABE;
        }
        CH_ET_BC_CDK_2017_sk& get_skCHET(){
            return skCHET;
        }
};

class RPCH_XNM_2021_pk{
    private:
        RABE_XNM_mpk mpkRABE;
        CH_ET_BC_CDK_2017_pk pkCHET;
    public:
        RABE_XNM_mpk& get_mpkRABE(){
            return mpkRABE;
        }
        CH_ET_BC_CDK_2017_pk& get_pkCHET(){
            return pkCHET;
        }
};

class RPCH_XNM_2021_skid{
    private:
        CH_ET_BC_CDK_2017_sk skCHET;
        RABE_XNM_skid skidRABE;
    public:
        CH_ET_BC_CDK_2017_sk& get_skCHET(){
            return skCHET;
        }
        RABE_XNM_skid& get_skidRABE(){
            return skidRABE;
        }
};

class RPCH_XNM_2021_dkidt{
    private:
        CH_ET_BC_CDK_2017_sk skCHET;
        RABE_XNM_dkidt dkidtRABE;
    public:
        CH_ET_BC_CDK_2017_sk& get_skCHET(){
            return skCHET;
        }
        RABE_XNM_dkidt& get_dkidtRABE(){
            return dkidtRABE;
        }
};

class RPCH_XNM_2021_r{
    private:
        CH_ET_BC_CDK_2017_r rCHET;
    public:
        CH_ET_BC_CDK_2017_r& get_rCHET(){
            return rCHET;
        }
};

class RPCH_XNM_2021_h{
    private:
        CH_ET_BC_CDK_2017_h h;
        RABE_XNM_ciphertext ct;
        unsigned char *cSE = nullptr;
        int cSE_len = 0;
    public:
        CH_ET_BC_CDK_2017_h& get_h(){
            return h;
        }
        RABE_XNM_ciphertext& get_ct(){
            return ct;
        }
        unsigned char *get_cSE(){
            return cSE;
        }
        int get_cSE_len(){
            return cSE_len;
        }
        void set_cSE(unsigned char *cSE, int cSE_len){
            this->cSE = cSE;
            this->cSE_len = cSE_len;
        }
        ~RPCH_XNM_2021_h(){
            if(cSE != nullptr){
                delete []cSE;
            }
        }
};

class RPCH_XNM_2021_RevokedPresonList{
    private:
        std::vector<RABE_XNM_revokedPreson> rl;
    public:
        std::vector<RABE_XNM_revokedPreson>& get_rl(){
            return rl;
        }
};

class RPCH_XNM_2022_Binary_tree{
    private:
        Binary_tree_RABE st;
    public:
        Binary_tree_RABE& get_st(){
            return st;
        }
};

class RPCH_XNM_2022_kut{
    private:
        RABE_XNM_kut kut;
    public:
        RABE_XNM_kut& get_kut(){
            return kut;
        }
};

class RPCH_XNM_2021: public PbcScheme{
    private:
        CH_ET_BC_CDK_2017 ch;
        RABE_XNM rabe;
        AES aes;
    
        element_t s1,s2;
        element_t K;

        bool swap;

    public:
        RPCH_XNM_2021(int curve, bool swap);

        void SetUp(RPCH_XNM_2021_pp &ppRPCH, RPCH_XNM_2021_sk &skRPCH, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2021_RevokedPresonList &rl, RPCH_XNM_2022_Binary_tree &st, int k, int n);

        void KeyGen(RPCH_XNM_2021_skid &skidRPCH, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2021_sk &skRPCH, RPCH_XNM_2022_Binary_tree &st, std::vector<std::string> &attr_list, element_t id, time_t re_time);

        void KUpt(RPCH_XNM_2022_kut &kut, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2022_Binary_tree &st, RPCH_XNM_2021_RevokedPresonList &rl, time_t t);

        void DKGen(RPCH_XNM_2021_dkidt &dkidtRPCH, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2021_skid &skidRPCH, RPCH_XNM_2022_kut &kut);

        void Rev(RPCH_XNM_2021_RevokedPresonList &rl, element_t id, time_t t);

        void Hash(RPCH_XNM_2021_h &h, RPCH_XNM_2021_r &r, std::string m, RPCH_XNM_2021_pk &pkRPCH, Element_t_matrix *MSP, time_t t, RPCH_XNM_2021_pp &ppRPCH);

        bool Check(RPCH_XNM_2021_h &h, RPCH_XNM_2021_r &r, std::string m, RPCH_XNM_2021_pk &pkRPCH);

        void Adapt(RPCH_XNM_2021_r &r_p, std::string m_p, RPCH_XNM_2021_h &h, RPCH_XNM_2021_r &r, std::string m, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2021_dkidt &dkidtRPCH, Element_t_matrix *MSP);

        bool Verify(RPCH_XNM_2021_h &h, RPCH_XNM_2021_r &r_p, std::string m_p, RPCH_XNM_2021_pk &pkRPCH);

        Element_t_matrix* ComputeMSP(const std::string &policy_str);

        ~RPCH_XNM_2021();
};


#endif //RPCH_XNM_2021_H