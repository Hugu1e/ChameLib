#ifndef CHAMELIB_MA_ABE_H
#define CHAMELIB_MA_ABE_H

#include <base/PbcElements_copy.h>
#include <base/PbcScheme.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <ABE/Policy_resolution.h>
#include <ABE/Policy_generation.h>
#include <utils/Hash.h>

class MA_ABE_gpk: public PbcElements_copy{};

class MA_ABE_pkTheta: public PbcElements_copy{
    private:
        std::string A;
    public: 
        void setA(std::string A){
            this->A = A;
        }
        std::string getA(){
            return this->A;
        }
        MA_ABE_pkTheta &operator=(const MA_ABE_pkTheta &other){
            if(this != &other){
                PbcElements_copy::operator=(other);
                this->A = other.A;
            }
            return *this;
        }
};

class MA_ABE_skTheta: public PbcElements_copy{};

class MA_ABE_skgidA: public PbcElements_copy{
    private:
        std::string gid;
        std::string A;
    public:
        void setGid(std::string gid){
            this->gid = gid;
        }
        void setA(std::string A){
            this->A = A;
        }
        std::string getGid(){
            return this->gid;
        }
        std::string getA(){
            return this->A;
        }
        MA_ABE_skgidA &operator=(const MA_ABE_skgidA &other){
            if(this != &other){
                PbcElements_copy::operator=(other);
                this->gid = other.gid;
                this->A = other.A;
            }
            return *this;
        }
};

class MA_ABE_ciphertext{
    private:
        std::string policy;
        PbcElements_copy c0;
        std::vector<PbcElements_copy> ci;
    public:
        void setPolicy(std::string policy){
            this->policy = policy;
        }
        std::string getPolicy(){
            return this->policy;
        }
        PbcElements_copy& getC0(){
            return c0;
        }
        PbcElements_copy& getCi(int i){
            return ci[i];
        }
        std::vector<PbcElements_copy>& getCi(){
            return ci;
        }
        MA_ABE_ciphertext &operator=(const MA_ABE_ciphertext &other){
            if(this != &other){
                this->policy = other.policy;
                this->c0 = other.c0;
                this->ci = other.ci;
            }
            return *this;
        }
};


class MA_ABE: public PbcScheme{
    private:
        element_t z;

        std::unordered_map<unsigned long int, std::string> pai;  // π(i) -> attr

        void HGID(element_t res, bool bit, std::string gid);
        void Hu(element_t res, std::string u);

    public:
        MA_ABE(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void GlobalSetup(MA_ABE_gpk &gpk);
        void GlobalSetup(MA_ABE_gpk &gpk, element_t _g);

        void AuthSetup(MA_ABE_pkTheta &pkTheta, MA_ABE_skTheta &skTheta, MA_ABE_gpk &gpk, std::string A);

        void KeyGen(MA_ABE_skgidA &skgidA, MA_ABE_gpk &gpk, MA_ABE_skTheta &skTheta, std::string gid, std::string A);        

        void Encrypt(MA_ABE_ciphertext &C, MA_ABE_gpk &gpk, std::vector<MA_ABE_pkTheta *> &pkThetas, std::string policy, element_t m);

        void Decrypt(element_t res, std::vector<MA_ABE_skgidA *> &skgidAs, MA_ABE_ciphertext &C);

        ~MA_ABE() override;

        enum {
            g
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

        enum {
            c0
        };

        enum{
            ci_1, ci_2, ci_3, ci_4
        };
};


#endif  // CHAMELIB_MA_ABE_H