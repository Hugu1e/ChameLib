#ifndef RPCH_TMM_2022_H
#define RPCH_TMM_2022_H

#include <utils/Hash.h>
#include <base/PbcScheme.h>
#include <base/PbcElements.h>

#include <AE/RSA.h>
#include <ABE/RABE_TMM.h>
#include <SE/AES.h>

class RPCH_TMM_2022_sk{
    private:
        RABE_TMM_msk mskRABE;
        PbcElements skCHET;
    public:
        RABE_TMM_msk &get_mskRABE(){
            return mskRABE;
        }
        PbcElements &get_skCHET(){
            return skCHET;
        }
};

class RPCH_TMM_2022_pk{
    private:
        RABE_TMM_mpk mpkRABE;
        PbcElements pkCHET;
    public:
        RABE_TMM_mpk &get_mpkRABE(){
            return mpkRABE;
        }
        PbcElements &get_pkCHET(){
            return pkCHET;
        }
};

class RPCH_TMM_2022_skid{
    private:
        PbcElements skCHET;
        RABE_TMM_skid skidRABE;
    public:
        PbcElements &get_skCHET(){
            return skCHET;
        }
        RABE_TMM_skid &get_skidRABE(){
            return skidRABE;
        }
};

class RPCH_TMM_2022_dkidt{
    private:
        PbcElements skCHET;
        RABE_TMM_dkidt dkidtRABE;
    public:
        PbcElements &get_skCHET(){
            return skCHET;
        }
        RABE_TMM_dkidt &get_dkidtRABE(){
            return dkidtRABE;
        }
};

class RPCH_TMM_2022_r: public PbcElements{
    private:
        // h,r
        // C
        RABE_TMM_ciphertext C;
    public:
        RABE_TMM_ciphertext &get_C(){
            return C;
        }
};

class RPCH_TMM_2022_h{
    private:
        PbcElements h;
        RPCH_TMM_2022_r r;
    public:
        PbcElements &get_h(){
            return h;
        }
        RPCH_TMM_2022_r &get_r(){
            return r;
        }
};

class RPCH_TMM_2022_RevokedPresonList{
    private:
        std::vector<RABE_TMM_revokedPreson> rl;
    public:
        std::vector<RABE_TMM_revokedPreson> &get_rl(){
            return rl;
        }
};

class RPCH_TMM_2022_Binary_tree{
    private:
        Binary_tree_RABE st;
    public:
        Binary_tree_RABE &get_st(){
            return st;
        }
};

class RPCH_TMM_2022_kut{
    private:
        RABE_TMM_kut kut;
    public:
        RABE_TMM_kut &get_kut(){
            return kut;
        }
};


class RPCH_TMM_2022: public PbcScheme{
    private:
        RABE_TMM rabe;
    
        int k;
        element_t s1,s2;
        element_t K;
        element_t R;
    public:
        RPCH_TMM_2022(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void SetUp(RPCH_TMM_2022_sk &skRPCH, RPCH_TMM_2022_pk &pkRPCH, RPCH_TMM_2022_RevokedPresonList &rl, RPCH_TMM_2022_Binary_tree &st, int k, int n);

        void KeyGen(RPCH_TMM_2022_skid &skidRPCH, RPCH_TMM_2022_pk &pkRPCH, RPCH_TMM_2022_sk &skRPCH, RPCH_TMM_2022_Binary_tree &st, element_t id, std::vector<std::string> &attr_list);

        void KUpt(RPCH_TMM_2022_kut &kut, RPCH_TMM_2022_pk &pkRPCH, RPCH_TMM_2022_Binary_tree &st, RPCH_TMM_2022_RevokedPresonList &rl, time_t t);

        void DKGen(RPCH_TMM_2022_dkidt &dkidtRPCH, RPCH_TMM_2022_pk &pkRPCH, RPCH_TMM_2022_skid &skidRPCH, RPCH_TMM_2022_kut &kut);

        void Rev(RPCH_TMM_2022_RevokedPresonList &rl, element_t id, time_t t);

        void Hash(RPCH_TMM_2022_h &h, element_t m, RPCH_TMM_2022_pk &pkRPCH, std::string policy_str, time_t t);

        bool Check(RPCH_TMM_2022_pk &pkRPCH, element_t m, RPCH_TMM_2022_h &h);

        void Adapt(RPCH_TMM_2022_h &h_p, element_t m_p, element_t m, RPCH_TMM_2022_h &h, RPCH_TMM_2022_pk &pkRPCH, RPCH_TMM_2022_dkidt &dkidtRPCH);

        bool Verify(RPCH_TMM_2022_pk &pkRPCH, element_t m_p, RPCH_TMM_2022_h &h_p);

        ~RPCH_TMM_2022();

        enum {
            x
        };
        
        enum{
            y
        };

        enum{
            b
        };

        enum{
            h1, r
        };
};


#endif //RPCH_TMM_2022_H