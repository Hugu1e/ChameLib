#ifndef CH_ET_BC_CDK_2017_H
#define CH_ET_BC_CDK_2017_H

#include <gmp.h>
#include <utils/Hash.h>
#include <AE/RSA.h>
#include <base/GmpElements_copy.h>
#include <stdexcept>

class CH_ET_BC_CDK_2017_pp{
    private:
        int k;
    public:
        void setK(int k){
            this->k = k;
        }
        int getK(){
            return this->k;
        }
};

class CH_ET_BC_CDK_2017_pk: public GmpElements_copy{};

class CH_ET_BC_CDK_2017_sk: public GmpElements_copy{};

class CH_ET_BC_CDK_2017_h: public GmpElements_copy{};

class CH_ET_BC_CDK_2017_r: public GmpElements_copy{};

class CH_ET_BC_CDK_2017_etd: public GmpElements_copy{};

class CH_ET_BC_CDK_2017{
    private:
        AE_RSA rsa;

        void H(mpz_t res, std::string m, mpz_t n);

    public:
        CH_ET_BC_CDK_2017();

        void SetUp(CH_ET_BC_CDK_2017_pp &pp, CH_ET_BC_CDK_2017_pk &pk, CH_ET_BC_CDK_2017_sk &sk, CH_ET_BC_CDK_2017_h &h, CH_ET_BC_CDK_2017_r &r, CH_ET_BC_CDK_2017_r &r_p, CH_ET_BC_CDK_2017_etd &etd, int k);

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