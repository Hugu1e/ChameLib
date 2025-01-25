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
void PCH_DSS_2019::SetUp(PCH_DSS_2019_pk *pkPCH, PCH_DSS_2019_sk *skPCH, int k) {
    mpz_t n, e, d;
    mpz_inits(n, e, d, NULL);
    
    this->k = k;
    // e > N
    rsa.KeyGen(n, e, d, k, 1);
    // set d1
    skPCH->getSkCHET()->insertElement("d1", d);
    // set N1,e
    pkPCH->getPkCHET()->insertElement("N1", n);
    pkPCH->getPkCHET()->insertElement("e", e);

    this->cp_abe.Setup(skPCH->getMskABE(), pkPCH->getMpkABE());
}

/**
 * input : skPCH, pkPCH, attr_list
 * output: sksPCH
 */
void PCH_DSS_2019::KeyGen(PCH_DSS_2019_sk *skPCH, PCH_DSS_2019_pk *pkPCH, std::vector<std::string> *attr_list, PCH_DSS_2019_sks *sksPCH) {
    this->cp_abe.KeyGen(sksPCH->getSksABE(), skPCH->getMskABE(), pkPCH->getMpkABE(), attr_list);
    sksPCH->getSkCHET()->insertElement("d1", skPCH->getSkCHET()->getElement("d1"));
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
void PCH_DSS_2019::Hash(PCH_DSS_2019_pk *pkPCH, mpz_t m, std::string policy_str, PCH_DSS_2019_h *h, PCH_DSS_2019_r *r) {
    mpz_t tmp1, tmp2, rr, kk, N2, d2, N1, r1, r2, e, h1, h2, ct_;
    mpz_inits(tmp1, tmp2, rr, kk, N2, d2, N1, r1, r2, e, h1, h2, ct_, NULL);

    mpz_set(N1, pkPCH->getPkCHET()->getElement("N1"));
    mpz_set(e, pkPCH->getPkCHET()->getElement("e"));

    rsa.KeyGen_E(N2, d2, e, this->k);

    // h.N2
    h->getH()->insertElement("N2", N2);
    
    // r1 ∈ ZN1*
    RandomGenerator::RandomCoprimeN(r1, N1);
    r->insertElement("r1", r1);
    // r2 ∈ ZN2*
    RandomGenerator::RandomCoprimeN(r2, N2);
    r->insertElement("r2", r2);

    // h1 = H1(m,N1,N2)* r1^e mod N1
    H1(tmp1, m, N1, N2, N1);
    // r1^e mod N1
    mpz_powm(tmp2, r1, e, N1);
    mpz_mul(h1, tmp1, tmp2);
    mpz_mod(h1, h1, N1);
    h->getH()->insertElement("h1", h1);

    // h2 = H2(m,N1,N2)* r2^e mod N2
    H2(tmp1, m, N1, N2, N2);
    // r2^e mod N2
    mpz_powm(tmp2, r2, e, N2);
    mpz_mul(h2, tmp1, tmp2);
    mpz_mod(h2, h2, N2);
    h->getH()->insertElement("h2", h2);

    aes.KGen(this->K, 256);
    // (u1,u2)<-H4((r,A))
    H4(u1, u2, rr, policy_str);

    cp_abe.Encrypt(h->getCt(), pkPCH->getMpkABE(), this->K, policy_str, u1, u2);

    // ct_
    aes.Enc(ct_, this->K, d2);
    h->getCt_()->insertElement("ct_", ct_);

    mpz_clears(tmp1, tmp2, rr, kk, N2, d2, N1, r1, r2, e, h1, h2, ct_, NULL);
}

/**
 * input : pkPCH, m, h, r
 * output: bool
 */
bool PCH_DSS_2019::Check(PCH_DSS_2019_pk *pkPCH, mpz_t m, PCH_DSS_2019_h *h, PCH_DSS_2019_r *r) {
    mpz_t tmp1, tmp2, tmp3, N1, N2, e;
    mpz_inits(tmp1, tmp2, tmp3, N1, N2, e, NULL);

    mpz_set(N1, pkPCH->getPkCHET()->getElement("N1"));
    mpz_set(N2, h->getH()->getElement("N2"));
    mpz_set(e, pkPCH->getPkCHET()->getElement("e"));

    // h1 = H1(m,N1,N2)* r1^e mod N1
    H1(tmp1, m, N1, N2, N1);
    // r1^e mod N1
    mpz_powm(tmp2, r->getElement("r1"), e, N1);
    mpz_mul(tmp3, tmp1, tmp2);
    mpz_mod(tmp3, tmp3, N1);
    if(mpz_cmp(tmp3, h->getH()->getElement("h1")) != 0){
        mpz_clears(tmp1, tmp2,tmp3, NULL);
        return false;
    }

    // h2 = H2(m,N1,N2)* r2^e mod N2
    H2(tmp1, m, N1, N2, N2);
    // r2^e mod N2
    mpz_powm(tmp2, r->getElement("r2"), e, N2);
    mpz_mul(tmp3, tmp1, tmp2);
    mpz_mod(tmp3, tmp3, N2);
    if(mpz_cmp(tmp3, h->getH()->getElement("h2")) != 0){
        mpz_clears(tmp1, tmp2,tmp3, NULL);
        return false;
    }
    mpz_clears(tmp1, tmp2, tmp3, N1, N2, e, NULL);
    return true;
}

/**
 * input : pkPCH, sksPCH, m, m', h, r
 * output: r'
 */
void PCH_DSS_2019::Adapt(PCH_DSS_2019_pk * pkPCH, PCH_DSS_2019_sks *sksPCH, mpz_t m, mpz_t m_p, PCH_DSS_2019_h *h, PCH_DSS_2019_r *r, PCH_DSS_2019_r *r_p) {
    mpz_t kk,rr,d2,x1,x1_p,y1,x2,x2_p,y2,N1,N2,e, r1_p, r2_p;
    mpz_inits(kk,rr,d2,x1,x1_p,y1,x2,x2_p,y2,N1,N2,e, r1_p, r2_p, NULL);

    mpz_set(N1, pkPCH->getPkCHET()->getElement("N1"));
    mpz_set(N2, h->getH()->getElement("N2"));
    mpz_set(e, pkPCH->getPkCHET()->getElement("e"));

    cp_abe.Decrypt(this->K, h->getCt(), pkPCH->getMpkABE(),  sksPCH->getSksABE());

    // DecSE(K, ct_) -> d2
    this->aes.Dec(d2, this->K, h->getCt_()->getElement("ct_"));

    // x1 = H1(m, N1, N2)
    H1(x1, m, N1, N2, N1);
    // x1' = H1(m', N1, N2)
    H1(x1_p, m_p, N1, N2, N1);
    // y1 = x1 r1^e mod N1
    mpz_powm(y1, r->getElement("r1"), e, N1);
    mpz_mul(y1, x1, y1);
    mpz_mod(y1, y1, N1);
    // x2 = H2(m, N1, N2)
    H2(x2, m, N1, N2, N2);
    // x2' = H2(m', N1, N2)
    H2(x2_p, m_p, N1, N2, N2);
    // y2 = x2 r2^e mod N2
    mpz_powm(y2, r->getElement("r2"), e, N2);
    mpz_mul(y2, x2, y2);
    mpz_mod(y2, y2, N2);
    // r1' = (y1(x1'^(-1)))^d1 mod N1
    mpz_invert(x1_p, x1_p, N1);
    mpz_mul(r1_p, y1, x1_p);
    mpz_mod(r1_p, r1_p, N1);
    mpz_powm(r1_p, r1_p, sksPCH->getSkCHET()->getElement("d1"), N1);
    r_p->insertElement("r1", r1_p);
    // r2' = (y2(x2'^(-1)))^d2 mod N2
    mpz_invert(x2_p, x2_p, N2);
    mpz_mul(r2_p, y2, x2_p);
    mpz_mod(r2_p, r2_p, N2);
    mpz_powm(r2_p, r2_p, d2, N2);
    r_p->insertElement("r2", r2_p);
  

    mpz_clears(kk,rr,d2,x1,x1_p,y1,x2,x2_p,y2,N1,N2,e, r1_p, r2_p, NULL);
}

/**
 * input : pkPCH, m', h, r'
 * output: bool
 */
bool PCH_DSS_2019::Verify(PCH_DSS_2019_pk *pkPCH, mpz_t m_p, PCH_DSS_2019_h *h, PCH_DSS_2019_r *r_p) {
    return this->Check(pkPCH, m_p, h, r_p);
}


PCH_DSS_2019::~PCH_DSS_2019() {
    element_clear(this->u1);
    element_clear(this->u2);
    element_clear(this->K);
}