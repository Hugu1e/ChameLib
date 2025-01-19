#ifndef DPCH_MXN_2022_H
#define DPCH_MXN_2022_H

#include <stdexcept>  // 包含 std::invalid_argument
#include "utils/func.h"
#include <ABE/MA_ABE.h>
#include <SE/AES.h>
#include <signature/BLS.h>
#include <scheme/CH_ET_BC_CDK_2017.h>


class DPCH_MXN_2022 {
    protected:
        MA_ABE ma_abe;
        AES aes;
        BLS bls;
        CH_ET_BC_CDK_2017 ch_et;

        element_t *G1, *G2, *GT, *Zn;
        element_t tmp_G,tmp_G_2,tmp_G_3,tmp_G_4,tmp_H,tmp_H_2,tmp_H_3,tmp_GT,tmp_GT_2,tmp_GT_3,tmp_Zn,tmp_Zn_2,tmp_Zn_3;
    

    public:
        struct pp{
            MA_ABE::gpk gpkMA_ABE;
            CH_ET_BC_CDK_2017::pp ppCH;
            BLS::pp ppBLS;
            void Init(element_t *_G){
                gpkMA_ABE.Init(_G);
                ppBLS.Init(_G);
            }
        };
        struct pkDPCH{
            CH_ET_BC_CDK_2017::pk pkCH;
            BLS::pk pkBLS;
            void Init(element_t *_G){
                pkBLS.Init(_G);
            }
        };
        struct skDPCH{
            CH_ET_BC_CDK_2017::sk skCH;
            BLS::sk skBLS;
            void Init(element_t *_Zn){
                skBLS.Init(_Zn);
            }
        };
        struct skGid{
            CH_ET_BC_CDK_2017::sk skCH;
        };
        struct sigmaGid{
            BLS::signature signature;
            void Init(element_t *_H){
                signature.Init(_H);
            }
        };
        struct pkTheta{
            MA_ABE::pkTheta pk;
            void Init(element_t *_G, element_t *_GT){
                pk.Init(_G, _GT);
            }
        };
        struct skTheta{
            MA_ABE::skTheta sk;
            void Init(element_t *_Zn){
                sk.Init(_Zn);
            }
        };
        struct skGidA{
            MA_ABE::skgidA sk;
            void Init(element_t *_G){
                sk.Init(_G);
            }
        };
        struct h{
            CH_ET_BC_CDK_2017::h h;
        };
        struct r{
            CH_ET_BC_CDK_2017::r r;
        };
        struct c{
            mpz_t c_etd;
            MA_ABE::ciphertext c_abe;
            void Init(element_t *_G, element_t *_GT, int rows){
                mpz_init(c_etd);
                c_abe.Init(_G, _GT, rows);
            }
            ~c(){
                mpz_clear(c_etd);
            }
        };
        

        
        
        DPCH_MXN_2022(element_t *_G, element_t *_H, element_t *_Zn, element_t *_GT);

        void SetUp(pp *pp, pkDPCH *pkDPCH, skDPCH *skDPCH, int k);

        void ModSetUp(skGid *skGid, sigmaGid *sigmaGid, skDPCH *skDPCH, string gid);

        void AuthSetUp(pkTheta *pkTheta, skTheta *skTheta, pp *pp, string A);

        void ModKeyGen(skGidA *skGidA, pp *pp, pkDPCH *pkDPCH, string gid, sigmaGid *sigmaGid, skTheta *skTheta, string A);

        void Hash(h *h, r *r, c *c, pp *pp, pkDPCH *pkDPCH, string m, vector<DPCH_MXN_2022::pkTheta *> *pkThetas, string polocy);

        bool Check(pkDPCH *pkDPCH, string m, h *h, r *r);

        void Forge(r *r_p, pkDPCH *pkDPCH, skGid *skGid, vector<DPCH_MXN_2022::skGidA *> *skGidAs, c *c, string m, string m_p, h *h, r *r);

        bool Verify(pkDPCH *pkDPCH, string m_p, h *h, r *r_p);


        ~DPCH_MXN_2022();
};


#endif //DPCH_MXN_2022_H