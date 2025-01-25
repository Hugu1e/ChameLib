#ifndef MAPCH_ZLW_2021_H
#define MAPCH_ZLW_2021_H

#include <utils/Hash.h>
#include <base/PbcScheme.h>
#include <base/GmpElements.h>
#include <utils/TypeConverter.h>

#include <ABE/MA_ABE.h>
#include <scheme/CH/CH_ET_BC_CDK_2017.h>

class MAPCH_ZLW_2021_pp{
    private:
        CH_ET_BC_CDK_2017_pp ppCH;
    public:
        CH_ET_BC_CDK_2017_pp *getPPCH(){
            return &this->ppCH;
        }
};

 class MAPCH_ZLW_2021_mhk{
    private:
        CH_ET_BC_CDK_2017_pk hk;
        MA_ABE_pkTheta pkj;
        MA_ABE_gpk gpkABE;
    public:
        CH_ET_BC_CDK_2017_pk *getHk(){
            return &this->hk;
        }
        MA_ABE_pkTheta *getPkj(){
            return &this->pkj;
        }
        MA_ABE_gpk *getGpkABE(){
            return &this->gpkABE;
        }
};
class MAPCH_ZLW_2021_mtk{
    private:
        CH_ET_BC_CDK_2017_sk tk;
        MA_ABE_skTheta skj;
    public:
        CH_ET_BC_CDK_2017_sk *getTk(){
            return &this->tk;
        }
        MA_ABE_skTheta *getSkj(){
            return &this->skj;
        }
};
class MAPCH_ZLW_2021_mski{
    private:
        CH_ET_BC_CDK_2017_sk tk;
        MA_ABE_skgidA KiGid;
    public:
        CH_ET_BC_CDK_2017_sk *getTk(){
            return &this->tk;
        }
        MA_ABE_skgidA *getKiGid(){
            return &this->KiGid;
        }
};
class MAPCH_ZLW_2021_h{
    private:
        CH_ET_BC_CDK_2017_h h;
        CH_ET_BC_CDK_2017_r r;
        MA_ABE_ciphertext c;
    public:
        CH_ET_BC_CDK_2017_h *getH(){
            return &this->h;
        }
        CH_ET_BC_CDK_2017_r *getR(){
            return &this->r;
        }
        MA_ABE_ciphertext *getC(){
            return &this->c;
        }
};

class MAPCH_ZLW_2021: public PbcScheme{
    protected:
        MA_ABE ma_abe;
        CH_ET_BC_CDK_2017 ch_et;


    public:
        MAPCH_ZLW_2021(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void SetUp(MAPCH_ZLW_2021_pp *pp, std::vector<MAPCH_ZLW_2021_mhk *> *mhks, std::vector<MAPCH_ZLW_2021_mtk *> *mtks, int k, std::vector<std::string> *As);

        void KeyGen(std::vector<MAPCH_ZLW_2021_mski *> *mskis, std::vector<MAPCH_ZLW_2021_mtk *> *mtks, std::vector<MAPCH_ZLW_2021_mhk *> *mhks, std::vector<std::string> *As, std::string GID);

        void Hash(MAPCH_ZLW_2021_h *h, MAPCH_ZLW_2021_pp *pp, std::vector<MAPCH_ZLW_2021_mhk *> *mhks, std::string m, std::string policy);

        bool Check(std::vector<MAPCH_ZLW_2021_mhk *> *mhks, std::string m, MAPCH_ZLW_2021_h *h);

        void Adapt(MAPCH_ZLW_2021_h *h_p,  std::vector<MAPCH_ZLW_2021_mhk *> *mhks, std::vector<MAPCH_ZLW_2021_mski *> *msks, std::string m, std::string m_p, MAPCH_ZLW_2021_h *h);

        bool Verify(std::vector<MAPCH_ZLW_2021_mhk *> *mhks, std::string m_p, MAPCH_ZLW_2021_h *h_p);

        ~MAPCH_ZLW_2021();
};


#endif //MAPCH_ZLW_2021_H