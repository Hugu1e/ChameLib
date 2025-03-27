#ifndef CH_ET_KOG_CDK_2017_H
#define CH_ET_KOG_CDK_2017_H

#include "../../base/PbcScheme.h"
#include "../../base/PbcElements.h"
#include <pbc/pbc.h>

#include "../../AE/RSA.h"
#include "../../utils/Hash.h"

class CH_ET_KOG_CDK_2017_pp: public PbcElements{
    public:
        CH_ET_KOG_CDK_2017_pp(){
            init(1);
        }
};

class CH_ET_KOG_CDK_2017_etd: public PbcElements{
    public:
        CH_ET_KOG_CDK_2017_etd(){
            init(1);
        }
};

class CH_ET_KOG_CDK_2017_pk{
    private:
        PbcElements ch_pk;
        RSA_pk enc_pk;
        PbcElements pai_pk;
    public:
        CH_ET_KOG_CDK_2017_pk(){
            ch_pk.init(1);
            pai_pk.init(2);
        }
        PbcElements &get_ch_pk(){
            return ch_pk;
        }
        RSA_pk &get_enc_pk(){
            return enc_pk;
        }
        PbcElements &get_pai_pk(){
            return pai_pk;
        }
};

class CH_ET_KOG_CDK_2017_sk{
    private:
        PbcElements ch_sk;
        RSA_sk enc_sk;
    public:
        CH_ET_KOG_CDK_2017_sk(){
            ch_sk.init(1);
        }
        PbcElements &get_ch_sk(){
            return ch_sk;
        }
        RSA_sk &get_enc_sk(){
            return enc_sk;
        }
};

class CH_ET_KOG_CDK_2017_h{
    private:
        PbcElements hash;  // b, h'
        PbcElements pai_t;
    public:
        CH_ET_KOG_CDK_2017_h(){
            hash.init(2);
            pai_t.init(2);
        }
        PbcElements &get_hash(){
            return hash;
        }
        PbcElements &get_pai_t(){
            return pai_t;
        }
};

class CH_ET_KOG_CDK_2017_r{
    private:
        PbcElements ch_r;  // p
        RSA_m enc_c;
        PbcElements pai_p;
    public:
        CH_ET_KOG_CDK_2017_r(){
            ch_r.init(1);
            pai_p.init(2);
        }
        PbcElements &get_ch_r(){
            return ch_r;
        }
        RSA_m &get_enc_c(){
            return enc_c;
        }
        PbcElements &get_pai_p(){
            return pai_p;
        }
};

class CH_ET_KOG_CDK_2017_NIZKPOK: public PbcScheme{
    private:
        element_t tmp_G, tmp_G_2, tmp_Zn, tmp_Zn_2;

    public:
        CH_ET_KOG_CDK_2017_NIZKPOK();

        void init(element_t _G, element_t _Zn);

        void proof(element_t z, element_t R, element_t pk, element_t sk, element_t g);

        bool verify(element_t z, element_t R, element_t pk, element_t g);

        ~CH_ET_KOG_CDK_2017_NIZKPOK();
};


class CH_ET_KOG_CDK_2017: public PbcScheme{
    private:
        AE_RSA enc;
        CH_ET_KOG_CDK_2017_NIZKPOK nizkpok;

        element_t tmp_G, tmp_G_2, tmp_G_3, tmp_Zn, tmp_Zn_2, tmp_Zn_3, tmp_Zn_4;

        void H(element_t res, element_t m);

    public:
        CH_ET_KOG_CDK_2017(int curve, int group);

        void SetUp(CH_ET_KOG_CDK_2017_pp &pp);

        void KeyGen(CH_ET_KOG_CDK_2017_sk &sk, CH_ET_KOG_CDK_2017_pk &pk, CH_ET_KOG_CDK_2017_pp &pp, short lamuda);

        void Hash(CH_ET_KOG_CDK_2017_h &hash, CH_ET_KOG_CDK_2017_r &r, CH_ET_KOG_CDK_2017_etd &etd, element_t m, CH_ET_KOG_CDK_2017_pk &pk, CH_ET_KOG_CDK_2017_pp &pp);

        bool Check(CH_ET_KOG_CDK_2017_h &hash, element_t m, CH_ET_KOG_CDK_2017_r &r, CH_ET_KOG_CDK_2017_pk &pk, CH_ET_KOG_CDK_2017_pp &pp);

        void Adapt(CH_ET_KOG_CDK_2017_r &r_p, element_t m_p, CH_ET_KOG_CDK_2017_h &hash, element_t m, CH_ET_KOG_CDK_2017_r &r, 
            CH_ET_KOG_CDK_2017_pk &pk, CH_ET_KOG_CDK_2017_pp &pp, CH_ET_KOG_CDK_2017_sk &sk, CH_ET_KOG_CDK_2017_etd &etd);

        bool Verify(CH_ET_KOG_CDK_2017_h &hash, element_t m_p, CH_ET_KOG_CDK_2017_r &r_p, CH_ET_KOG_CDK_2017_pk &pk, CH_ET_KOG_CDK_2017_pp &pp);


        ~CH_ET_KOG_CDK_2017();

        enum{
            g
        };  // pp

        enum{
            x
        };  // sk

        enum{
            h
        };  // pk

        enum{
            b, h_
        };  // hash

        enum{
            p
        };  // r

        enum{
            pai_z, pai_R
        };  // pai
};

#endif  // CH_ET_KOG_CDK_2017_H