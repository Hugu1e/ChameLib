#ifndef PCH_DSS_2019_H
#define PCH_DSS_2019_H

#include "../../utils/Hash.h"
#include "../../base/PbcScheme.h"
#include "../../base/GmpElements.h"

#include "../../ABE/CP_ABE.h"
#include "../../SE/AES.h"
#include "../../AE/RSA.h"
#include "../CH/CH_ET_BC_CDK_2017.h"

class PCH_DSS_2019_pp{
    private:
        CH_ET_BC_CDK_2017_pp ppCHET;
    public:
        CH_ET_BC_CDK_2017_pp& get_ppCHET(){
            return ppCHET;
        }
};

class PCH_DSS_2019_sk{
    private:
        CP_ABE_msk mskABE;
        CH_ET_BC_CDK_2017_sk skCHET;
    public:
        CP_ABE_msk& getMskABE(){
            return mskABE;
        }
        CH_ET_BC_CDK_2017_sk& get_skCHET(){
            return skCHET;
        }
};

class PCH_DSS_2019_pk{
    private:
        CP_ABE_mpk mpkABE;
        CH_ET_BC_CDK_2017_pk pkCHET;
    public:
        CP_ABE_mpk& getMpkABE(){
            return mpkABE;
        }
        CH_ET_BC_CDK_2017_pk& get_pkCHET(){
            return pkCHET;
        }
};

class PCH_DSS_2019_sks{
    private:
        CH_ET_BC_CDK_2017_sk skCHET;
        CP_ABE_sks sksABE;
    public:
        CH_ET_BC_CDK_2017_sk& get_skCHET(){
            return skCHET;
        }
        CP_ABE_sks& getSksABE(){
            return sksABE;
        }
};

class PCH_DSS_2019_r{
    private:
        CH_ET_BC_CDK_2017_r rCHET;
    public:
        CH_ET_BC_CDK_2017_r& get_rCHET(){
            return rCHET;
        }
};

class PCH_DSS_2019_h{
    private:
        CH_ET_BC_CDK_2017_h h;
        CP_ABE_ciphertext ct;
        GmpElements ct_;
    public:
        PCH_DSS_2019_h(){
            ct_.init(1);
        }
        CH_ET_BC_CDK_2017_h& get_h(){
            return h;
        }
        CP_ABE_ciphertext& getCt(){
            return ct;
        }
        GmpElements& getCt_(){
            return ct_;
        }
};

class PCH_DSS_2019: public PbcScheme{
    private:
        CH_ET_BC_CDK_2017 ch;
        CP_ABE cp_abe;
        AES aes;

        bool swap;
        int k;

        element_t tmp_GT, tmp_Zn, tmp_Zn_2, tmp_Zn_3, tmp_Zn_4;

        void H1(mpz_t res, mpz_t m, mpz_t N1, mpz_t N2, mpz_t n);
        void H2(mpz_t res, mpz_t m, mpz_t N1, mpz_t N2, mpz_t n);
        void H4(element_t u1, element_t u2, unsigned char *r, int r_len, std::string A);

        void Encode(element_t res, unsigned char *k, unsigned char *r, int k_len, int r_len);
        void Decode(unsigned char *k, unsigned char * r, element_t res);

    public:
        PCH_DSS_2019(int curve, bool swap);

        void SetUp(PCH_DSS_2019_pp &pp, PCH_DSS_2019_pk &pk, PCH_DSS_2019_sk &sk, int k);

        void KeyGen(PCH_DSS_2019_sks &sks, PCH_DSS_2019_sk &sk, PCH_DSS_2019_pk &pk, std::vector<std::string> &attr_list);

        void Hash(PCH_DSS_2019_h &h, PCH_DSS_2019_r &r, std::string m, Element_t_matrix *MSP, std::string policy_str, PCH_DSS_2019_pk &pk, PCH_DSS_2019_pp &pp);

        bool Check(PCH_DSS_2019_h &h, PCH_DSS_2019_r &r, std::string m, PCH_DSS_2019_pk &pk);

        void Adapt(PCH_DSS_2019_r &r_p, std::string m_p, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r, std::string m, PCH_DSS_2019_sks &sks, PCH_DSS_2019_pk &pk, Element_t_matrix *MSP, std::string policy_str);

        bool Verify(PCH_DSS_2019_h &h, PCH_DSS_2019_r &r_p, std::string m_p, PCH_DSS_2019_pk &pk);

        ~PCH_DSS_2019();
};


#endif //PCH_DSS_2019_H