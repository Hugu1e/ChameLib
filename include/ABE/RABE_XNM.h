#ifndef CHAMELIB_RABE_XNM_H
#define CHAMELIB_RABE_XNM_H

#include "../base/PbcElements.h"
#include "../base/PbcScheme.h"
#include <vector>
#include <string>
#include <unordered_map>
#include "Policy_resolution.h"
#include "Policy_generation.h"
#include "../utils/Hash.h"
#include "../utils/TimeUtils.h"
#include "../base/Binary_tree_RABE.h"

class RABE_XNM_mpk: public PbcElements{
    public:
        RABE_XNM_mpk(){
            this->init(5);
        }
};
class RABE_XNM_msk: public PbcElements{
    public:
        RABE_XNM_msk(){
            this->init(9);
        }
};
class RABE_XNM_skid{
    private:
        PbcElements sk0;
        std::vector<PbcElements> sk_y;
        PbcElements sk_prime;
        std::vector<std::pair<Binary_tree_RABE_node*,PbcElements>> sk_theta;
        std::unordered_map<std::string, unsigned long int> attr2id;
    public:
        RABE_XNM_skid(){
            sk0.init(3);
            sk_prime.init(2);
        }
        PbcElements& get_sk0()
        {
            return sk0;
        }
        PbcElements& get_sk_y(int i)
        {
            return sk_y[i];
        }
        std::vector<PbcElements>& get_sk_y()
        {
            return sk_y;
        }
        PbcElements& get_sk_prime()
        {
            return sk_prime;
        }
        std::vector<std::pair<Binary_tree_RABE_node*,PbcElements>>& get_sk_theta()
        {
            return sk_theta;
        }
        std::unordered_map<std::string, unsigned long int> &get_attr2id()
        {
            return attr2id;
        }
};
class RABE_XNM_revokedPreson: public PbcElements{
    private:
        time_t time;
    public:
        RABE_XNM_revokedPreson(): PbcElements(){}
        RABE_XNM_revokedPreson(const RABE_XNM_revokedPreson &other) : PbcElements(other), time(other.time){}
        void setTime(time_t time){
            this->time = time;
        }
        time_t getTime(){
            return this->time;
        }
};
class RABE_XNM_kut{
    private:
        time_t t;
        std::vector<std::pair<Binary_tree_RABE_node*,PbcElements>> ku_theta;
    public:
        void setTime(time_t t){
            this->t = t;
        }
        time_t getTime(){
            return this->t;
        }
        std::vector<std::pair<Binary_tree_RABE_node*,PbcElements>>& get_ku_theta()
        {
            return ku_theta;
        }
};
class RABE_XNM_dkidt{
    private:
        time_t t;
        PbcElements sk0_prime;
        std::vector<PbcElements> sk_y;
        PbcElements sk_prime_prime;
        std::unordered_map<std::string, unsigned long int> attr2id;
    public:
        void setTime(time_t t){
            this->t = t;
        }
        time_t getTime(){
            return this->t;
        }
        PbcElements& get_sk0_prime()
        {
            return sk0_prime;
        }
        PbcElements& get_sk_y(int i)
        {
            return sk_y[i];
        }
        std::vector<PbcElements>& get_sk_y()
        {
            return sk_y;
        }
        PbcElements& get_sk_prime_prime()
        {
            return sk_prime_prime;
        }
        std::unordered_map<std::string, unsigned long int>& get_attr2id()
        {
            return attr2id;
        }
};
class RABE_XNM_ciphertext{
    private:
        time_t t;
        PbcElements ct0;
        std::vector<PbcElements> ct_y;
        PbcElements ct_prime;
    public:
        RABE_XNM_ciphertext(){
            ct0.init(4);
            ct_prime.init(1);
        }
        void setTime(time_t t){
            this->t = t;
        }
        time_t getTime(){
            return this->t;
        }
        PbcElements& get_ct0()
        {
            return ct0;
        }
        PbcElements& get_ct_y(int i)
        {
            return ct_y[i];
        }
        std::vector<PbcElements>& get_ct_y()
        {
            return ct_y;
        }
        PbcElements& get_ct_prime()
        {
            return ct_prime;
        }
};


class RABE_XNM: public PbcScheme{
    private:
        element_t d1,d2,d3;
        element_t r1,r2;
        element_t b1r1a1,b1r1a2,b2r2a1,b2r2a2,r1r2a1,r1r2a2;
        element_t s1,s2;

        element_t tmp_G,tmp_G_2,tmp_G_3,tmp_G_4;
        element_t tmp_H;
        element_t tmp_GT,tmp_GT_2,tmp_GT_3,tmp_GT_4;
        element_t tmp_Zn,tmp_Zn_2;

        bool swap;

        void Pairing(element_t res, element_t a, element_t b);

        void initTmp();
    
    public:
        RABE_XNM(){}
        RABE_XNM(int curve, bool swap);

        void init(element_t _G1, element_t _G2, element_t _GT, element_t _Zn, bool swap);

        ~RABE_XNM();

        void Setup(RABE_XNM_mpk &mpk, RABE_XNM_msk &msk, std::vector<RABE_XNM_revokedPreson> &rl, Binary_tree_RABE &st, int n);

        void Hash(element_t res, std::string m);

        void KGen(RABE_XNM_skid &skid, Binary_tree_RABE &st, RABE_XNM_mpk &mpk, RABE_XNM_msk &msk, std::vector<std::string> &attr_list, element_t id, time_t re_time);

        std::vector<Binary_tree_RABE_node *> KUNodes(Binary_tree_RABE &st, std::vector<RABE_XNM_revokedPreson> &rl, time_t t);

        void KUpt(RABE_XNM_kut &kut, RABE_XNM_mpk &mpk, Binary_tree_RABE &st, std::vector<RABE_XNM_revokedPreson> &rl, time_t t);

        void DKGen(RABE_XNM_dkidt &dkidt, RABE_XNM_mpk &mpk, RABE_XNM_skid &skid, RABE_XNM_kut &kut);

        void Enc(RABE_XNM_ciphertext &ciphertext, RABE_XNM_mpk &mpk, element_t msg, Element_t_matrix *MSP, time_t t, element_t s1, element_t s2);

        void Dec(element_t res, RABE_XNM_mpk &mpk, RABE_XNM_ciphertext &ciphertext, RABE_XNM_dkidt &dkidt, Element_t_matrix *MSP);

        void Rev(std::vector<RABE_XNM_revokedPreson> &rl, element_t id, time_t t);

        enum {
            _,
            H1,
            H2,
            T1,
            T2
        };

        enum {
            h,
            g,
            a1,
            a2,
            b1,
            b2,
            g_pow_d1,
            g_pow_d2,
            g_pow_d3
        };

        enum {
            sk0_1,
            sk0_2,
            sk0_3,
            sk0_4
        };

        enum {
            sk_1,
            sk_2,
            sk_3,
            sk_4
        };

        enum {
            sk_theta
        };

        enum {
            ku_theta_1,
            ku_theta_2
        };

        enum {
            ct0_1,
            ct0_2,
            ct0_3,
            ct0_4
        };

        enum {
            ct_prime
        };

        enum {
            ct_1,
            ct_2,
            ct_3
        };
};;

#endif  // CHAMELIB_RABE_XNM_H