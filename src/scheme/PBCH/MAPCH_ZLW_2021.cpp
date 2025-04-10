#include "scheme/PBCH/MAPCH_ZLW_2021.h"

MAPCH_ZLW_2021::MAPCH_ZLW_2021(int curve): PbcScheme(curve){
    element_init_G1(G1, pairing);
    element_init_GT(GT, pairing);
    element_init_Zr(Zn, pairing);

    ma_abe.init(G1, GT, Zn, true);
    
    element_init_same_as(tmp_G, G1);
    element_init_same_as(tmp_GT, GT);
    element_init_same_as(tmp_Zn, Zn);
    element_init_same_as(tmp_Zn_2, Zn);
}

/**
 * @brief 
 * 
 * @param  pkCH[out]  public key of CH
 * @param  skCH[out]  secret key of CH
 * @param  pp[out]    public parameters
 * @param  k[in]      security parameter
 * 
 */
void MAPCH_ZLW_2021::GlobalSetup(CH_ET_BC_CDK_2017_pk &pkCH, CH_ET_BC_CDK_2017_sk &skCH, MAPCH_ZLW_2021_pp &pp, int k){
    ch_et.SetUp(pp.get_pp_CH(), k);
    ch_et.KeyGen(pkCH, skCH, pp.get_pp_CH());

    element_random(tmp_G);
    ma_abe.GlobalSetup(pp.get_gpk_MA_ABE(), tmp_G);
}

/**
 * @brief 
 * 
 * @param  mhk[out]   master public key
 * @param  mtk[out]   master secret key
 * @param  A[in]      attribute of the authority
 * @param  pkCH[in]   public key of CH
 * @param  skCH[in]   secret key of CH
 * @param  pp[in]     public parameters
 * 
 */
void MAPCH_ZLW_2021::AuthSetUp(MAPCH_ZLW_2021_mhk &mhk, MAPCH_ZLW_2021_mtk &mtk, std::string A, CH_ET_BC_CDK_2017_pk &pkCH, CH_ET_BC_CDK_2017_sk &skCH, MAPCH_ZLW_2021_pp &pp){
    mhk.get_hk().set(pkCH);
    mhk.get_gpk_ABE().set(pp.get_gpk_MA_ABE());
    mtk.get_tk().set(skCH);

    ma_abe.AuthSetup(mhk.get_pkj(), mtk.get_skj(), mhk.get_gpk_ABE(), A);
}

/**
 * @brief 
 * 
 * @param  msk[out]  secret key of gid and attribute
 * @param  mtk[in]   master secret key
 * @param  mhk[in]   master public key
 * @param  A[in]     attributes
 * @param  GID[in]   global identifier
 * 
 */
void MAPCH_ZLW_2021::KeyGen(MAPCH_ZLW_2021_mski &msk, MAPCH_ZLW_2021_mtk &mtk, MAPCH_ZLW_2021_mhk &mhk, std::string A, std::string GID){
    msk.get_tk().set(mtk.get_tk());

    ma_abe.KeyGen(msk.get_KiGid(), mhk.get_gpk_ABE(), mtk.get_skj(), GID, A);
}

/**
 * @brief 
 * 
 * @param  h[out]      hash value
 * @param  r[out]      random value
 * @param  m[in]       message
 * @param  pp[in]     
 * @param  mhks[in]   
 * @param  MSP[in]    
 * @param  policy[in]  
 * 
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
 * @brief 
 * 
 * @param  h[in]     
 * @param  r[in]     
 * @param  m[in]      
 * @param  mhks[in]  
 * 
 * @return 
 */
bool MAPCH_ZLW_2021::Check(MAPCH_ZLW_2021_h &h, MAPCH_ZLW_2021_r &r, std::string m, std::vector<MAPCH_ZLW_2021_mhk *> &mhks){
    return ch_et.Check(h.get_h(), r.get_r(), mhks.at(0)->get_hk(), m);
}

/**
 * @brief 
 * 
 * @param  r_p[out]   
 * @param  m_p[in]    
 * @param  h[in]     
 * @param  r[in]     
 * @param  m[in]      
 * @param  mhks[in]  
 * @param  msks[in]  
 * @param  MSP[in]   
 * 
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
 * @brief 
 * 
 * @param  h[in]     
 * @param  r[in]     
 * @param  m[in]      
 * @param  mhks[in]  
 * 
 * @return 
 */
bool MAPCH_ZLW_2021::Verify(MAPCH_ZLW_2021_h &h, MAPCH_ZLW_2021_r &r, std::string m, std::vector<MAPCH_ZLW_2021_mhk *> &mhks){
    return Check(h, r, m, mhks);
}

Element_t_matrix* MAPCH_ZLW_2021::ComputeMSP(const std::string &policy_str){
    return ma_abe.ComputeMSP(policy_str);
}

MAPCH_ZLW_2021::~MAPCH_ZLW_2021() {
    element_s *clear_list[] = {tmp_G, tmp_GT, tmp_Zn, tmp_Zn_2};
    for(int i=0;i<sizeof(clear_list)/sizeof(clear_list[0]);i++){
        element_clear(clear_list[i]);
    }
}