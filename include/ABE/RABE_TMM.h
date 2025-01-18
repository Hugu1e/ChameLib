#ifndef RABE_TMM_H
#define RABE_TMM_H

#include <base/PbcElements.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <ABE/Policy_resolution.h>
#include <ABE/Policy_generation.h>
#include <utils/Hash.h>
#include <utils/TimeUtils.h>
#include <base/Binary_tree_RABE.h>

class RABE_TMM_mpk: public PbcElements{};
class RABE_TMM_msk: public PbcElements{};
class RABE_TMM_skid{
    private:
        PbcElements sk0;
        std::vector<PbcElements> sk_y;
        std::vector<std::pair<Binary_tree_RABE_node*,PbcElements>> sk_prime;
    public:
        PbcElements *get_sk0()
        {
            return &sk0;
        }
        PbcElements *get_sk_y(int i)
        {
            return &sk_y[i];
        }
        std::vector<PbcElements> *get_sk_y()
        {
            return &sk_y;
        }
        std::vector<std::pair<Binary_tree_RABE_node*,PbcElements>> *get_sk_prime()
        {
            return &sk_prime;
        }
        std::pair<Binary_tree_RABE_node*,PbcElements>* get_sk_prime(int i)
        {
            return &sk_prime[i];
        }

};
class RABE_TMM_revokedPreson: public PbcElements{
    private:
        time_t time;
    public:
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
        std::vector<std::pair<Binary_tree_RABE_node*,PbcElements>> ku_theta;
    public:
        void setTime(time_t t){
            this->t = t;
        }
        time_t getTime(){
            return this->t;
        }
        std::vector<std::pair<Binary_tree_RABE_node*,PbcElements>> *get_ku_theta()
        {
            return &ku_theta;
        }
        std::pair<Binary_tree_RABE_node*,PbcElements>* get_ku_theta(int i)
        {
            return &ku_theta[i];
        }
};
class RABE_TMM_dkidt{
    private:
        time_t t;
        PbcElements sk0;
        std::vector<PbcElements> sk_y;
        PbcElements sk_prime;
        PbcElements skt1;  // sk(t,1)
    public:
        void setTime(time_t t){
            this->t = t;
        }
        time_t getTime(){
            return this->t;
        }
        PbcElements *get_sk0()
        {
            return &sk0;
        }
        PbcElements *get_sk_y(int i)
        {
            return &sk_y[i];
        }
        std::vector<PbcElements> *get_sk_y()
        {
            return &sk_y;
        }
        PbcElements *get_sk_prime()
        {
            return &sk_prime;
        }
        PbcElements *get_skt1()
        {
            return &skt1;
        }
};
class RABE_TMM_ciphertext{
    private:
        time_t t;
        PbcElements ct0;
        std::vector<PbcElements> ct_y;
        PbcElements ct_prime;
    public:
        void setTime(time_t t){
            this->t = t;
        }
        time_t getTime(){
            return this->t;
        }
        PbcElements *get_ct0()
        {
            return &ct0;
        }
        PbcElements *get_ct_y(int i)
        {
            return &ct_y[i];
        }
        std::vector<PbcElements> *get_ct_y()
        {
            return &ct_y;
        }
        PbcElements *get_ct_prime()
        {
            return &ct_prime;
        }
};

class RABE_TMM{
    protected:
        element_s *G1, *G2, *GT, *Zn;

        // temporary variable
        element_t tmp_G,tmp_G_2,tmp_G_3,tmp_G_4,tmp_H,tmp_H_2,tmp_H_3,tmp_GT,tmp_GT_2,tmp_GT_3, tmp_GT_4,tmp_Zn,tmp_Zn_2,tmp_Zn_3;
        element_t d1,d2,d3;
        element_t r1,r2;
        element_t b1r1a1,b1r1a2,b2r2a1,b2r2a2,r1r2a1,r1r2a2;
        element_t s1,s2;

        std::unordered_map<unsigned long int, std::string> pai;  // π(i) -> attr
        std::unordered_map<std::string, unsigned long int> attr_map;  // attr -> index of attr_list
        std::string policy_str;
    
    public:
        RABE_TMM(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        ~RABE_TMM();

        void Setup(RABE_TMM_mpk *mpk, RABE_TMM_msk *msk, std::vector<RABE_TMM_revokedPreson *> *rl, Binary_tree_RABE *&st, int n);

        void Hash(element_t res, std::string m);
        void Hash(element_t res, element_t m);

        void KGen(RABE_TMM_skid *skid, Binary_tree_RABE *st, RABE_TMM_mpk *mpk, RABE_TMM_msk *msk, element_t id, std::vector<std::string> *attr_list);

        std::vector<Binary_tree_RABE_node *> KUNodes(Binary_tree_RABE *st, std::vector<RABE_TMM_revokedPreson *> *rl, time_t t);

        void KUpt(RABE_TMM_kut *kut, RABE_TMM_mpk *mpk, Binary_tree_RABE *st, std::vector<RABE_TMM_revokedPreson *> *rl, time_t t);

        void DKGen(RABE_TMM_dkidt *dkidt, RABE_TMM_mpk *mpk, RABE_TMM_skid *skid, RABE_TMM_kut *kut);

        void Enc(RABE_TMM_ciphertext *ciphertext, RABE_TMM_mpk *mpk, element_t msg, std::string policy_str,time_t t, element_t s1, element_t s2);

        void Dec(element_t res, RABE_TMM_mpk *mpk, RABE_TMM_ciphertext *ciphertext, RABE_TMM_dkidt *dkidt);

        void Rev(std::vector<RABE_TMM_revokedPreson *> *rl, element_t id, time_t t);
};

#endif  // RABE_TMM_H