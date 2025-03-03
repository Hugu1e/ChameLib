#ifndef PCH_DSS_2019_H
#define PCH_DSS_2019_H

#include "../../utils/Hash.h"
#include "../../base/PbcScheme.h"
#include "../../base/GmpElements.h"

#include "../../ABE/CP_ABE.h"
#include "../../SE/AES.h"
#include "../../AE/RSA.h"

class PCH_DSS_2019_sk{
    private:
        CP_ABE_msk mskABE;
        GmpElements skCHET;
    public:
        CP_ABE_msk& getMskABE(){
            return mskABE;
        }
        GmpElements& getSkCHET(){
            return skCHET;
        }
};

class PCH_DSS_2019_pk{
    private:
        CP_ABE_mpk mpkABE;
        GmpElements pkCHET;
    public:
        CP_ABE_mpk& getMpkABE(){
            return mpkABE;
        }
        GmpElements& getPkCHET(){
            return pkCHET;
        }
};

class PCH_DSS_2019_sks{
    private:
        GmpElements skCHET;
        CP_ABE_sks sksABE;
    public:
        GmpElements& getSkCHET(){
            return skCHET;
        }
        CP_ABE_sks& getSksABE(){
            return sksABE;
        }
};

class PCH_DSS_2019_r: public GmpElements{};

class PCH_DSS_2019_h{
    private:
        GmpElements h;  // h1,h2,N2
        CP_ABE_ciphertext ct;
        GmpElements ct_;
    public:
        GmpElements& getH(){
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
        AE_RSA rsa;
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

        void SetUp(PCH_DSS_2019_pk &pkPCH, PCH_DSS_2019_sk &skPCH, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r, PCH_DSS_2019_r &r_p, int k);

        void KeyGen(PCH_DSS_2019_sk &skPCH, PCH_DSS_2019_pk &pkPCH, std::vector<std::string> &attr_list, PCH_DSS_2019_sks &sksPCH);

        void Hash(PCH_DSS_2019_pk &pkPCH, mpz_t m, const std::string &policy_str, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r);

        bool Check(PCH_DSS_2019_pk &pkPCH, mpz_t m, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r);

        void Adapt(PCH_DSS_2019_r &r_p, mpz_t m_p, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r, mpz_t m, PCH_DSS_2019_pk &pkPCH, PCH_DSS_2019_sks &sksPCH, const std::string &policy_str);

        bool Verify(PCH_DSS_2019_pk &pkPCH, mpz_t m_p, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r_p);

        ~PCH_DSS_2019();

        enum {
            d1
        };

        enum {
            N1,
            e
        };

        enum {
            N2,
            h1,
            h2
        };

        enum {
            r1,
            r2
        };

        enum {
            ct_
        };
};


#endif //PCH_DSS_2019_H