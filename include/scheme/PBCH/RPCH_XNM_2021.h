#ifndef RPCH_XNM_2021_H
#define RPCH_XNM_2021_H

#include <utils/Hash.h>
#include <base/PbcScheme.h>
#include <base/GmpElements_copy.h>

#include <AE/RSA.h>
#include <ABE/RABE_XNM.h>
#include <SE/AES.h>

class RPCH_XNM_2021_sk{
    private:
        RABE_XNM_msk mskRABE;
        GmpElements_copy skCHET;
    public:
        RABE_XNM_msk& get_mskRABE(){
            return mskRABE;
        }
        GmpElements_copy& get_skCHET(){
            return skCHET;
        }
};

class RPCH_XNM_2021_pk{
    private:
        RABE_XNM_mpk mpkRABE;
        GmpElements_copy pkCHET;
    public:
        RABE_XNM_mpk& get_mpkRABE(){
            return mpkRABE;
        }
        GmpElements_copy& get_pkCHET(){
            return pkCHET;
        }
};

class RPCH_XNM_2021_skid{
    private:
        GmpElements_copy skCHET;
        RABE_XNM_skid skidRABE;
    public:
        GmpElements_copy& get_skCHET(){
            return skCHET;
        }
        RABE_XNM_skid& get_skidRABE(){
            return skidRABE;
        }
};

class RPCH_XNM_2021_dkidt{
    private:
        GmpElements_copy skCHET;
        RABE_XNM_dkidt dkidtRABE;
    public:
        GmpElements_copy& get_skCHET(){
            return skCHET;
        }
        RABE_XNM_dkidt& get_dkidtRABE(){
            return dkidtRABE;
        }
};

class RPCH_XNM_2021_r: public GmpElements_copy{
    private:
        // r1, r2, N2, cSE
        RABE_XNM_ciphertext ct;
    public:
        RABE_XNM_ciphertext& get_ct(){
            return ct;
        }
};

class RPCH_XNM_2021_h{
    private:
        GmpElements_copy h;
        RPCH_XNM_2021_r r;
    public:
        GmpElements_copy& get_h(){
            return h;
        }
        RPCH_XNM_2021_r& get_r(){
            return r;
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
        AE_RSA rsa;
        RABE_XNM rabe;
        AES aes;
    
        int k;
        element_t s1,s2;
        element_t K;

        void H1(mpz_t res, mpz_t m, mpz_t N1, mpz_t N2, mpz_t n);
        void H2(mpz_t res, mpz_t m, mpz_t N1, mpz_t N2, mpz_t n);
        void H4(element_t u1, element_t u2, mpz_t r, std::string A);

    public:
        RPCH_XNM_2021(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void SetUp(RPCH_XNM_2021_sk &skRPCH, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2021_RevokedPresonList &rl, RPCH_XNM_2022_Binary_tree &st, int k, int n);

        void KeyGen(RPCH_XNM_2021_skid &skidRPCH, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2021_sk &skRPCH, RPCH_XNM_2022_Binary_tree &st, element_t id, std::vector<std::string> &attr_list);

        void KUpt(RPCH_XNM_2022_kut &kut, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2022_Binary_tree &st, RPCH_XNM_2021_RevokedPresonList &rl, time_t t);

        void DKGen(RPCH_XNM_2021_dkidt &dkidtRPCH, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2021_skid &skidRPCH, RPCH_XNM_2022_kut &kut);

        void Rev(RPCH_XNM_2021_RevokedPresonList &rl, element_t id, time_t t);

        void Hash(RPCH_XNM_2021_h &h, mpz_t m, RPCH_XNM_2021_pk &pkRPCH, std::string policy_str, time_t t);

        bool Check(RPCH_XNM_2021_pk &pkRPCH, mpz_t m, RPCH_XNM_2021_h &h);

        void Adapt(RPCH_XNM_2021_h &h_p, mpz_t m_p, mpz_t m, RPCH_XNM_2021_h &h, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2021_dkidt &dkidtRPCH);

        bool Verify(RPCH_XNM_2021_pk &pkRPCH, mpz_t m_p, RPCH_XNM_2021_h &h_p);

        ~RPCH_XNM_2021();

        enum {
            d1,   
        };

        enum {
            N1, e
        };

        enum {
            N2, r1, r2, cSE
        };

        enum {
            h1, h2
        };
};


#endif //RPCH_XNM_2021_H