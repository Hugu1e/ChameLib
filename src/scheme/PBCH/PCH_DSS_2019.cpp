#include <scheme/PBCH/PCH_DSS_2019.h>
#include <openssl/rand.h>
#include <string>

PCH_DSS_2019::PCH_DSS_2019(int curve, bool swap): PbcScheme(curve){
    this->swap = swap;

    if(swap){
        element_init_G2(G1, pairing);
        element_init_G1(G2, pairing);
    }else{
        element_init_G1(G1, pairing);
        element_init_G2(G2, pairing);
    }
    element_init_GT(GT, pairing);
    element_init_Zr(Zn, pairing);

    cp_abe.init(G1, G2, GT, Zn, swap);

    element_init_same_as(tmp_GT, GT);
    element_init_same_as(tmp_Zn, Zn);
    element_init_same_as(tmp_Zn_2, Zn);
    element_init_same_as(tmp_Zn_3, Zn);
    element_init_same_as(tmp_Zn_4, Zn);
}

/**
 * input : k
 * output: skPCH, pkPCH
 */
void PCH_DSS_2019::SetUp(PCH_DSS_2019_pp &pp, PCH_DSS_2019_pk &pk, PCH_DSS_2019_sk &sk, int k) {
    pp.get_ppCHET().setK(k);
    ch.KeyGen(pk.get_pkCHET(), sk.get_skCHET(), pp.get_ppCHET());
    cp_abe.Setup(sk.getMskABE(), pk.getMpkABE());
}

/**
 * input : skPCH, pkPCH, attr_list
 * output: sksPCH
 */
void PCH_DSS_2019::KeyGen(PCH_DSS_2019_sks &sks, PCH_DSS_2019_sk &sk, PCH_DSS_2019_pk &pk, std::vector<std::string> &attr_list) {
    this->cp_abe.KeyGen(sks.getSksABE(), sk.getMskABE(), pk.getMpkABE(), attr_list);
    sks.get_skCHET().copyFrom(sk.get_skCHET());
}

void PCH_DSS_2019::H1(mpz_t res, mpz_t m, mpz_t N1, mpz_t N2, mpz_t n){
    HASH::hash_n(res, m, N1, N2, n);
}
void PCH_DSS_2019::H2(mpz_t res, mpz_t m, mpz_t N1, mpz_t N2, mpz_t n){
    HASH::hash_n(res, m, N1, N2, n);
}
/**
 * (u1,u2)<-H4((r,A))
 */
void PCH_DSS_2019::H4(element_t u1, element_t u2, unsigned char *r, int r_len, std::string A){
    HASH::hash(u1, r, r_len);
    HASH::hash(u2, A);
}

/**
 * @brief Encode two messages into one element
 */
void PCH_DSS_2019::Encode(element_t res, unsigned char *k, unsigned char *r, int k_len, int r_len){
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
void PCH_DSS_2019::Decode(unsigned char *k, unsigned char * r, element_t res){
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
 * input : pkPCH, m, policy_str
 * output: h, r
 */
void PCH_DSS_2019::Hash(PCH_DSS_2019_h &h, PCH_DSS_2019_r &r, std::string m, Element_t_matrix *MSP, std::string policy_str, PCH_DSS_2019_pk &pk, PCH_DSS_2019_pp &pp) {
    CH_ET_BC_CDK_2017_etd etd;
    ch.Hash(h.get_h(), r.get_rCHET(), etd, pp.get_ppCHET(), pk.get_pkCHET(), m);

    int k_bits = 128;
    int k_bytes = k_bits / 8;
    unsigned char K[k_bytes];
    memset(K, 0, sizeof(K));
    unsigned char R[k_bytes];
    memset(R, 0, sizeof(R));
    aes.KGen(K, k_bits);
    RAND_bytes(R, k_bytes);
    // (u1,u2)<-H4((r,A))
    H4(tmp_Zn_3, tmp_Zn_4, R, k_bytes, policy_str);

    // Encode(k, r) -> K
    Encode(tmp_GT, K, R, k_bytes, k_bytes);

    cp_abe.Encrypt(h.getCt(), pk.getMpkABE(), tmp_GT, MSP, tmp_Zn_3, tmp_Zn_4);

    // ct_
    aes.Enc(h.getCt_()[0], K, etd[0], k_bits);
}

/**
 * input : pkPCH, m, h, r
 * output: bool
 */
bool PCH_DSS_2019::Check(PCH_DSS_2019_h &h, PCH_DSS_2019_r &r, std::string m, PCH_DSS_2019_pk &pk) {
    return ch.Check(h.get_h(), r.get_rCHET(), pk.get_pkCHET(), m);
}

/**
 * input : pkPCH, sksPCH, m, m', h, r
 * output: r'
 */
void PCH_DSS_2019::Adapt(PCH_DSS_2019_r &r_p, std::string m_p, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r, std::string m, PCH_DSS_2019_sks &sks, PCH_DSS_2019_pk &pk, Element_t_matrix *MSP, std::string policy_str) {
    cp_abe.Decrypt(tmp_GT, h.getCt(), MSP, pk.getMpkABE(),  sks.getSksABE());

    int k_bits = 128;
    int k_bytes = k_bits / 8;
    // Decode(K) -> (k, r)
    unsigned char K[k_bytes];
    memset(K, 0, sizeof(K));
    unsigned char R[k_bytes];
    memset(R, 0, sizeof(R));
    Decode(K, R, tmp_GT);

    // check ct ?= ct'
    // (u1,u2)<-H4((r,A))
    H4(tmp_Zn_3, tmp_Zn_4, R, k_bytes, policy_str);
    CP_ABE_ciphertext tmp_ct;
    cp_abe.Encrypt(tmp_ct, pk.getMpkABE(), tmp_GT, MSP, tmp_Zn_3, tmp_Zn_4);
    if(!(tmp_ct == h.getCt())){
        throw std::runtime_error("PCH_DSS_2019::Adapt(): ct != ct'");
    }

    CH_ET_BC_CDK_2017_etd etd;
    // DecSE(K, ct_) -> d2
    this->aes.Dec(etd[0], K, h.getCt_()[0], k_bits);

    // Hash Check
    if(!(Check(h, r, m, pk))){
        throw std::runtime_error("PCH_DSS_2019::Adapt(): Hash check failed");
    }

    ch.Adapt(r_p.get_rCHET(), sks.get_skCHET(), etd, pk.get_pkCHET(), h.get_h(), r.get_rCHET(), m, m_p);

    if(!(Check(h, r_p, m_p, pk))){
        throw std::runtime_error("PCH_DSS_2019::Adapt(): Adapt failed");
    }
}

/**
 * input : pkPCH, m', h, r'
 * output: bool
 */
bool PCH_DSS_2019::Verify(PCH_DSS_2019_h &h, PCH_DSS_2019_r &r_p, std::string m_p, PCH_DSS_2019_pk &pk) {
    return this->Check(h, r_p, m_p, pk);
}


PCH_DSS_2019::~PCH_DSS_2019() {
    element_s *clear_list[] = {tmp_GT, tmp_Zn, tmp_Zn_2, tmp_Zn_3, tmp_Zn_4, G1, G2, GT, Zn};
    for(int i=0;i<sizeof(clear_list)/sizeof(clear_list[0]);i++){
        element_clear(clear_list[i]);
    }
}