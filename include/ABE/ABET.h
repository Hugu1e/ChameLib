/**
 * Policy-based Chameleon Hash for Blockchain Rewriting with Black-box Accountability
 */
#ifndef CHAMELIB_ABET_H
#define CHAMELIB_ABET_H

#include <base/PbcElements.h>
#include <utils/Hash.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <ABE/Policy_resolution.h>
#include <ABE/Policy_generation.h>

class ABET_mpk: public PbcElements{};
class ABET_msk: public PbcElements{};
class ABET_ID: public PbcElements{};
class ABET_sks{
    private:
        PbcElements sk0;
        std::vector<PbcElements> sk_y;
        PbcElements sk_prime;
        PbcElements sk1;
        PbcElements sk2;
    public:
        PbcElements *get_sk0(){
            return &sk0;
        }
        PbcElements *get_sk_y(int i){
            return &sk_y[i];
        }
        std::vector<PbcElements> *get_sk_y(){
            return &sk_y;
        }
        PbcElements *get_sk_prime(){
            return &sk_prime;
        }
        PbcElements *get_sk1(){
            return &sk1;
        }
        PbcElements *get_sk2(){
            return &sk2;
        }
};
class ABET_ciphertext{
    private:
        PbcElements ct0;
        std::vector<PbcElements> ct_y;
        PbcElements ct_;  // use ct_ to avoid conflict with ct(ct_1, ct_2, ct_3)
        PbcElements ct_prime;
        PbcElements ct1;
        PbcElements ct2;
        PbcElements ct3;
    public:
        PbcElements *get_ct0(){
            return &ct0;
        }
        PbcElements *get_ct_y(int i){
            return &ct_y[i];
        }
        std::vector<PbcElements> *get_ct_y(){
            return &ct_y;
        }
        PbcElements *get_ct_(){
            return &ct_;
        }
        PbcElements *get_ct_prime(){
            return &ct_prime;
        }
        PbcElements *get_ct1(){
            return &ct1;
        }
        PbcElements *get_ct2(){
            return &ct2;
        }
        PbcElements *get_ct3(){
            return &ct3;
        }
};

class ABET{
    private:
        element_s *G1, *G2, *GT, *Zn;

        // temporary variable
        element_t tmp_G,tmp_G_2,tmp_G_3,tmp_G_4,tmp_H,tmp_H_2,tmp_H_3,tmp_GT,tmp_GT_2,tmp_GT_3,tmp_Zn,tmp_Zn_2,tmp_Zn_3;
        int k;
        element_t d1,d2,d3;
        element_t r1,r2;
        element_t R;
        element_t b1r1a1,b1r1a2,b2r2a1,b2r2a2,r1r2a1,r1r2a2;
        element_t s1,s2;

        std::unordered_map<unsigned long int, std::string> pai;  // π(i) -> attr
        std::unordered_map<std::string, unsigned long int> attr_map;  // attr -> index of attr_list
        std::string policy_str;

    public:
        ABET(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void Setup(ABET_msk *msk, ABET_mpk *mpk, int k);

        void KeyGen(ABET_sks *sks, ABET_msk *msk, ABET_mpk *mpk, std::vector<std::string> *attr_list, ABET_ID *ID, int mi);

        void Hash(element_t res, std::string m);
        void Hash2(element_t res, element_t m);


        void Encrypt(ABET_ciphertext *ciphertext, ABET_mpk *mpk, ABET_msk *msk,  element_t *r, element_t *R, std::string policy_str, ABET_ID *ID, int oj,  element_t *s1, element_t *s2);

        void Decrypt(element_t res_R, element_t res_r, ABET_mpk *mpk, ABET_ciphertext *ciphertext, ABET_sks *sks);

        ~ABET();

        const int MODIFIER = 1;
        const int OWNER = 2;
        void GetID_(element_t ID_, ABET_mpk *mpk, ABET_ID *ID, int mi_oj, int type);
};


#endif  // CHAMELIB_ABET_H