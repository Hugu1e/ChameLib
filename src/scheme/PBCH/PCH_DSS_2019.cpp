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
void PCH_DSS_2019::SetUp(PCH_DSS_2019_pk &pkPCH, PCH_DSS_2019_sk &skPCH, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r, PCH_DSS_2019_r &r_p, int k) {
    skPCH.getSkCHET().init(1);
    pkPCH.getPkCHET().init(2);
    h.getH().init(3);
    h.getCt_().init(1);
    r.init(2);
    r_p.init(2);

    mpz_t _n, _e, _d;
    mpz_inits(_n, _e, _d, NULL);
    
    this->k = k;
    // e > N
    rsa.KeyGen(_n, _e, _d, k, 1);
    // set d1
    skPCH.getSkCHET().set(d1, _d);
    // set N1,e
    pkPCH.getPkCHET().set(N1, _n);
    pkPCH.getPkCHET().set(e, _e);

    this->cp_abe.Setup(skPCH.getMskABE(), pkPCH.getMpkABE());
}

/**
 * input : skPCH, pkPCH, attr_list
 * output: sksPCH
 */
void PCH_DSS_2019::KeyGen(PCH_DSS_2019_sk &skPCH, PCH_DSS_2019_pk &pkPCH, std::vector<std::string> &attr_list, PCH_DSS_2019_sks &sksPCH) {
    this->cp_abe.KeyGen(sksPCH.getSksABE(), skPCH.getMskABE(), pkPCH.getMpkABE(), attr_list);
    sksPCH.getSkCHET().init(1);
    sksPCH.getSkCHET().set(d1, skPCH.getSkCHET()[d1]);
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
void PCH_DSS_2019::Hash(PCH_DSS_2019_pk &pkPCH, mpz_t m, const std::string &policy_str, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r) {
    mpz_t tmp1, tmp2, kk, d2;
    mpz_inits(tmp1, tmp2, kk, d2, NULL);

    rsa.KeyGen_E(h.getH()[N2], d2, pkPCH.getPkCHET()[e], this->k);

    // r1 ∈ ZN1*
    RandomGenerator::RandomCoprimeN(r[r1], pkPCH.getPkCHET()[N1]);
    // r2 ∈ ZN2*
    RandomGenerator::RandomCoprimeN(r[r2], h.getH()[N2]);

    // h1 = H1(m,N1,N2)* r1^e mod N1
    H1(tmp1, m, pkPCH.getPkCHET()[N1], h.getH()[N2], pkPCH.getPkCHET()[N1]);
    // r1^e mod N1
    mpz_powm(tmp2, r[r1], pkPCH.getPkCHET()[e], pkPCH.getPkCHET()[N1]);
    mpz_mul(h.getH()[h1], tmp1, tmp2);
    mpz_mod(h.getH()[h1], h.getH()[h1], pkPCH.getPkCHET()[N1]);

    // h2 = H2(m,N1,N2)* r2^e mod N2
    H2(tmp1, m, pkPCH.getPkCHET()[N1], h.getH()[N2], h.getH()[N2]);
    // r2^e mod N2
    mpz_powm(tmp2, r[r2], pkPCH.getPkCHET()[e], h.getH()[N2]);
    mpz_mul(h.getH()[h2], tmp1, tmp2);
    mpz_mod(h.getH()[h2], h.getH()[h2], h.getH()[N2]);

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

    cp_abe.Encrypt(h.getCt(), pkPCH.getMpkABE(), tmp_GT, policy_str, tmp_Zn_3, tmp_Zn_4);

    // ct_
    aes.Enc(h.getCt_()[ct_], K, d2, k_bits);

    mpz_clears(tmp1, tmp2, kk, d2, NULL);
}

/**
 * input : pkPCH, m, h, r
 * output: bool
 */
bool PCH_DSS_2019::Check(PCH_DSS_2019_pk &pkPCH, mpz_t m, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r) {
    mpz_t tmp1, tmp2, tmp3;
    mpz_inits(tmp1, tmp2, tmp3, NULL);

    // h1 = H1(m,N1,N2)* r1^e mod N1
    H1(tmp1, m, pkPCH.getPkCHET()[N1], h.getH()[N2], pkPCH.getPkCHET()[N1]);
    // r1^e mod N1
    mpz_powm(tmp2, r[r1], pkPCH.getPkCHET()[e], pkPCH.getPkCHET()[N1]);
    mpz_mul(tmp3, tmp1, tmp2);
    mpz_mod(tmp3, tmp3, pkPCH.getPkCHET()[N1]);
    if(mpz_cmp(tmp3, h.getH()[h1]) != 0){
        mpz_clears(tmp1, tmp2, tmp3, NULL);
        return false;
    }

    // h2 = H2(m,N1,N2)* r2^e mod N2
    H2(tmp1, m, pkPCH.getPkCHET()[N1], h.getH()[N2], h.getH()[N2]);
    // r2^e mod N2
    mpz_powm(tmp2, r[r2], pkPCH.getPkCHET()[e], h.getH()[N2]);
    mpz_mul(tmp3, tmp1, tmp2);
    mpz_mod(tmp3, tmp3, h.getH()[N2]);
    if(mpz_cmp(tmp3, h.getH()[h2]) != 0){
        mpz_clears(tmp1, tmp2, tmp3, NULL);
        return false;
    }
    mpz_clears(tmp1, tmp2, tmp3, NULL);
    return true;
}

/**
 * input : pkPCH, sksPCH, m, m', h, r
 * output: r'
 */
void PCH_DSS_2019::Adapt(PCH_DSS_2019_r &r_p, mpz_t m_p, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r, mpz_t m, PCH_DSS_2019_pk &pkPCH, PCH_DSS_2019_sks &sksPCH, const std::string &policy_str) {
    mpz_t kk,rr,d2,x1,x1_p,y1,x2,x2_p,y2, r1_p, r2_p;
    mpz_inits(kk,rr,d2,x1,x1_p,y1,x2,x2_p,y2, r1_p, r2_p, NULL);

    cp_abe.Decrypt(tmp_GT, h.getCt(), policy_str, pkPCH.getMpkABE(),  sksPCH.getSksABE());

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
    cp_abe.Encrypt(tmp_ct, pkPCH.getMpkABE(), tmp_GT, policy_str, tmp_Zn_3, tmp_Zn_4);
    if(!(tmp_ct == h.getCt())){
        throw std::runtime_error("PCH_DSS_2019::Adapt(): ct != ct'");
    }

    // DecSE(K, ct_) -> d2
    this->aes.Dec(d2, K, h.getCt_()[ct_], k_bits);

    // Hash Check
    if(!(Check(pkPCH, m, h, r))){
        throw std::runtime_error("PCH_DSS_2019::Adapt(): Hash check failed");
    }

    // x1 = H1(m, N1, N2)
    H1(x1, m, pkPCH.getPkCHET()[N1], h.getH()[N2], pkPCH.getPkCHET()[N1]);
    // x1' = H1(m', N1, N2)
    H1(x1_p, m_p, pkPCH.getPkCHET()[N1], h.getH()[N2], pkPCH.getPkCHET()[N1]);
    // y1 = x1 r1^e mod N1
    mpz_powm(y1, r[r1], pkPCH.getPkCHET()[e], pkPCH.getPkCHET()[N1]);
    mpz_mul(y1, x1, y1);
    mpz_mod(y1, y1, pkPCH.getPkCHET()[N1]);
    // x2 = H2(m, N1, N2)
    H2(x2, m, pkPCH.getPkCHET()[N1], h.getH()[N2], h.getH()[N2]);
    // x2' = H2(m', N1, N2)
    H2(x2_p, m_p, pkPCH.getPkCHET()[N1], h.getH()[N2], h.getH()[N2]);
    // y2 = x2 r2^e mod N2
    mpz_powm(y2, r[r2], pkPCH.getPkCHET()[e], h.getH()[N2]);
    mpz_mul(y2, x2, y2);
    mpz_mod(y2, y2, h.getH()[N2]);
    // r1' = (y1(x1'^(-1)))^d1 mod N1
    mpz_invert(x1_p, x1_p, pkPCH.getPkCHET()[N1]);
    mpz_mul(r1_p, y1, x1_p);
    mpz_mod(r1_p, r1_p, pkPCH.getPkCHET()[N1]);
    mpz_powm(r1_p, r1_p, sksPCH.getSkCHET()[d1], pkPCH.getPkCHET()[N1]);
    r_p.set(r1, r1_p);
    // r2' = (y2(x2'^(-1)))^d2 mod N2
    mpz_invert(x2_p, x2_p, h.getH()[N2]);
    mpz_mul(r2_p, y2, x2_p);
    mpz_mod(r2_p, r2_p, h.getH()[N2]);
    mpz_powm(r2_p, r2_p, d2, h.getH()[N2]);
    r_p.set(r2, r2_p);
  

    mpz_clears(kk,rr,d2,x1,x1_p,y1,x2,x2_p,y2, r1_p, r2_p, NULL);

    if(!(Check(pkPCH, m_p, h, r_p))){
        throw std::runtime_error("PCH_DSS_2019::Adapt(): Adapt failed");
    }
}

/**
 * input : pkPCH, m', h, r'
 * output: bool
 */
bool PCH_DSS_2019::Verify(PCH_DSS_2019_pk &pkPCH, mpz_t m_p, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r_p) {
    return this->Check(pkPCH, m_p, h, r_p);
}


PCH_DSS_2019::~PCH_DSS_2019() {
    element_clear(tmp_GT);
    element_clear(tmp_Zn);
    element_clear(tmp_Zn_2);
    element_clear(tmp_Zn_3);
    element_clear(tmp_Zn_4);

    element_clear(G1);
    element_clear(G2);
    element_clear(GT);
    element_clear(Zn);
}