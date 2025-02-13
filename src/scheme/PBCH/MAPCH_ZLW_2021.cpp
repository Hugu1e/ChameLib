#include <scheme/PBCH/MAPCH_ZLW_2021.h>

MAPCH_ZLW_2021::MAPCH_ZLW_2021(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn): PbcScheme(_G1, _G2, _GT, _Zn), ma_abe(_G1, _G2, _GT, _Zn){}

/**
 * @param pp: public parameters
 * @param mhks: master public keys
 * @param mtks: master secret keys
 * @param k: security parameter
 * @param As: attributes
 */
void MAPCH_ZLW_2021::SetUp(MAPCH_ZLW_2021_pp &pp, std::vector<MAPCH_ZLW_2021_mhk *> &mhks, std::vector<MAPCH_ZLW_2021_mtk *> &mtks, int k, std::vector<std::string> &As, MAPCH_ZLW_2021_h &h, MAPCH_ZLW_2021_h &h_p){
    int numOfAuthority = As.size();

    CH_ET_BC_CDK_2017_pk pkCH;
    CH_ET_BC_CDK_2017_sk skCH;
    MA_ABE_gpk gpkABE;

    ch_et.SetUp(pp.getPPCH(), pkCH, skCH, h.getH(), h.getR(), h_p.getR(), k);
    ch_et.SetUp(pp.getPPCH(), pkCH, skCH, h_p.getH(), h.getR(), h_p.getR(), k);
    ch_et.KeyGen(pkCH, skCH, pp.getPPCH());

    element_random(tmp_G);
    ma_abe.GlobalSetup(gpkABE, tmp_G);

    for(int i=0;i<numOfAuthority;i++){
        MAPCH_ZLW_2021_mhk *mhk = new MAPCH_ZLW_2021_mhk();
        MAPCH_ZLW_2021_mtk *mtk = new MAPCH_ZLW_2021_mtk();
        
        mhk->getHk() = pkCH;
        mhk->getGpkABE() = gpkABE;
        mtk->getTk() = skCH;

        ma_abe.AuthSetup(mhk->getPkj(), mtk->getSkj(), mhk->getGpkABE(), As.at(i));

        mhks.push_back(mhk);
        mtks.push_back(mtk);
    }
}


/**
 * @param mskis: secret keys of gid and attributes
 * @param mtks: master secret keys
 * @param mhks: master public keys
 * @param As: attributes
 * @param GID: global identifier
 */
void MAPCH_ZLW_2021::KeyGen(std::vector<MAPCH_ZLW_2021_mski *> &mskis, std::vector<MAPCH_ZLW_2021_mtk *> &mtks, std::vector<MAPCH_ZLW_2021_mhk *> &mhks, std::vector<std::string> &As, std::string GID){
    int numOfAuthority = As.size();
    for(int i=0;i<numOfAuthority;i++){
        MAPCH_ZLW_2021_mski *mski = new MAPCH_ZLW_2021_mski();

        mski->getTk() = mtks.at(i)->getTk();

        ma_abe.KeyGen(mski->getKiGid(), mhks.at(i)->getGpkABE(), mtks.at(i)->getSkj(), GID, As.at(i));

        mskis.push_back(mski);
    }
}

/**
 * @param h: hash value
 * @param pp: public parameters
 * @param mhks: master public keys
 * @param m: message
 * @param polocy: policy
 */
void MAPCH_ZLW_2021::Hash(MAPCH_ZLW_2021_h &h, MAPCH_ZLW_2021_pp &pp, std::vector<MAPCH_ZLW_2021_mhk *> &mhks, std::string m, std::string policy){
    CH_ET_BC_CDK_2017_etd etd;
    etd.init(1);

    std::vector<MA_ABE_pkTheta *> pkThetas(mhks.size());
    for(int i=0;i<mhks.size();i++){
        MA_ABE_pkTheta *pkTheta = new MA_ABE_pkTheta();

        *pkTheta = mhks.at(i)->getPkj();

        pkThetas[i] = pkTheta;
    }

    ch_et.Hash(h.getH(), h.getR(), etd, pp.getPPCH(), mhks.at(0)->getHk(), m);

    TypeConverter::mpz_to_element(tmp_GT, etd[CH_ET_BC_CDK_2017::d1]);
    
    // TODO
    element_random(tmp_Zn);
    ma_abe.Encrypt(h.getC(), tmp_GT, tmp_Zn, mhks.at(0)->getGpkABE(), pkThetas, policy);
}

/**
 * @param mhks: master public keys
 * @param m: message
 * @param h: hash value
 * @return bool
 */
bool MAPCH_ZLW_2021::Check(std::vector<MAPCH_ZLW_2021_mhk *> &mhks, std::string m, MAPCH_ZLW_2021_h &h){
    return ch_et.Check(h.getH(), h.getR(), mhks.at(0)->getHk(), m);
}

/**
 * @param h_p: h with adapted random value
 * @param mhks: master public keys
 * @param msks: secret keys of gid and attributes
 * @param m: message
 * @param m_p: message
 * @param h: hash value
 */
void MAPCH_ZLW_2021::Adapt(MAPCH_ZLW_2021_h &h_p,  std::vector<MAPCH_ZLW_2021_mhk *> &mhks, std::vector<MAPCH_ZLW_2021_mski *> &msks, std::string m, std::string m_p, MAPCH_ZLW_2021_h &h){
    if(!Check(mhks, m, h)){
        throw std::runtime_error("Forge: Hash Check failed!");
    }
    std::vector<MA_ABE_skgidA *> skgidAs;
    for(int i=0;i<msks.size();i++){
        MA_ABE_skgidA *skgidA = new MA_ABE_skgidA();
        
        *skgidA = msks.at(i)->getKiGid();

        skgidAs.push_back(skgidA);
    }

    ma_abe.Decrypt(tmp_GT, skgidAs, h.getC());

    CH_ET_BC_CDK_2017_etd etd;
    etd.init(1);

    mpz_t d1;
    mpz_init(d1);
    TypeConverter::mpz_from_element(d1, tmp_GT);
    etd.set(CH_ET_BC_CDK_2017::d1, d1);
    mpz_clear(d1);

    ch_et.Adapt(h_p.getR(), msks.at(0)->getTk(), etd, mhks.at(0)->getHk(), h.getH(), h.getR(), m, m_p);

    h_p.getC() = h.getC();
    h_p.getH() = h.getH();
}

/**
 * @param pkDPCH: public key of DPCH
 * @param m_p: message
 * @param h: hash value
 * @param r_p: random value
 * @return bool
 */
bool MAPCH_ZLW_2021::Verify(std::vector<MAPCH_ZLW_2021_mhk *> &mhks, std::string m_p, MAPCH_ZLW_2021_h &h_p){
    return Check(mhks, m_p, h_p);
}


MAPCH_ZLW_2021::~MAPCH_ZLW_2021() {}