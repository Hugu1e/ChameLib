#ifndef MAPCH_ZLW_2021_H
#define MAPCH_ZLW_2021_H

#include <stdexcept>
#include "utils/func.h"
#include <ABE/MA_ABE.h>
#include <scheme/CH_ET_BC_CDK_2017.h>


class MAPCH_ZLW_2021 {
    protected:
        MA_ABE ma_abe;
        CH_ET_BC_CDK_2017 ch_et;

        element_t *G1, *G2, *GT, *Zn;
        element_t tmp_G,tmp_G_2,tmp_G_3,tmp_G_4,tmp_H,tmp_H_2,tmp_H_3,tmp_GT,tmp_GT_2,tmp_GT_3,tmp_Zn,tmp_Zn_2,tmp_Zn_3;
    

    public:
        struct pp{
            CH_ET_BC_CDK_2017::pp ppCH;
        };
        struct mhk{
            CH_ET_BC_CDK_2017::pk hk;
            MA_ABE::pkTheta pkj;
            MA_ABE::gpk gpkABE;
            void Init(element_t *_G, element_t *_GT){
                pkj.Init(_G, _GT);
                gpkABE.Init(_G);
            }
        };
        struct mtk{
            CH_ET_BC_CDK_2017::sk tk;
            MA_ABE::skTheta skj;
            void Init(element_t *_Zn){
                skj.Init(_Zn);
            }
        };
        struct mski{
            CH_ET_BC_CDK_2017::sk tk;
            MA_ABE::skgidA KiGid;
            void Init(element_t *_G){
                KiGid.Init(_G);
            }
        };
        struct h{
            CH_ET_BC_CDK_2017::h h;
            CH_ET_BC_CDK_2017::r r;
            MA_ABE::ciphertext c;
            void Init(element_t *_G, element_t *_GT, int rows){
                c.Init(_G, _GT, rows);
            }
        };

        
        
        MAPCH_ZLW_2021(element_t *_G, element_t *_H, element_t *_Zn, element_t *_GT);

        void SetUp(pp *pp, vector<mhk *> *mhks, vector<mtk *> *mtks, int k, vector<string> *As);

        void KeyGen(vector<mski *> *mskis, vector<mtk *> *mtks, vector<mhk *> *mhks, vector<string> *As, string GID);

        void Hash(h *h, pp *pp, vector<mhk *> *mhks, string m, string policy);

        bool Check(vector<mhk *> *mhks, string m, h *h);

        void Forge(h *h_p,  vector<mhk *> *mhks, vector<mski *> *msks, string m, string m_p, h *h);

        bool Verify(vector<mhk *> *mhks, string m_p, h *h_p);

        ~MAPCH_ZLW_2021();
};


#endif //MAPCH_ZLW_2021_H