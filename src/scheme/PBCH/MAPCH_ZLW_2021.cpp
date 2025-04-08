#include "scheme/PBCH/MAPCH_ZLW_2021.h"

MAPCH_ZLW_2021::MAPCH_ZLW_2021(int curve): PbcScheme(curve){
    element_init_G1(G1, pairing);
    element_init_GT(GT, pairing);
    element_init_Zr(Zn, pairing);

    ma_abe.init(G1, GT, Zn);
    
    element_init_same_as(tmp_G, G1);
    element_init_same_as(tmp_GT, GT);
    element_init_same_as(tmp_Zn, Zn);
    element_init_same_as(tmp_Zn_2, Zn);
}

/**
 * @param pkCH: public key of CH
 * @param skCH: secret key of CH
 * @param gpkABE: global public key of ABE
 * @param pp: public parameters
 * @param mhks: master public keys
 * @param mtks: master secret keys
 * @param k: security parameter
 * @param As: attributes
 */
void MAPCH_ZLW_2021::GlobalSetup(CH_ET_BC_CDK_2017_pk &pkCH, CH_ET_BC_CDK_2017_sk &skCH, MA_ABE_gpk &gpkABE, MAPCH_ZLW_2021_pp &pp, int k){
    ch_et.SetUp(pp.get_pp_CH(), k);
    ch_et.KeyGen(pkCH, skCH, pp.get_pp_CH());

    element_random(tmp_G);
    ma_abe.GlobalSetup(gpkABE, tmp_G);
}

/**
 * @param mhk: master public key
 * @param mtk: master secret key
 * @param A: attribute of the authority
 * @param pkCH: public key of CH
 * @param skCH: secret key of CH
 * @param gpkABE: global public key of ABE
 * @param pp: public parameters
 */
void MAPCH_ZLW_2021::AuthSetUp(MAPCH_ZLW_2021_mhk &mhk, MAPCH_ZLW_2021_mtk &mtk, std::string A, CH_ET_BC_CDK_2017_pk &pkCH, CH_ET_BC_CDK_2017_sk &skCH, MA_ABE_gpk &gpkABE, MAPCH_ZLW_2021_pp &pp){
    mhk.get_hk() = pkCH;
    mhk.get_gpk_ABE() = gpkABE;
    mtk.get_tk() = skCH;

    ma_abe.AuthSetup(mhk.get_pkj(), mtk.get_skj(), mhk.get_gpk_ABE(), A);
}


/**
 * @param msk: secret key of gid and attribute
 * @param mtk: master secret key
 * @param mhk: master public key
 * @param A: attributes
 * @param GID: global identifier
 */
void MAPCH_ZLW_2021::KeyGen(MAPCH_ZLW_2021_mski &msk, MAPCH_ZLW_2021_mtk &mtk, MAPCH_ZLW_2021_mhk &mhk, std::string A, std::string GID){
    msk.get_tk() = mtk.get_tk();

    ma_abe.KeyGen(msk.get_KiGid(), mhk.get_gpk_ABE(), mtk.get_skj(), GID, A);
}

/**
 * @param h: hash value
 * @param pp: public parameters
 * @param mhks: master public keys
 * @param m: message
 * @param polocy: policy
 */
void MAPCH_ZLW_2021::Hash(MAPCH_ZLW_2021_h &h, MAPCH_ZLW_2021_r &r, std::string m, MAPCH_ZLW_2021_pp &pp, std::vector<MAPCH_ZLW_2021_mhk *> &mhks, Element_t_matrix *MSP, std::string policy){
    CH_ET_BC_CDK_2017_etd etd;
    etd.init(1);

    std::vector<MA_ABE_pkTheta *> pkThetas(mhks.size());
    for(int i=0;i<mhks.size();i++) pkThetas[i] = new MA_ABE_pkTheta(mhks.at(i)->get_pkj());

    ch_et.Hash(h.get_h(), r.get_r(), etd, pp.get_pp_CH(), mhks.at(0)->get_hk(), m);

    TypeConverter::mpz_to_element(tmp_GT, etd[CH_ET_BC_CDK_2017::d1]);
    
    ma_abe.Encrypt(r.get_c(), tmp_GT, mhks.at(0)->get_gpk_ABE(), pkThetas, MSP, policy);

    // free pkThetas
    for(int i=0;i<pkThetas.size();i++){
        delete pkThetas[i];
    }
}

/**
 * @param mhks: master public keys
 * @param m: message
 * @param h: hash value
 * @return bool
 */
bool MAPCH_ZLW_2021::Check(MAPCH_ZLW_2021_h &h, MAPCH_ZLW_2021_r &r, std::string m, std::vector<MAPCH_ZLW_2021_mhk *> &mhks){
    return ch_et.Check(h.get_h(), r.get_r(), mhks.at(0)->get_hk(), m);
}

/**
 * @param h_p: h with adapted random value
 * @param mhks: master public keys
 * @param msks: secret keys of gid and attributes
 * @param m: message
 * @param m_p: message
 * @param h: hash value
 */
void MAPCH_ZLW_2021::Adapt(MAPCH_ZLW_2021_r &r_p, std::string m_p, MAPCH_ZLW_2021_h &h, MAPCH_ZLW_2021_r &r, std::string m, std::vector<MAPCH_ZLW_2021_mhk *> &mhks, std::vector<MAPCH_ZLW_2021_mski *> &msks, Element_t_matrix *MSP){
    if(!Check(h, r, m, mhks)){
        throw std::runtime_error("Forge: Hash Check failed!");
    }
    std::vector<MA_ABE_skgidA *> skgidAs(msks.size());
    for(int i=0;i<msks.size();i++) skgidAs[i] = new MA_ABE_skgidA(msks.at(i)->get_KiGid());

    ma_abe.Decrypt(tmp_GT, skgidAs, r.get_c(), MSP);

    CH_ET_BC_CDK_2017_etd etd;
    etd.init(1);

    mpz_t d1;
    mpz_init(d1);
    TypeConverter::mpz_from_element(d1, tmp_GT);
    etd.set(CH_ET_BC_CDK_2017::d1, d1);
    mpz_clear(d1);

    ch_et.Adapt(r_p.get_r(), msks.at(0)->get_tk(), etd, mhks.at(0)->get_hk(), h.get_h(), r.get_r(), m, m_p);

    r_p.get_c() = r.get_c();

    // free skgidAs
    for(int i=0;i<skgidAs.size();i++) delete skgidAs[i];
}

/**
 * @param pkDPCH: public key of DPCH
 * @param m_p: message
 * @param h: hash value
 * @param r_p: random value
 * @return bool
 */
bool MAPCH_ZLW_2021::Verify(MAPCH_ZLW_2021_h &h, MAPCH_ZLW_2021_r &r, std::string m, std::vector<MAPCH_ZLW_2021_mhk *> &mhks){
    return Check(h, r, m, mhks);
}


MAPCH_ZLW_2021::~MAPCH_ZLW_2021() {
    element_s *clear_list[] = {tmp_G, tmp_GT, tmp_Zn, tmp_Zn_2, G1, GT, Zn};
    for(int i=0;i<sizeof(clear_list)/sizeof(clear_list[0]);i++){
        element_clear(clear_list[i]);
    }
}