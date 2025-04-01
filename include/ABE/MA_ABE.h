#ifndef CHAMELIB_MA_ABE_H
#define CHAMELIB_MA_ABE_H

#include "../base/PbcElements.h"
#include "../base/PbcScheme.h"
#include "../base/Element_t_vector.h"
#include <vector>
#include <string>
#include <unordered_map>
#include "Policy_resolution.h"
#include "Policy_generation.h"
#include "../utils/Hash.h"

class MA_ABE_gpk: public PbcElements{
    public:
        MA_ABE_gpk(){
            init(2);
        }
};

class MA_ABE_pkTheta: public PbcElements{
    private:
        std::string A;
    public: 
        MA_ABE_pkTheta(){
            init(2);
        }
        void set_A(std::string A){
            this->A = A;
        }
        std::string get_A(){
            return this->A;
        }
        MA_ABE_pkTheta &operator=(const MA_ABE_pkTheta &other){
            if(this != &other){
                PbcElements::operator=(other);
                this->A = other.A;
            }
            return *this;
        }
};

class MA_ABE_skTheta: public PbcElements{
    public:
        MA_ABE_skTheta(){
            init(2);
        }
};

class MA_ABE_skgidA: public PbcElements{
    private:
        std::string gid;
        std::string A;
    public:
        MA_ABE_skgidA(){
            init(2);
        }
        void set_gid(std::string gid){
            this->gid = gid;
        }
        void set_A(std::string A){
            this->A = A;
        }
        std::string get_gid(){
            return this->gid;
        }
        std::string get_A(){
            return this->A;
        }
        MA_ABE_skgidA &operator=(const MA_ABE_skgidA &other){
            if(this != &other){
                PbcElements::operator=(other);
                this->gid = other.gid;
                this->A = other.A;
            }
            return *this;
        }
};

class MA_ABE_ciphertext{
    private:
        std::string policy;
        PbcElements ct_0;
        std::vector<PbcElements> ct_i;
    public:
        MA_ABE_ciphertext(){
            ct_0.init(1);
        }
        void setPolicy(std::string policy){
            this->policy = policy;
        }
        std::string get_policy(){
            return this->policy;
        }
        PbcElements& get_ct_0(){
            return ct_0;
        }
        PbcElements& get_ct_i(int i){
            return ct_i[i];
        }
        std::vector<PbcElements>& get_ct_i(){
            return ct_i;
        }
        MA_ABE_ciphertext &operator=(const MA_ABE_ciphertext &other){
            if(this != &other){
                this->policy = other.policy;
                this->ct_0 = other.ct_0;
                this->ct_i = other.ct_i;
            }
            return *this;
        }
        bool operator==(const MA_ABE_ciphertext &other){
            return this->policy == other.policy && this->ct_0 == other.ct_0 && this->ct_i == other.ct_i;
        }
};


class MA_ABE: public PbcScheme{
    private:
        element_t z;
        element_t tmp_G, tmp_G_2, tmp_G_3, tmp_G_4, tmp_GT, tmp_GT_2, tmp_GT_3, tmp_Zn;

        void HGID(element_t res, bool bit, std::string gid);
        void Hu(element_t res, std::string u);
        void Ht(element_t res, element_t rt, std::string A);

        void initTmp();

    public:
        MA_ABE(){}
        MA_ABE(int curve);

        void init(element_t _G1, element_t _GT, element_t _Zn);

        void GlobalSetup(MA_ABE_gpk &gpk);
        void GlobalSetup(MA_ABE_gpk &gpk, element_t _g);

        void AuthSetup(MA_ABE_pkTheta &pkTheta, MA_ABE_skTheta &skTheta, MA_ABE_gpk &gpk, std::string A);

        void KeyGen(MA_ABE_skgidA &skgidA, MA_ABE_gpk &gpk, MA_ABE_skTheta &skTheta, std::string gid, std::string A);        

        void Encrypt(MA_ABE_ciphertext &C, element_t m, MA_ABE_gpk &gpk, std::vector<MA_ABE_pkTheta *> &pkThetas, Element_t_matrix *MSP, std::string policy_str);
        void Encrypt(MA_ABE_ciphertext &C, element_t m, MA_ABE_gpk &gpk, std::vector<MA_ABE_pkTheta *> &pkThetas, Element_t_matrix *MSP, std::string policy_str, Element_t_vector &v, Element_t_vector &w, Element_t_vector &t_x);

        void Decrypt(element_t res, std::vector<MA_ABE_skgidA *> &skgidAs, MA_ABE_ciphertext &C, Element_t_matrix *MSP);

        ~MA_ABE();

        enum {
            g, egg
        };

        enum {
            aTheta, yTheta
        };

        enum{
            pkTheta_1, pkTheta_2
        };

        enum {
            skgidA_0, skgidA_1
        };

        enum{
            ct_1, ct_2, ct_3, ct_4
        };
};


#endif  // CHAMELIB_MA_ABE_H