#include <scheme/PBCH/DPCH_MXN_2022.h>

DPCH_MXN_2022::DPCH_MXN_2022(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn):PbcScheme(_G1, _G2, _GT, _Zn), ma_abe(_G1, _G2, _GT, _Zn), bls(_G1, _G2, _GT, _Zn) {}


/**
 * @brief Encode two messages into one element
 */
void DPCH_MXN_2022::Encode(element_t res, element_t m1, element_t m2){
    int len_1 = element_length_in_bytes(m1);
    int len_2 = element_length_in_bytes(m2);
    unsigned char *str_1 = (unsigned char *)malloc(len_1);
    element_to_bytes(str_1, m1);
    unsigned char *str_2 = (unsigned char *)malloc(len_2);
    element_to_bytes(str_2, m2);
    
    unsigned char *str_res = (unsigned char *)malloc(len_1 + len_2 + 1);
    str_res[0] = len_1;
    memcpy(str_res + 1, str_1, len_1);
    memcpy(str_res + 1 + len_1, str_2, len_2);
    element_from_bytes(res, str_res);

    free(str_1);
    free(str_2);
    free(str_res);
}
/**
 * @brief Decode two messages from one element
 */
void DPCH_MXN_2022::Decode(element_t res1, element_t res2, element_t m){
    int len_m = element_length_in_bytes(m);
    unsigned char *str_m = (unsigned char *)malloc(len_m);
    element_to_bytes(str_m, m);

    int len_1 = str_m[0];
    unsigned char *str_1 = (unsigned char *)malloc(len_1);
    memcpy(str_1, str_m + 1, len_1);
    element_from_bytes(res1, str_1);

    int len_2 = len_m - len_1 - 1;
    unsigned char *str_2 = (unsigned char *)malloc(len_2);
    memcpy(str_2, str_m + 1 + len_1, len_2);
    element_from_bytes(res2, str_2);

    free(str_m);
    free(str_1);
    free(str_2);
}

/**
 * @param pp: public parameters
 * @param pkDPCH: public key of DPCH
 * @param skDPCH: secret key of DPCH
 * @param k: key size
 */
void DPCH_MXN_2022::SetUp(DPCH_MXN_2022_pp &pp, DPCH_MXN_2022_pk &pkDPCH, DPCH_MXN_2022_sk &skDPCH, DPCH_MXN_2022_h &h, DPCH_MXN_2022_r &r, DPCH_MXN_2022_r &r_p, DPCH_MXN_2022_sigmaGid &sigmaGid, DPCH_MXN_2022_skGid &skGid, int k) {
    skGid.get_sk_CH().init(1);
    r.get_c_etd().init(1);
    
    element_random(tmp_G);

    ch_et.SetUp(pp.get_pp_CH(), pkDPCH.get_pk_CH(), skDPCH.get_sk_CH(), h.get_h(), r.get_r(), r_p.get_r(), k);
    ch_et.KeyGen(pkDPCH.get_pk_CH(), skDPCH.get_sk_CH(), pp.get_pp_CH());

    ma_abe.GlobalSetup(pp.get_gpk_MA_ABE(), tmp_G);

    bls.Setup(pp.get_pp_BLS(), pkDPCH.get_pk_BLS(), skDPCH.get_sk_BLS(), sigmaGid.get_signature(), tmp_G);
    bls.KeyGen(pkDPCH.get_pk_BLS(), skDPCH.get_sk_BLS(), pp.get_pp_BLS());
}

/**
 * @param skGid: secret key of gid
 * @param sigmaGid: signature of gid
 * @param skDPCH: secret key of DPCH
 * @param gid: global id
 */
void DPCH_MXN_2022::ModSetUp(DPCH_MXN_2022_skGid &skGid, DPCH_MXN_2022_sigmaGid &sigmaGid, DPCH_MXN_2022_sk &skDPCH, std::string gid){
    skGid.get_sk_CH().set(CH_ET_BC_CDK_2017::d0, skDPCH.get_sk_CH()[CH_ET_BC_CDK_2017::d0]);
    bls.Sign(sigmaGid.get_signature(), skDPCH.get_sk_BLS(), gid);
}

/**
 * @param pkTheta: public key of Theta
 * @param skTheta: secret key of Theta
 * @param pp: public parameters
 * @param A: attribute
 */
void DPCH_MXN_2022::AuthSetUp(DPCH_MXN_2022_pkTheta &pkTheta, DPCH_MXN_2022_skTheta &skTheta, DPCH_MXN_2022_pp &pp, std::string A){
    ma_abe.AuthSetup(pkTheta.get_pk(), skTheta.get_sk(), pp.get_gpk_MA_ABE(), A);
}

/**
 * @param skGidA: secret key of gid and attribute
 * @param pp: public parameters
 * @param pkDPCH: public key of DPCH
 * @param gid: global id
 * @param sigmaGid: signature of gid
 * @param skTheta: secret key of Theta
 * @param A: attribute
 */
void DPCH_MXN_2022::ModKeyGen(DPCH_MXN_2022_skGidA &skGidA, DPCH_MXN_2022_pp &pp, DPCH_MXN_2022_pk &pkDPCH, std::string gid, DPCH_MXN_2022_sigmaGid &sigmaGid, DPCH_MXN_2022_skTheta &skTheta, std::string A){
    if(!(bls.Verify(pp.get_pp_BLS(), pkDPCH.get_pk_BLS(), gid, sigmaGid.get_signature()))){
        throw std::runtime_error("ModKeyGen(): Signature Verify failed!");
    }

    ma_abe.KeyGen(skGidA.get_sk(), pp.get_gpk_MA_ABE(), skTheta.get_sk(), gid, A);
}

/**
 * @param h: hash value
 * @param r: random value
 * @param c: cyphertext
 * @param pp: public parameters
 * @param pkDPCH: public key of DPCH
 * @param m: message
 * @param pkThetas: public keys of Theta
 * @param polocy: policy
 */
void DPCH_MXN_2022::Hash(DPCH_MXN_2022_h &h, DPCH_MXN_2022_r &r, std::string m, DPCH_MXN_2022_pp &pp, DPCH_MXN_2022_pk &pkDPCH, std::vector<DPCH_MXN_2022_pkTheta *> &pkThetas, std::string polocy){
    CH_ET_BC_CDK_2017_etd etd;
    etd.init(1);

    ch_et.Hash(h.get_h(), r.get_r(), etd, pp.get_pp_CH(), pkDPCH.get_pk_CH(), m);

    aes.KGen(tmp_Zn);
    mpz_t c_etd;
    mpz_init(c_etd);
    aes.Enc(c_etd, tmp_Zn, etd[CH_ET_BC_CDK_2017::d1], 128);
    r.get_c_etd().set(0, c_etd);
    mpz_clear(c_etd);

    std::vector<MA_ABE_pkTheta *> pkThetas_ABE(pkThetas.size());
    for(int i=0;i<pkThetas.size();i++){
        pkThetas_ABE[i] = &pkThetas[i]->get_pk();
    }
    // rt
    element_random(tmp_Zn_2);

    Encode(tmp_GT, tmp_Zn, tmp_Zn_2);
    ma_abe.Encrypt(r.get_c_abe(), tmp_GT, tmp_Zn_2, pp.get_gpk_MA_ABE(), pkThetas_ABE, polocy);
}

/**
 * @param pkDPCH: public key of DPCH
 * @param m: message
 * @param h: hash value
 * @param r: random value
 * @return bool
 */
bool DPCH_MXN_2022::Check(DPCH_MXN_2022_pk &pkDPCH, std::string m, DPCH_MXN_2022_h &h, DPCH_MXN_2022_r &r){
    return ch_et.Check(h.get_h(), r.get_r(), pkDPCH.get_pk_CH(), m);
}

/**
 * @param r_p: random value
 * @param pkDPCH: public key of DPCH
 * @param skGid: secret key of gid
 * @param skGidAs: secret keys of gid and attributes
 * @param c: cyphertext
 * @param m: message
 * @param m_p: message
 * @param h: hash value
 * @param r: random value
 */
void DPCH_MXN_2022::Adapt(DPCH_MXN_2022_r &r_p, std::string m_p, DPCH_MXN_2022_h &h, DPCH_MXN_2022_r &r, std::string m,
    DPCH_MXN_2022_pk &pkDPCH, DPCH_MXN_2022_skGid &skGid, std::vector<DPCH_MXN_2022_skGidA *> &skGidAs,
    DPCH_MXN_2022_pp &pp, std::vector<DPCH_MXN_2022_pkTheta *> &pkThetas, std::string polocy){
    if(m == m_p){
        r_p.get_r().set(CH_ET_BC_CDK_2017::r0, r.get_r()[CH_ET_BC_CDK_2017::r0]);
        r_p.get_r().set(CH_ET_BC_CDK_2017::r1, r.get_r()[CH_ET_BC_CDK_2017::r1]);
        return;
    }

    std::vector<MA_ABE_skgidA *> skgidAs_ABE(skGidAs.size());
    for(int i=0;i<skGidAs.size();i++){
        skgidAs_ABE[i] = &skGidAs[i]->get_sk();
    }
    ma_abe.Decrypt(tmp_GT, skgidAs_ABE, r.get_c_abe());

    Decode(tmp_Zn, tmp_Zn_2, tmp_GT);

    // c' ?= c
    std::vector<MA_ABE_pkTheta *> pkThetas_ABE(pkThetas.size());
    for(int i=0;i<pkThetas.size();i++){
        pkThetas_ABE[i] = &pkThetas[i]->get_pk();
    }
    MA_ABE_ciphertext tmp_c;
    ma_abe.Encrypt(tmp_c, tmp_GT, tmp_Zn_2, pp.get_gpk_MA_ABE(), pkThetas_ABE, polocy);
    if(!(tmp_c == r.get_c_abe())){
        throw std::runtime_error("DPCH_MXN_2022::Adapt(): c' != c");
    }


    mpz_t c_etd;
    mpz_init(c_etd);
    aes.Dec(c_etd, tmp_Zn, r.get_c_etd()[0], 128);

    CH_ET_BC_CDK_2017_etd etd;
    etd.init(1);
    etd.set(CH_ET_BC_CDK_2017::d1, c_etd);
    mpz_clear(c_etd);

    ch_et.Adapt(r_p.get_r(), skGid.get_sk_CH(), etd, pkDPCH.get_pk_CH(), h.get_h(), r.get_r(), m, m_p);
}

/**
 * @param pkDPCH: public key of DPCH
 * @param m_p: message
 * @param h: hash value
 * @param r_p: random value
 * @return bool
 */
bool DPCH_MXN_2022::Verify(DPCH_MXN_2022_pk &pkDPCH, std::string m_p, DPCH_MXN_2022_h &h, DPCH_MXN_2022_r &r_p){
    return Check(pkDPCH, m_p, h, r_p);
}


DPCH_MXN_2022::~DPCH_MXN_2022() {}