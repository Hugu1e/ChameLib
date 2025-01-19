#ifndef CHAMELIB_MA_ABE_H
#define CHAMELIB_MA_ABE_H

#include <base/PbcElements.h>
#include <base/PbcScheme.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <ABE/Policy_resolution.h>
#include <ABE/Policy_generation.h>
#include <utils/Hash.h>

class MA_ABE_gpk: public PbcElements{};

class MA_ABE_pkTheta: public PbcElements{
    private:
        std::string A;
    public: 
        void setA(std::string A){
            this->A = A;
        }
        std::string getA(){
            return this->A;
        }
};

class MA_ABE_skTheta: public PbcElements{};

class MA_ABE_skgidA: public PbcElements{
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
};

class MA_ABE_ciphertext{
    private:
        std::string policy;
        PbcElements c0;
        std::vector<PbcElements> ci;
    public:
        void setPolicy(std::string policy){
            this->policy = policy;
        }
        std::string getPolicy(){
            return this->policy;
        }
        PbcElements *getC0(){
            return &c0;
        }
        PbcElements *getCi(int i){
            return &ci[i];
        }
        std::vector<PbcElements> *getCi(){
            return &ci;
        }
};


class MA_ABE: public PbcScheme{
    private:
        element_t z;

        std::unordered_map<unsigned long int, std::string> pai;  // π(i) -> attr

    public:
        MA_ABE(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void HGID(element_t res, bool bit, std::string gid);
        void Hu(element_t res, std::string u);

        void GlobalSetup(MA_ABE_gpk *gpk);
        void GlobalSetup(MA_ABE_gpk *gpk, element_t g);

        void AuthSetup(MA_ABE_pkTheta *pkTheta, MA_ABE_skTheta *skTheta, MA_ABE_gpk *gpk, std::string A);

        void KeyGen(MA_ABE_skgidA *skgidA, MA_ABE_gpk *gpk, MA_ABE_skTheta *skTheta, std::string gid, std::string A);        

        void Encrypt(MA_ABE_ciphertext *C, MA_ABE_gpk *gpk, std::vector<MA_ABE_pkTheta *> *pkThetas, std::string policy, element_t m);

        void Decrypt(element_t res, std::vector<MA_ABE_skgidA *> *skgidAs, MA_ABE_ciphertext *C);

        ~MA_ABE() override;
};


#endif  // CHAMELIB_MA_ABE_H