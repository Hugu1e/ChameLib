#include "scheme/PBCH/DPCH_MXN_2022.h"
#include <openssl/rand.h>

DPCH_MXN_2022::DPCH_MXN_2022(int curve):PbcScheme(curve){
    element_init_G1(G1, pairing);
    element_init_G2(G2, pairing);
    element_init_GT(GT, pairing);
    element_init_Zr(Zn, pairing);

    ma_abe.init(G1, GT, Zn);
    bls.init(G1, G2, GT, Zn);

    element_init_same_as(tmp_G, G1);
    element_init_same_as(tmp_GT, GT);
    element_init_same_as(tmp_Zn, Zn);
    element_init_same_as(tmp_Zn_2, Zn);
}


/**
 * @brief Encode two messages into one element
 */
void DPCH_MXN_2022::Encode(element_t res, unsigned char *k, unsigned char *r, int k_len, int r_len){
    unsigned char tmp[element_length_in_bytes(res)];
    memset(tmp, 0, sizeof(tmp));
    tmp[1] = (unsigned char)k_len;
    memcpy(tmp + 2, k, k_len);
    tmp[element_length_in_bytes(res) / 2 + 1] = (unsigned char)r_len;
    memcpy(tmp + element_length_in_bytes(res) / 2 + 2, r, r_len);
    element_from_bytes(res, tmp);
}

/**
 * @brief Decode two messages from one element
 */
void DPCH_MXN_2022::Decode(unsigned char *k, unsigned char * r, element_t res){
    unsigned char tmp[element_length_in_bytes(res)];
    element_to_bytes(tmp, res);
    int l1 = tmp[1];
    if (l1 < 0 || l1 + 2 >= element_length_in_bytes(res)) {
        throw std::runtime_error("Decode Failed");
    }
    memcpy(k, tmp + 2, l1);
    int l2 = tmp[element_length_in_bytes(res) / 2 + 1];
    if (l2 < 0 || l2 + element_length_in_bytes(res) / 2 + 2 >= element_length_in_bytes(res)) {
        throw std::runtime_error("Decode Failed");
    }
    memcpy(r, tmp + element_length_in_bytes(res) / 2 + 2, l2);
}

/**
 * @brief 
 * 
 * @param  pp[out]        public parameters
 * @param  pkDPCH[out]    public key of DPCH
 * @param  skDPCH[out]    secret key of DPCH
 * @param  k[in]          key size
 * 
 */
void DPCH_MXN_2022::SetUp(DPCH_MXN_2022_pp &pp, DPCH_MXN_2022_pk &pkDPCH, DPCH_MXN_2022_sk &skDPCH, int k) {    
    element_random(tmp_G);

    ch_et.SetUp(pp.get_pp_CH(), k);
    ch_et.KeyGen(pkDPCH.get_pk_CH(), skDPCH.get_sk_CH(), pp.get_pp_CH());

    ma_abe.GlobalSetup(pp.get_gpk_MA_ABE(), tmp_G);

    bls.Setup(pp.get_pp_BLS(), tmp_G);
    bls.KeyGen(pkDPCH.get_pk_BLS(), skDPCH.get_sk_BLS(), pp.get_pp_BLS());
}

/**
 * @brief 
 * 
 * @param  skGid[out]     secret key of gid
 * @param  sigmaGid[out]  signature of gid
 * @param  skDPCH[in]     secret key of DPCH
 * @param  gid[in]        global id
 * 
 */
void DPCH_MXN_2022::ModSetUp(DPCH_MXN_2022_skGid &skGid, DPCH_MXN_2022_sigmaGid &sigmaGid, DPCH_MXN_2022_sk &skDPCH, std::string gid){
    skGid.get_sk_CH().set(CH_ET_BC_CDK_2017::d0, skDPCH.get_sk_CH()[CH_ET_BC_CDK_2017::d0]);
    bls.Sign(sigmaGid.get_signature(), skDPCH.get_sk_BLS(), gid);
}

/**
 * @brief 
 * 
 * @param  pkTheta[out]  public key of Theta
 * @param  skTheta[out]  secret key of Theta
 * @param  pp[in]        public parameters
 * @param  A[in]         attribute
 * 
 */
void DPCH_MXN_2022::AuthSetUp(DPCH_MXN_2022_pkTheta &pkTheta, DPCH_MXN_2022_skTheta &skTheta, DPCH_MXN_2022_pp &pp, std::string A){
    ma_abe.AuthSetup(pkTheta.get_pk(), skTheta.get_sk(), pp.get_gpk_MA_ABE(), A);
}

/**
 * @brief 
 * 
 * @param  skGidA[out]    secret key of gid and attribute
 * @param  pp[in]         public parameters
 * @param  pkDPCH[in]     public key of DPCH
 * @param  gid[in]        global id
 * @param  sigmaGid[in]   signature of gid
 * @param  skTheta[in]    secret key of Theta
 * @param  A[in]          attribute
 * 
 */
void DPCH_MXN_2022::ModKeyGen(DPCH_MXN_2022_skGidA &skGidA, DPCH_MXN_2022_pp &pp, DPCH_MXN_2022_pk &pkDPCH, std::string gid, DPCH_MXN_2022_sigmaGid &sigmaGid, DPCH_MXN_2022_skTheta &skTheta, std::string A){
    if(!(bls.Verify(pp.get_pp_BLS(), pkDPCH.get_pk_BLS(), gid, sigmaGid.get_signature()))){
        throw std::runtime_error("ModKeyGen(): Signature Verify failed!");
    }

    ma_abe.KeyGen(skGidA.get_sk(), pp.get_gpk_MA_ABE(), skTheta.get_sk(), gid, A);
}

void DPCH_MXN_2022::Ht(element_t res, element_t rt, std::string A){
    HASH::hash(res, rt, A);
}

void DPCH_MXN_2022::genEncMAABE(MA_ABE_ciphertext &ct, element_t pt, Element_t_matrix *MSP, std::string policy_str, std::vector<MA_ABE_pkTheta *> &pkThetas_ABE, DPCH_MXN_2022_pp &pp){
    int rows = MSP->row();
    int cols = MSP->col();
    // v, w, t_x
    Element_t_vector v = Element_t_vector(cols, Zn);
    Element_t_vector w = Element_t_vector(cols, Zn);
    Element_t_vector t_x = Element_t_vector(rows, Zn);
    for(int i = 0; i<cols; i++){
        if(i==0){
            // v[0] = z = Ht(rt, policy)
            this->Ht(v[0], tmp_Zn_2, policy_str);
            // w[0] = 0
            element_set0(w[0]);
        }else{
            std::string str_vi = policy_str + "1" + std::to_string(i);
            this->Ht(v[i], tmp_Zn_2, str_vi);

            std::string str_wi = policy_str + "2" + std::to_string(i);
            this->Ht(w[i], tmp_Zn_2, str_wi);
        }
    }
    for(int i = 0;i<rows;i++){
        std::string str_ti = policy_str + "0" + std::to_string(i);
        this->Ht(t_x[i], tmp_Zn_2, str_ti);
    }

    ma_abe.Encrypt(ct, pt, pp.get_gpk_MA_ABE(), pkThetas_ABE, MSP, policy_str, v, w, t_x);
}

/**
 * @brief 
 * 
 * @param  h[out]          hash value
 * @param  r[out]          random value
 * @param  m[in]           message
 * @param  pp[in]          public parameters
 * @param  pkDPCH[in]      public key of DPCH
 * @param  pkThetas[in]    public keys of Theta
 * @param  MSP[in]         
 * @param  policy_str[in]  
 * 
 */
void DPCH_MXN_2022::Hash(DPCH_MXN_2022_h &h, DPCH_MXN_2022_r &r, std::string m, DPCH_MXN_2022_pp &pp, DPCH_MXN_2022_pk &pkDPCH, std::vector<DPCH_MXN_2022_pkTheta *> &pkThetas, Element_t_matrix *MSP, std::string policy_str){
    CH_ET_BC_CDK_2017_etd etd;
    etd.init(1);

    ch_et.Hash(h.get_h(), r.get_r(), etd, pp.get_pp_CH(), pkDPCH.get_pk_CH(), m);

    int k_bits = 128;
    int k_bytes = k_bits / 8;
    unsigned char K[k_bytes];
    memset(K, 0, sizeof(K));
    unsigned char R[k_bytes];
    memset(R, 0, sizeof(R));
    aes.KGen(K, k_bits);
    RAND_bytes(R, k_bytes);

    int enc_len;
    unsigned char* enc = aes.Enc(&enc_len, K, etd[CH_ET_BC_CDK_2017::d1], k_bits);
    r.set_c_etd(enc, enc_len);

    std::vector<MA_ABE_pkTheta *> pkThetas_ABE(pkThetas.size());
    for(int i=0;i<pkThetas.size();i++){
        pkThetas_ABE[i] = &pkThetas[i]->get_pk();
    }
    // rt
    int rt_len = element_length_in_bytes(tmp_Zn_2);
    unsigned char tmp[rt_len];
    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, R, k_bytes);
    element_from_bytes(tmp_Zn_2, tmp);

    Encode(tmp_GT, K, R, k_bytes, k_bytes);

    genEncMAABE(r.get_c_abe(), tmp_GT, MSP, policy_str, pkThetas_ABE, pp);
}

/**
 * @brief 
 * 
 * @param  pkDPCH[in]  public key of DPCH
 * @param  m[in]       message
 * @param  h[in]       hash value
 * @param  r[in]       random value
 * 
 * @return 
 */
bool DPCH_MXN_2022::Check(DPCH_MXN_2022_pk &pkDPCH, std::string m, DPCH_MXN_2022_h &h, DPCH_MXN_2022_r &r){
    return ch_et.Check(h.get_h(), r.get_r(), pkDPCH.get_pk_CH(), m);
}

/**
 * @brief 
 * 
 * @param  r_p[out]        adapted random value
 * @param  m_p[in]         message to adapt
 * @param  h[in]           hash value
 * @param  r[in]           random value
 * @param  m[in]           original message
 * @param  pkDPCH[in]      public key of DPCH
 * @param  skGid[in]       secret key of gid
 * @param  skGidAs[in]     secret keys of gid and attributes
 * @param  pp[in]          public parameters
 * @param  pkThetas[in]    public keys of Theta
 * @param  MSP[in]         
 * @param  policy_str[in]  
 * 
 */
void DPCH_MXN_2022::Adapt(DPCH_MXN_2022_r &r_p, std::string m_p, DPCH_MXN_2022_h &h, DPCH_MXN_2022_r &r, std::string m,
    DPCH_MXN_2022_pk &pkDPCH, DPCH_MXN_2022_skGid &skGid, std::vector<DPCH_MXN_2022_skGidA *> &skGidAs,
    DPCH_MXN_2022_pp &pp, std::vector<DPCH_MXN_2022_pkTheta *> &pkThetas, Element_t_matrix *MSP, std::string policy_str){
    if(m == m_p){
        r_p.get_r().set(CH_ET_BC_CDK_2017::r0, r.get_r()[CH_ET_BC_CDK_2017::r0]);
        r_p.get_r().set(CH_ET_BC_CDK_2017::r1, r.get_r()[CH_ET_BC_CDK_2017::r1]);
        return;
    }

    std::vector<MA_ABE_skgidA *> skgidAs_ABE(skGidAs.size());
    for(int i=0;i<skGidAs.size();i++){
        skgidAs_ABE[i] = &skGidAs[i]->get_sk();
    }
    ma_abe.Decrypt(tmp_GT, skgidAs_ABE, r.get_c_abe(), MSP);

    int k_bits = 128;
    int k_bytes = k_bits / 8;
    // Decode(K) -> (k, r)
    unsigned char K[k_bytes];
    memset(K, 0, sizeof(K));
    unsigned char R[k_bytes];
    memset(R, 0, sizeof(R));
    Decode(K, R, tmp_GT);

    // c' ?= c
    std::vector<MA_ABE_pkTheta *> pkThetas_ABE(pkThetas.size());
    for(int i=0;i<pkThetas.size();i++){
        pkThetas_ABE[i] = &pkThetas[i]->get_pk();
    }
    MA_ABE_ciphertext tmp_c;
    // rt
    int rt_len = element_length_in_bytes(tmp_Zn_2);
    unsigned char tmp[rt_len];
    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, R, k_bytes);
    element_from_bytes(tmp_Zn_2, tmp);

    genEncMAABE(tmp_c, tmp_GT, MSP, policy_str, pkThetas_ABE, pp);
    if(!(tmp_c == r.get_c_abe())){
        throw std::runtime_error("DPCH_MXN_2022::Adapt(): c' != c");
    }

    mpz_t c_etd;
    mpz_init(c_etd);
    aes.Dec(c_etd, K, r.get_c_etd(), r.get_c_etd_len(), k_bits);

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


DPCH_MXN_2022::~DPCH_MXN_2022() {
    element_s *clear_list[] = {tmp_G, tmp_GT, tmp_Zn, tmp_Zn_2, G1, G2, GT, Zn};
    for(int i=0;i<sizeof(clear_list)/sizeof(clear_list[0]);i++){
        element_clear(clear_list[i]);
    }
}