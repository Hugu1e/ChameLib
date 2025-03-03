#ifndef DPCH_MXN_2022_H
#define DPCH_MXN_2022_H

#include "../../utils/Hash.h"
#include "../../base/PbcScheme.h"
#include "../../base/GmpElements.h"

#include "../../ABE/MA_ABE.h"
#include "../../SE/AES.h"
#include "../../signature/BLS.h"
#include "../CH/CH_ET_BC_CDK_2017.h"

class DPCH_MXN_2022_pp{
    private:
        MA_ABE_gpk gpk_MA_ABE;
        CH_ET_BC_CDK_2017_pp pp_CH;
        BLS_pp pp_BLS;
    public:
        MA_ABE_gpk &get_gpk_MA_ABE(){
            return gpk_MA_ABE;
        }
        CH_ET_BC_CDK_2017_pp &get_pp_CH(){
            return pp_CH;
        }
        BLS_pp &get_pp_BLS(){
            return pp_BLS;
        }
};

class DPCH_MXN_2022_pk{
    private:
        CH_ET_BC_CDK_2017_pk pk_CH;
        BLS_pk pk_BLS;
    public:
        CH_ET_BC_CDK_2017_pk &get_pk_CH(){
            return pk_CH;
        }
        BLS_pk &get_pk_BLS(){
            return pk_BLS;
        }
};

class DPCH_MXN_2022_sk{
    private:
        CH_ET_BC_CDK_2017_sk sk_CH;
        BLS_sk sk_BLS;
    public:
        CH_ET_BC_CDK_2017_sk &get_sk_CH(){
            return sk_CH;
        }
        BLS_sk &get_sk_BLS(){
            return sk_BLS;
        }
};

class DPCH_MXN_2022_skGid{
    private:
        CH_ET_BC_CDK_2017_sk sk_CH;
    public:
        CH_ET_BC_CDK_2017_sk &get_sk_CH(){
            return sk_CH;
        }
};
class DPCH_MXN_2022_sigmaGid {
    private:
        BLS_signature signature;
    public:
        BLS_signature &get_signature() {
            return signature;
        }
};

class DPCH_MXN_2022_pkTheta {
    private:
        MA_ABE_pkTheta pk;
    public:
        MA_ABE_pkTheta &get_pk() {
            return pk;
        }
};

class DPCH_MXN_2022_skTheta {
    private:
        MA_ABE_skTheta sk;
    public:
        MA_ABE_skTheta &get_sk() {
            return sk;
        }
};

class DPCH_MXN_2022_skGidA {
    private:
        MA_ABE_skgidA sk;
    public:
        MA_ABE_skgidA &get_sk() {
            return sk;
        }
};

class DPCH_MXN_2022_h {
    private:
        CH_ET_BC_CDK_2017_h h;
    public:
        CH_ET_BC_CDK_2017_h &get_h() {
            return h;
        }
};

class DPCH_MXN_2022_r {
    private:
        CH_ET_BC_CDK_2017_r r;
        GmpElements c_etd;
        MA_ABE_ciphertext c_abe;
    public:
        CH_ET_BC_CDK_2017_r &get_r() {
            return r;
        }
        GmpElements &get_c_etd() {
            return c_etd;
        }
        MA_ABE_ciphertext &get_c_abe() {
            return c_abe;
        }
};



class DPCH_MXN_2022: public PbcScheme{
    protected:
        MA_ABE ma_abe;
        AES aes;
        BLS bls;
        CH_ET_BC_CDK_2017 ch_et;

        void Encode(element_t res, unsigned char *k, unsigned char *r, int k_len, int r_len);
        void Decode(unsigned char *k, unsigned char * r, element_t res);

        element_t tmp_G, tmp_GT, tmp_Zn, tmp_Zn_2;

    public:
        DPCH_MXN_2022(int curve);

        void SetUp(DPCH_MXN_2022_pp &pp, DPCH_MXN_2022_pk &pkDPCH, DPCH_MXN_2022_sk &skDPCH, DPCH_MXN_2022_h &h, DPCH_MXN_2022_r &r, DPCH_MXN_2022_r &r_p, DPCH_MXN_2022_sigmaGid &sigmaGid, DPCH_MXN_2022_skGid &skGid,int k);

        void ModSetUp(DPCH_MXN_2022_skGid &skGid, DPCH_MXN_2022_sigmaGid &sigmaGid, DPCH_MXN_2022_sk &skDPCH, std::string gid);

        void AuthSetUp(DPCH_MXN_2022_pkTheta &pkTheta, DPCH_MXN_2022_skTheta &skTheta, DPCH_MXN_2022_pp &pp, std::string A);

        void ModKeyGen(DPCH_MXN_2022_skGidA &skGidA, DPCH_MXN_2022_pp &pp, DPCH_MXN_2022_pk &pkDPCH, std::string gid, DPCH_MXN_2022_sigmaGid &sigmaGid, DPCH_MXN_2022_skTheta &skTheta, std::string A);

        void Hash(DPCH_MXN_2022_h &h, DPCH_MXN_2022_r &r, std::string m, DPCH_MXN_2022_pp &pp, DPCH_MXN_2022_pk &pkDPCH, std::vector<DPCH_MXN_2022_pkTheta *> &pkThetas, std::string polocy);

        bool Check(DPCH_MXN_2022_pk &pkDPCH, std::string m, DPCH_MXN_2022_h &h, DPCH_MXN_2022_r &r);

        void Adapt(DPCH_MXN_2022_r &r_p, std::string m_p, DPCH_MXN_2022_h &h, DPCH_MXN_2022_r &r, std::string m,
            DPCH_MXN_2022_pk &pkDPCH, DPCH_MXN_2022_skGid &skGid, std::vector<DPCH_MXN_2022_skGidA *> &skGidAs,  
            DPCH_MXN_2022_pp &pp, std::vector<DPCH_MXN_2022_pkTheta *> &pkThetas, std::string polocy);

        bool Verify(DPCH_MXN_2022_pk &pkDPCH, std::string m_p, DPCH_MXN_2022_h &h, DPCH_MXN_2022_r &r_p);

        ~DPCH_MXN_2022();
};


#endif //DPCH_MXN_2022_H