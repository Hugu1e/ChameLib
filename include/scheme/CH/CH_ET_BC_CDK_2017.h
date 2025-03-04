#ifndef CH_ET_BC_CDK_2017_H
#define CH_ET_BC_CDK_2017_H

#include <pbc/pbc.h>
#include "../../utils/Hash.h"
#include "../../AE/RSA.h"
#include "../../base/GmpElements.h"
#include <stdexcept>

class CH_ET_BC_CDK_2017_pp{
    private:
        int k;
    public:
        CH_ET_BC_CDK_2017_pp(){}
        CH_ET_BC_CDK_2017_pp(int k){
            this->k = k;
        }
        void setK(int k){
            this->k = k;
        }
        int getK(){
            return this->k;
        }
};

class CH_ET_BC_CDK_2017_pk: public GmpElements{
    public:
        CH_ET_BC_CDK_2017_pk(){
            this->init(2);
        }
};

class CH_ET_BC_CDK_2017_sk: public GmpElements{
    public:
        CH_ET_BC_CDK_2017_sk(){
            this->init(1);
        }
};

class CH_ET_BC_CDK_2017_h: public GmpElements{
    public:
        CH_ET_BC_CDK_2017_h(){
            this->init(4);
        }
};

class CH_ET_BC_CDK_2017_r: public GmpElements{
    public:
        CH_ET_BC_CDK_2017_r(){
            this->init(2);
        }
};

class CH_ET_BC_CDK_2017_etd: public GmpElements{
    public:
        CH_ET_BC_CDK_2017_etd(){
            this->init(1);
        }
};

class CH_ET_BC_CDK_2017{
    private:
        AE_RSA rsa;

        void H(mpz_t res, std::string m, mpz_t n);

    public:
        CH_ET_BC_CDK_2017();

        void SetUp(CH_ET_BC_CDK_2017_pp &pp, CH_ET_BC_CDK_2017_pk &pk, CH_ET_BC_CDK_2017_sk &sk, CH_ET_BC_CDK_2017_h &h, CH_ET_BC_CDK_2017_r &r, CH_ET_BC_CDK_2017_r &r_p, CH_ET_BC_CDK_2017_etd &etd, int k);
        void SetUp(CH_ET_BC_CDK_2017_pp &pp, CH_ET_BC_CDK_2017_pk &pk, CH_ET_BC_CDK_2017_sk &sk, CH_ET_BC_CDK_2017_h &h, CH_ET_BC_CDK_2017_r &r, CH_ET_BC_CDK_2017_r &r_p, int k);

        void KeyGen(CH_ET_BC_CDK_2017_pk &pk, CH_ET_BC_CDK_2017_sk &sk, CH_ET_BC_CDK_2017_pp &pp);

        void Hash(CH_ET_BC_CDK_2017_h &h, CH_ET_BC_CDK_2017_r &r, CH_ET_BC_CDK_2017_etd &etd, CH_ET_BC_CDK_2017_pp &pp, CH_ET_BC_CDK_2017_pk &pk, std::string m);

        bool Check(CH_ET_BC_CDK_2017_h &h, CH_ET_BC_CDK_2017_r &r, CH_ET_BC_CDK_2017_pk &pk, std::string m);

        void Adapt(CH_ET_BC_CDK_2017_r &r_p, CH_ET_BC_CDK_2017_sk &sk, CH_ET_BC_CDK_2017_etd &etd, CH_ET_BC_CDK_2017_pk &pk, CH_ET_BC_CDK_2017_h &h, CH_ET_BC_CDK_2017_r &r, std::string m, std::string m_p);
        
        bool Verify(CH_ET_BC_CDK_2017_h &h, CH_ET_BC_CDK_2017_r &r_p, CH_ET_BC_CDK_2017_pk &pk, std::string m_p);

        ~CH_ET_BC_CDK_2017();


        enum{
            n0,
            e0
        };

        enum{
            d0
        };

        enum{
            n1,
            e1,
            h0,
            h1
        };

        enum{
            r0,
            r1
        };

        enum{
            d1
        };
};




#endif  //CH_ET_BC_CDK_2017_H