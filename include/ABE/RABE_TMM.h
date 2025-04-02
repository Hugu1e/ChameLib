#ifndef RABE_TMM_H
#define RABE_TMM_H

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
#include <string.h>

class RABE_TMM_mpk: public PbcElements{
    public:
        RABE_TMM_mpk(){
            this->init(6);
        }
};
class RABE_TMM_msk: public PbcElements{
    public:
        RABE_TMM_msk(){
            this->init(7);
        }
};
class RABE_TMM_skid{
    private:
        PbcElements sk0;
        std::vector<PbcElements> sk_y;
        std::vector<std::pair<Binary_tree_RABE_node*, PbcElements*>> sk_prime;
        std::unordered_map<std::string, unsigned long int> attr2id;
    public:
        RABE_TMM_skid(){
            sk0.init(3);
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
        std::vector<std::pair<Binary_tree_RABE_node*, PbcElements*>>& get_sk_prime()
        {
            return sk_prime;
        }
        std::pair<Binary_tree_RABE_node*, PbcElements*>& get_sk_prime(int i)
        {
            return sk_prime[i];
        }
        std::unordered_map<std::string, unsigned long int>& get_attr2id()
        {
            return attr2id;
        }
        ~RABE_TMM_skid(){
            for(int i = 0; i < sk_prime.size(); i++){
                delete sk_prime[i].second;
            }
        }
};
class RABE_TMM_revokedPreson: public PbcElements{
    private:
        time_t time;
    public:
        RABE_TMM_revokedPreson(): PbcElements(){}
        RABE_TMM_revokedPreson(const RABE_TMM_revokedPreson &other) : PbcElements(other), time(other.time){}
        void setTime(time_t time){
            this->time = time;
        }
        time_t getTime(){
            return this->time;
        }
};
class RABE_TMM_kut{
    private:
        time_t t;
        std::vector<std::pair<Binary_tree_RABE_node*, PbcElements*>> ku_theta;
    public:
        void setTime(time_t t){
            this->t = t;
        }
        time_t getTime(){
            return this->t;
        }
        std::vector<std::pair<Binary_tree_RABE_node*, PbcElements*>>& get_ku_theta()
        {
            return ku_theta;
        }
        std::pair<Binary_tree_RABE_node*, PbcElements*>& get_ku_theta(int i)
        {
            return ku_theta[i];
        }
        ~RABE_TMM_kut(){
            for(int i = 0; i < ku_theta.size(); i++){
                delete ku_theta[i].second;
            }
        }
};
class RABE_TMM_dkidt{
    private:
        time_t t;
        PbcElements sk0;
        std::vector<PbcElements> sk_y;
        PbcElements sk_prime;
        PbcElements skt1;  // sk(t,1)
        std::unordered_map<std::string, unsigned long int> attr2id;
    public:
        void setTime(time_t t){
            this->t = t;
        }
        time_t getTime(){
            return this->t;
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
        PbcElements& get_skt1()
        {
            return skt1;
        }
        std::unordered_map<std::string, unsigned long int>& get_attr2id()
        {
            return attr2id;
        }
};
class RABE_TMM_ciphertext{
    private:
        time_t t;
        PbcElements ct0;
        std::vector<PbcElements> ct_y;
        PbcElements ct_prime;
        unsigned char *ct_p = nullptr;
    public:
        RABE_TMM_ciphertext(){
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
        void set_ct_p(unsigned char *str, int size){
            ct_p = new unsigned char[size];
            memcpy(ct_p, str, size);
        }
        unsigned char* get_ct_p(){
            return ct_p;
        }
        ~RABE_TMM_ciphertext(){
            if(ct_p != nullptr) delete []ct_p;
        }
};

class RABE_TMM : public PbcScheme{
    private:
        element_t d1,d2,d3;
        element_t r1,r2;
        element_t b1r1a1,b1r1a2,b2r2a1,b2r2a2,r1r2a1,r1r2a2;
        element_t s1,s2;

        element_t tmp_G,tmp_G_2,tmp_G_3,tmp_G_4,tmp_H,tmp_GT,tmp_GT_2,tmp_GT_3,tmp_GT_4,tmp_Zn,tmp_Zn_2,tmp_Zn_3;

        bool swap;

        void Pairing(element_t res, element_t a, element_t b);

        void initTmp();
    
    public:
        RABE_TMM(){}
        RABE_TMM(int curve, bool swap);

        void init(element_t _G1, element_t _G2, element_t _GT, element_t _Zn, bool swap);

        ~RABE_TMM();

        void Setup(RABE_TMM_mpk &mpk, RABE_TMM_msk &msk, std::vector<RABE_TMM_revokedPreson> &rl, Binary_tree_RABE &st, int n);

        void Hash(element_t res, std::string m);
        void Hash(element_t res, element_t m);

        void KGen(RABE_TMM_skid &skid, Binary_tree_RABE &st, RABE_TMM_mpk &mpk, RABE_TMM_msk &msk, std::vector<std::string> &attr_list, element_t id, time_t re_time);

        std::vector<Binary_tree_RABE_node *> KUNodes(Binary_tree_RABE &st, std::vector<RABE_TMM_revokedPreson> &rl, time_t t);

        void KUpt(RABE_TMM_kut &kut, RABE_TMM_mpk &mpk, Binary_tree_RABE &st, std::vector<RABE_TMM_revokedPreson> &rl, time_t t);

        void DKGen(RABE_TMM_dkidt &dkidt, RABE_TMM_mpk &mpk, RABE_TMM_skid &skid, RABE_TMM_kut &kut);

        void Enc(RABE_TMM_ciphertext &ciphertext, RABE_TMM_mpk &mpk, element_t msg, Element_t_matrix *MSP, time_t t, element_t s1, element_t s2);

        void Dec(element_t res, RABE_TMM_mpk &mpk, RABE_TMM_ciphertext &ciphertext, RABE_TMM_dkidt &dkidt, Element_t_matrix *MSP);

        void Rev(std::vector<RABE_TMM_revokedPreson> &rl, element_t id, time_t t);

        enum {
            g,
            h,
            H1,
            H2,
            T1,
            T2
        };

        enum {
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
            sk0_3
        };

        enum {
            sk_1,
            sk_2,
            sk_3
        };

        enum {
            ct0_1,
            ct0_2,
            ct0_3,
            ct0_4
        };

        enum {
            ct_1,
            ct_2,
            ct_3
        };

        enum {
            ct_prime
        };

        enum {
            ku_theta_1,
            ku_theta_2
        };

        enum {
            skt1
        };
};;

#endif  // RABE_TMM_H