/**
 * Policy-based Chameleon Hash for Blockchain Rewriting with Black-box Accountability
 */
#ifndef CHAMELIB_ABET_H
#define CHAMELIB_ABET_H

#include "../base/PbcElements.h"
#include "../base/PbcScheme.h"
#include "../utils/Hash.h"
#include <vector>
#include <string>
#include <cstring>
#include <unordered_map>
#include "Policy_resolution.h"
#include "Policy_generation.h"

class ABET_mpk{
    private:
        PbcElements mpk;  // g, h, H1, H2, T1, T2, g_pow_a, h_pow_d_div_a, h_pow_1_div_a, h_pow_b_div_a
        PbcElements gk;  // g^z1, g^z2,...g^zk
        PbcElements gk_pow_a;  // (g^z1)^a, (g^z2)^a,...(g^zk)^a
        PbcElements hk;  // h^z1, h^z2,...h^zk
    public:
        PbcElements& get_mpk(){
            return mpk;
        }
        PbcElements& get_gk(){
            return gk;
        }
        PbcElements& get_gk_pow_a(){
            return gk_pow_a;
        }
        PbcElements& get_hk(){
            return hk;
        }
};
class ABET_msk{
    private:
        PbcElements msk;  // a1, a2, b1, b2, a, b, g_pow_d1, g_pow_d2, g_pow_d3
        PbcElements zk;  // z1,z2,...zk
    public:
        PbcElements& get_msk(){
            return msk;
        }
        PbcElements& get_zk(){
            return zk;
        }
};
class ABET_ID: public PbcElements{};
class ABET_sks{
    private:
        PbcElements sk0;
        std::vector<PbcElements> sk_y;
        PbcElements sk_prime;
        PbcElements sk1;
        PbcElements sk2;
        std::unordered_map<std::string, unsigned long int> attr2id;
    public:
        ABET_sks(){
            sk0.init(6);
            sk_prime.init(3);
            sk1.init(1);
        }
        PbcElements& get_sk0(){
            return sk0;
        }
        PbcElements& get_sk_y(int i){
            return sk_y[i];
        }
        std::vector<PbcElements>& get_sk_y(){
            return sk_y;
        }
        PbcElements& get_sk_prime(){
            return sk_prime;
        }
        PbcElements& get_sk1(){
            return sk1;
        }
        PbcElements& get_sk2(){
            return sk2;
        }
        std::unordered_map<std::string, unsigned long int> &get_attr2id(){
            return attr2id;
        }
};
class ABET_ciphertext{
    private:
        PbcElements ct0;
        std::vector<PbcElements> ct_y;
        PbcElements ct_p;
        unsigned char *ct_;
        unsigned char *ct_prime;
        PbcElements ct1;
        PbcElements ct2;
        PbcElements ct3;
    public:
        ABET_ciphertext(){
            ct0.init(4);
            ct_p.init(1);
            ct1.init(1);
            ct2.init(1);
            ct3.init(1);
        }
        PbcElements& get_ct0(){
            return ct0;
        }
        PbcElements& get_ct_y(int i){
            return ct_y[i];
        }
        std::vector<PbcElements>& get_ct_y(){
            return ct_y;
        }
        PbcElements& get_ct_p(){
            return ct_p;
        }
        void set_ct_(unsigned char *str, int size){
            ct_ = new unsigned char[size];
            memcpy(ct_, str, size);
        }
        void set_ct_prime(unsigned char *str, int size){
            ct_prime = new unsigned char[size];
            memcpy(ct_prime, str, size);
        }
        unsigned char* get_ct_(){
            return ct_;
        }
        unsigned char* get_ct_prime(){
            return ct_prime;
        }
        PbcElements& get_ct1(){
            return ct1;
        }
        PbcElements& get_ct2(){
            return ct2;
        }
        PbcElements& get_ct3(){
            return ct3;
        }
        ~ABET_ciphertext(){
            delete[] ct_;
            delete[] ct_prime;
        }
};

class ABET:public PbcScheme{
    private:
        int k;
        element_t d1,d2,d3;
        element_t r1,r2;
        element_t R;
        element_t b1r1a1,b1r1a2,b2r2a1,b2r2a2,r1r2a1,r1r2a2;
        element_t s1,s2;
        element_t z_1,z_2, z;
        element_t alpha_a_1, alpha_a_2;
        element_t ID_i_1;

        element_t tmp_G, tmp_G_2, tmp_G_3, tmp_G_4;
        element_t tmp_H, tmp_H_2;
        element_t tmp_GT, tmp_GT_2, tmp_GT_3;
        element_t tmp_Zn, tmp_Zn_2;

        bool swap;

        void Pairing(element_t res, element_t a, element_t b);

        bool UserDelegate(ABET_sks &sk, ABET_mpk &mpk, ABET_msk &msk, ABET_ID &ID, int i);
        bool SkDelegate(ABET_sks &sk, ABET_mpk &mpk, ABET_msk &msk, element_t ID_i_1, element_t I_i_1);

    public:
        ABET(){}
        ABET(int curve, bool swap);

        void init(element_t _G1, element_t _G2, element_t _GT, element_t _Zn, bool swap);

        void Setup(ABET_msk &msk, ABET_mpk &mpk, int k);

        void KeyGen(ABET_sks &sks, ABET_msk &msk, ABET_mpk &mpk, std::vector<std::string> &attr_list, ABET_ID &ID, int mi);

        void Encrypt(ABET_ciphertext &ciphertext, ABET_mpk &mpk, ABET_msk &msk, element_t r, unsigned char *R, int size_R, std::string policy_str, ABET_ID &ID, int oj, element_t s1, element_t s2);

        void Decrypt(unsigned char *res_R, element_t res_r, ABET_mpk &mpk, ABET_msk &msk, ABET_ciphertext &ciphertext, ABET_sks &sks, std::string policy_str, ABET_ID &ID, int mi, int oj);

        void Hash(element_t res, std::string m);
        void Hash2(element_t res, element_t m);

        ~ABET();

        const int MODIFIER = 1;
        const int OWNER = 2;
        void GetID_(element_t ID_, ABET_mpk &mpk, ABET_ID &ID, int mi_oj, int type);

        enum{
            a1, a2, b1, b2, a, b, g_pow_d1, g_pow_d2, g_pow_d3
        };

        enum{
            g, h, H1, H2, T1, T2, g_pow_a, h_pow_d_div_a, h_pow_1_div_a, h_pow_b_div_a
        };

        enum{
            sk0_1, sk0_2, sk0_3, sk0_4, sk0_5, sk0_6
        };

        enum{
            sk_1, sk_2, sk_3
        };

        enum{
            ct0_1, ct0_2, ct0_3, ct0_4
        };

        enum{
            ct_1, ct_2, ct_3
        };



};


#endif  // CHAMELIB_ABET_H