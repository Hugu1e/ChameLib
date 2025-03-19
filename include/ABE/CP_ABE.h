#ifndef CHAMELIB_CP_ABE_H
#define CHAMELIB_CP_ABE_H

#include "../base/PbcElements.h"
#include "../base/PbcScheme.h"
#include <vector>
#include <unordered_map>
#include "Policy_resolution.h"
#include "Policy_generation.h"
#include "../utils/Hash.h"


class CP_ABE_mpk : public PbcElements{
    public:
        CP_ABE_mpk(){
            this->init(5);
        }
};

class CP_ABE_msk : public PbcElements{
    public:
        CP_ABE_msk(){
            this->init(9);
        }
};

class CP_ABE_sks{
    private:
        PbcElements sk_0;
        std::vector<PbcElements> sk_y;
        PbcElements sk_prime;
        std::unordered_map<std::string, unsigned long int> attr2id;
    public:
        CP_ABE_sks(){
            sk_0.init(3);
            sk_prime.init(3);
        }
        PbcElements &get_sk_0()
        {
            return sk_0;
        }
        PbcElements &get_sk_y(int i)
        {
            return sk_y[i];
        }
        std::vector<PbcElements> &get_sk_y()
        {
            return sk_y;
        }
        PbcElements &get_sk_prime()
        {
            return sk_prime;
        }
        std::unordered_map<std::string, unsigned long int> &get_attr2id()
        {
            return attr2id;
        }
};

class CP_ABE_ciphertext{
    private:
        PbcElements ct_0;
        std::vector<PbcElements> ct_y;
        PbcElements ct_prime;

    public:
        CP_ABE_ciphertext(){
            ct_0.init(3);
            ct_prime.init(1);
        }
        PbcElements &get_ct_0()
        {
            return ct_0;
        }
        PbcElements &get_ct_y(int i)
        {
            return ct_y[i];
        }
        std::vector<PbcElements> &get_ct_y()
        {
            return ct_y;
        }
        PbcElements &get_ct_prime()
        {
            return ct_prime;
        }
        bool operator==(const CP_ABE_ciphertext &other) const
        {
            return ct_0 == other.ct_0 &&
                   ct_y == other.ct_y &&
                   ct_prime == other.ct_prime;
        }
};


class CP_ABE:public PbcScheme{
    private:
        bool swap;

        element_t d1, d2, d3;
        element_t r1, r2;
        element_t b1r1a1, b1r1a2, b2r2a1, b2r2a2, r1r2a1, r1r2a2;
        element_t s1, s2;
        element_t sk_1_G, sk_2_G, sk_3_G, sk_1_H, sk_2_H, sk_3_H;
        element_t ct_1_G, ct_2_G, ct_3_G, ct_1_H, ct_2_H, ct_3_H;

        element_t tmp_G, tmp_G_2, tmp_G_3, tmp_G_4;
        element_t tmp_H;
        element_t tmp_GT, tmp_GT_2, tmp_GT_3;
        element_t tmp_Zn, tmp_Zn_2;

        void Hash(element_t res, std::string m);

        void Pairing(element_t res, element_t a, element_t b);

        void initTmp();

    public:
        CP_ABE(){}
        CP_ABE(int curve, bool swap);

        void init(element_t _G1, element_t _G2, element_t _GT, element_t _Zn,  bool swap);

        void Setup(CP_ABE_msk &msk, CP_ABE_mpk &mpk);

        void KeyGen(CP_ABE_sks &sks, CP_ABE_msk &msk, CP_ABE_mpk &mpk, std::vector<std::string> &attr_list);

        void Encrypt(CP_ABE_ciphertext &ciphertext, CP_ABE_mpk &mpk, element_t msg, Element_t_matrix *MSP, element_t s1, element_t s2);

        void Decrypt(element_t res, CP_ABE_ciphertext &ciphertext, Element_t_matrix *MSP, CP_ABE_mpk &mpk, CP_ABE_sks &sks);

        ~CP_ABE();

        enum {
            h, g, a1, a2, b1, b2,
            g_pow_d1, g_pow_d2, g_pow_d3
        };  // msk

        enum{
            _, H1, H2, T1, T2
        };  // mpk

        enum{
            sk_1, sk_2, sk_3
        };

        enum{
            ct_1, ct_2, ct_3
        };
};

#endif // CHAMELIB_CP_ABE_H