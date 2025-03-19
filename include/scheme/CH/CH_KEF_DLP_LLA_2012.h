#ifndef CH_KEF_DLP_LLA_2012_H
#define CH_KEF_DLP_LLA_2012_H

#include <stdexcept>
#include "../../base/PbcElements.h"
#include "../../base/PbcScheme.h"
#include "../../utils/Hash.h"
#include <map>

class CH_KEF_DLP_LLA_2012_pp : public PbcElements{
    public:
        CH_KEF_DLP_LLA_2012_pp(){
            init(1);
        }
};
class CH_KEF_DLP_LLA_2012_pk : public PbcElements{
    public:
        CH_KEF_DLP_LLA_2012_pk(){
            init(1);
        }
        std::string toString(){
            int length = element_length_in_bytes((*this)[0]);
            unsigned char bytes[length];
            element_to_bytes(bytes, (*this)[0]);
            std::string str = "";
            for(int i=0; i<length; i++){
                str += bytes[i];
            }
            return str;
        }
};
class CH_KEF_DLP_LLA_2012_sk : public PbcElements{
    public:
        CH_KEF_DLP_LLA_2012_sk(){
            init(3);
        }
};
class CH_KEF_DLP_LLA_2012_label : public PbcElements{
    public:
        CH_KEF_DLP_LLA_2012_label(){
            init(2);
        }
};
class CH_KEF_DLP_LLA_2012_labelGen : public PbcElements{
    public:
        CH_KEF_DLP_LLA_2012_labelGen(){
            init(2);
        }
};

class CH_KEF_DLP_LLA_2012_labelManager{
    private:
        std::map<std::string, CH_KEF_DLP_LLA_2012_labelGen *> Dict;
    public:
        void add(CH_KEF_DLP_LLA_2012_pk &pk, CH_KEF_DLP_LLA_2012_labelGen *lg){
            Dict[pk.toString()] = lg;
        }
        CH_KEF_DLP_LLA_2012_labelGen *get_lg(std::string pk){
            return Dict[pk];
        }
        ~CH_KEF_DLP_LLA_2012_labelManager(){
            for(auto it = Dict.begin(); it != Dict.end(); it++){
                delete it->second;
            }
        }
};

class CH_KEF_DLP_LLA_2012: public PbcScheme{
    private:
        element_t tmp_G, tmp_G_2, tmp_Zn, tmp_Zn_2;

        void H1(element_t res, element_t m1, element_t m2, element_t m3);
        void H2(element_t res, element_t m);

    public:
        CH_KEF_DLP_LLA_2012(int curve, int group);

        void getLabel(CH_KEF_DLP_LLA_2012_label &label, CH_KEF_DLP_LLA_2012_labelManager &lm, CH_KEF_DLP_LLA_2012_pk &pk);

        void SetUp(CH_KEF_DLP_LLA_2012_pp &pp);

        void KeyGen(CH_KEF_DLP_LLA_2012_sk &sk, CH_KEF_DLP_LLA_2012_pk &pk, CH_KEF_DLP_LLA_2012_labelManager &lm, CH_KEF_DLP_LLA_2012_pp &pp);

        void Hash(element_t h, element_t r, CH_KEF_DLP_LLA_2012_label &label, CH_KEF_DLP_LLA_2012_pk &pk, element_t m, CH_KEF_DLP_LLA_2012_labelManager &lm, CH_KEF_DLP_LLA_2012_pp &pp);

        bool Check(element_t m, element_t r, CH_KEF_DLP_LLA_2012_pk &pk, CH_KEF_DLP_LLA_2012_label &label, element_t h, CH_KEF_DLP_LLA_2012_pp &pp);

        void UForge(element_t r_p, CH_KEF_DLP_LLA_2012_sk &sk, CH_KEF_DLP_LLA_2012_pk &pk, CH_KEF_DLP_LLA_2012_label &label, element_t h, element_t m, element_t m_p, element_t r, CH_KEF_DLP_LLA_2012_pp &pp);
        void IForge(element_t r_pp, CH_KEF_DLP_LLA_2012_label &label, element_t m, element_t m_p, element_t r, element_t r_p, element_t m_pp);

        bool Verify(element_t m_p, element_t r_p, CH_KEF_DLP_LLA_2012_pk &pk, CH_KEF_DLP_LLA_2012_label &label, element_t h, CH_KEF_DLP_LLA_2012_pp &pp);

        ~CH_KEF_DLP_LLA_2012();

        enum{
            g
        };

        enum{
            y_1, omega_1
        };

        enum {
            alpha, x1, x2
        };

        enum {
            y2
        };

        enum {
            L, R
        };
};


#endif //CH_KEF_DLP_LLA_2012_H