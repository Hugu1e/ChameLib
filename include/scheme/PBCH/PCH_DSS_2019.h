#ifndef PCH_DSS_2019_H
#define PCH_DSS_2019_H

#include <utils/Hash.h>
#include <base/PbcScheme.h>
#include <base/GmpElements.h>

#include <ABE/CP_ABE.h>
#include <SE/AES.h>
#include <AE/RSA.h>

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

        int k;
        element_t u1,u2;
        element_t K;

        void H1(mpz_t res, mpz_t m, mpz_t N1, mpz_t N2, mpz_t n);
        void H2(mpz_t res, mpz_t m, mpz_t N1, mpz_t N2, mpz_t n);
        void H4(element_t u1, element_t u2, mpz_t r, std::string A);

    public:
        PCH_DSS_2019(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void SetUp(PCH_DSS_2019_pk &pkPCH, PCH_DSS_2019_sk &skPCH, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r, PCH_DSS_2019_r &r_p, int k);

        void KeyGen(PCH_DSS_2019_sk &skPCH, PCH_DSS_2019_pk &pkPCH, std::vector<std::string> &attr_list, PCH_DSS_2019_sks &sksPCH);

        void Hash(PCH_DSS_2019_pk &pkPCH, mpz_t m, const std::string &policy_str, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r);

        bool Check(PCH_DSS_2019_pk &pkPCH, mpz_t m, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r);

        void Adapt(PCH_DSS_2019_pk &pkPCH, PCH_DSS_2019_sks &sksPCH, mpz_t m, mpz_t m_p, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r, PCH_DSS_2019_r &r_p);

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