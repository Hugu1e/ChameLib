#ifndef DPCH_MXN_2022_H
#define DPCH_MXN_2022_H

#include <utils/Hash.h>
#include <base/PbcScheme.h>
#include <base/GmpElements.h>

#include <ABE/MA_ABE.h>
#include <SE/AES.h>
#include <signature/BLS.h>
#include <scheme/CH/CH_ET_BC_CDK_2017.h>

class DPCH_MXN_2022_pp{
    private:
        MA_ABE_gpk gpkMA_ABE;
        CH_ET_BC_CDK_2017_pp ppCH;
        BLS_pp ppBLS;
    public:
        MA_ABE_gpk &getGpkMA_ABE(){
            return gpkMA_ABE;
        }
        CH_ET_BC_CDK_2017_pp &getPPCH(){
            return ppCH;
        }
        BLS_pp &getPPBLS(){
            return ppBLS;
        }
};

class DPCH_MXN_2022_pk{
    private:
        CH_ET_BC_CDK_2017_pk pkCH;
        BLS_pk pkBLS;
    public:
        CH_ET_BC_CDK_2017_pk &getPkCH(){
            return pkCH;
        }
        BLS_pk &getPkBLS(){
            return pkBLS;
        }
};

class DPCH_MXN_2022_sk{
    private:
        CH_ET_BC_CDK_2017_sk skCH;
        BLS_sk skBLS;
    public:
        CH_ET_BC_CDK_2017_sk &getSkCH(){
            return skCH;
        }
        BLS_sk &getSkBLS(){
            return skBLS;
        }
};

class DPCH_MXN_2022_skGid{
    private:
        CH_ET_BC_CDK_2017_sk skCH;
    public:
        CH_ET_BC_CDK_2017_sk &getSkCH(){
            return skCH;
        }
};
class DPCH_MXN_2022_sigmaGid {
    private:
        BLS_signature signature;
    public:
        BLS_signature &getSignature() {
            return signature;
        }
};

class DPCH_MXN_2022_pkTheta {
    private:
        MA_ABE_pkTheta pk;
    public:
        MA_ABE_pkTheta &getPk() {
            return pk;
        }
};

class DPCH_MXN_2022_skTheta {
    private:
        MA_ABE_skTheta sk;
    public:
        MA_ABE_skTheta &getSk() {
            return sk;
        }
};

class DPCH_MXN_2022_skGidA {
    private:
        MA_ABE_skgidA sk;
    public:
        MA_ABE_skgidA &getSk() {
            return sk;
        }
};

class DPCH_MXN_2022_h {
    private:
        CH_ET_BC_CDK_2017_h h;
    public:
        CH_ET_BC_CDK_2017_h &getH() {
            return h;
        }
};

class DPCH_MXN_2022_r {
    private:
        CH_ET_BC_CDK_2017_r r;
    public:
        CH_ET_BC_CDK_2017_r &getR() {
            return r;
        }
};

class DPCH_MXN_2022_c {
    private:
        GmpElements c_etd;
        MA_ABE_ciphertext c_abe;
    public:
        GmpElements &getC_etd() {
            return c_etd;
        }
        MA_ABE_ciphertext &getC_abe() {
            return c_abe;
        }
};



class DPCH_MXN_2022: public PbcScheme{
    protected:
        MA_ABE ma_abe;
        AES aes;
        BLS bls;
        CH_ET_BC_CDK_2017 ch_et;

    public:
        DPCH_MXN_2022(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void SetUp(DPCH_MXN_2022_pp &pp, DPCH_MXN_2022_pk &pkDPCH, DPCH_MXN_2022_sk &skDPCH, DPCH_MXN_2022_h &h, DPCH_MXN_2022_r &r, DPCH_MXN_2022_r &r_p, DPCH_MXN_2022_c &c, DPCH_MXN_2022_sigmaGid &sigmaGid, DPCH_MXN_2022_skGid &skGid,int k);

        void ModSetUp(DPCH_MXN_2022_skGid &skGid, DPCH_MXN_2022_sigmaGid &sigmaGid, DPCH_MXN_2022_sk &skDPCH, std::string gid);

        void AuthSetUp(DPCH_MXN_2022_pkTheta &pkTheta, DPCH_MXN_2022_skTheta &skTheta, DPCH_MXN_2022_pp &pp, std::string A);

        void ModKeyGen(DPCH_MXN_2022_skGidA &skGidA, DPCH_MXN_2022_pp &pp, DPCH_MXN_2022_pk &pkDPCH, std::string gid, DPCH_MXN_2022_sigmaGid &sigmaGid, DPCH_MXN_2022_skTheta &skTheta, std::string A);

        void Hash(DPCH_MXN_2022_h &h, DPCH_MXN_2022_r &r, DPCH_MXN_2022_c &c, DPCH_MXN_2022_pp &pp, DPCH_MXN_2022_pk &pkDPCH, std::string m, std::vector<DPCH_MXN_2022_pkTheta *> &pkThetas, std::string polocy);

        bool Check(DPCH_MXN_2022_pk &pkDPCH, std::string m, DPCH_MXN_2022_h &h, DPCH_MXN_2022_r &r);

        void Adapt(DPCH_MXN_2022_r &r_p, DPCH_MXN_2022_pk &pkDPCH, DPCH_MXN_2022_skGid &skGid, std::vector<DPCH_MXN_2022_skGidA *> &skGidAs, DPCH_MXN_2022_c &c, std::string m, std::string m_p, DPCH_MXN_2022_h &h, DPCH_MXN_2022_r &r);

        bool Verify(DPCH_MXN_2022_pk &pkDPCH, std::string m_p, DPCH_MXN_2022_h &h, DPCH_MXN_2022_r &r_p);

        ~DPCH_MXN_2022();

        enum{
            d1
        };
};


#endif //DPCH_MXN_2022_H