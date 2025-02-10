#include <scheme/PBCH/PCH_DSS_2019.h>

PCH_DSS_2019::PCH_DSS_2019(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn): PbcScheme(_G1, _G2, _GT, _Zn), cp_abe(_G1, _G2, _GT, _Zn){ 
    element_init_same_as(this->u1, Zn);
    element_init_same_as(this->u2, Zn);
    element_init_same_as(this->K, GT);
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
void PCH_DSS_2019::H4(element_t u1, element_t u2, mpz_t r, std::string A){
    // r -> string
    std::string r_str = mpz_get_str(NULL, 10, r);
    HASH::hash(u1, r_str);
    HASH::hash(u2, A);
}


/**
 * input : pkPCH, m, policy_str
 * output: h, r
 */
void PCH_DSS_2019::Hash(PCH_DSS_2019_pk &pkPCH, mpz_t m, const std::string &policy_str, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r) {
    mpz_t tmp1, tmp2, rr, kk, d2;
    mpz_inits(tmp1, tmp2, rr, kk, d2, NULL);

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

    aes.KGen(this->K, 256);
    // (u1,u2)<-H4((r,A))
    H4(u1, u2, rr, policy_str);

    cp_abe.Encrypt(h.getCt(), pkPCH.getMpkABE(), this->K, policy_str, u1, u2);

    // ct_
    aes.Enc(h.getCt_()[ct_], this->K, d2);

    mpz_clears(tmp1, tmp2, rr, kk, d2, NULL);
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
void PCH_DSS_2019::Adapt(PCH_DSS_2019_pk &pkPCH, PCH_DSS_2019_sks &sksPCH, mpz_t m, mpz_t m_p, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r, PCH_DSS_2019_r &r_p) {
    mpz_t kk,rr,d2,x1,x1_p,y1,x2,x2_p,y2, r1_p, r2_p;
    mpz_inits(kk,rr,d2,x1,x1_p,y1,x2,x2_p,y2, r1_p, r2_p, NULL);

    cp_abe.Decrypt(this->K, h.getCt(), pkPCH.getMpkABE(),  sksPCH.getSksABE());

    // DecSE(K, ct_) -> d2
    this->aes.Dec(d2, this->K, h.getCt_()[ct_]);

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
}

/**
 * input : pkPCH, m', h, r'
 * output: bool
 */
bool PCH_DSS_2019::Verify(PCH_DSS_2019_pk &pkPCH, mpz_t m_p, PCH_DSS_2019_h &h, PCH_DSS_2019_r &r_p) {
    return this->Check(pkPCH, m_p, h, r_p);
}


PCH_DSS_2019::~PCH_DSS_2019() {
    element_clear(this->u1);
    element_clear(this->u2);
    element_clear(this->K);
}