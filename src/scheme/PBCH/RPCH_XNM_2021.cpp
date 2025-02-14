#include <scheme/PBCH/RPCH_XNM_2021.h>

RPCH_XNM_2021::RPCH_XNM_2021(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn): PbcScheme(_G1, _G2, _GT, _Zn), rabe(_G1, _G2, _GT, _Zn){
    element_init_same_as(this->s1, Zn);
    element_init_same_as(this->s2, Zn);
    element_init_same_as(this->K, GT);
}

/**
 * input : k, n
 * output: skRPCH, pkRPCH, _rl, _st
 */
void RPCH_XNM_2021::SetUp(RPCH_XNM_2021_sk &skRPCH, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2021_RevokedPresonList &rl, RPCH_XNM_2022_Binary_tree &st, int k, int n) {
    skRPCH.get_skCHET().init(1);
    pkRPCH.get_pkCHET().init(2);
    
    this->k = k;

    // e > N
    rsa.KeyGen(pkRPCH.get_pkCHET()[N1], pkRPCH.get_pkCHET()[e], skRPCH.get_skCHET()[d1], k, 1);
    
    rabe.Setup(pkRPCH.get_mpkRABE(), skRPCH.get_mskRABE(), rl.get_rl(), st.get_st(), n);
}

/**
 * input : pkRPCH, skRPCH, _st, id, attr_list
 * output: skidRPCH
 */
void RPCH_XNM_2021::KeyGen(RPCH_XNM_2021_skid &skidRPCH, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2021_sk &skRPCH, RPCH_XNM_2022_Binary_tree &st, std::vector<std::string> &attr_list, element_t id, time_t re_time){
    skidRPCH.get_skCHET().init(1);
    rabe.KGen(skidRPCH.get_skidRABE(), st.get_st(), pkRPCH.get_mpkRABE(), skRPCH.get_mskRABE(), attr_list, id, re_time);
    skidRPCH.get_skCHET().set(d1, skRPCH.get_skCHET()[d1]);
}

void RPCH_XNM_2021::H1(mpz_t res, mpz_t m, mpz_t N1, mpz_t N2, mpz_t n){
    HASH::hash_n(res, m, N1, N2, n);
}
void RPCH_XNM_2021::H2(mpz_t res, mpz_t m, mpz_t N1, mpz_t N2, mpz_t n){
    HASH::hash_n(res, m, N1, N2, n);
}

/**
 * (u1,u2)<-H4((r,A))
 */
void RPCH_XNM_2021::H4(element_t u1, element_t u2, mpz_t r, std::string A){
    // r -> string
    std::string r_str = mpz_get_str(NULL, 10, r);
    // str -> element_t
    element_from_hash(this->tmp_Zn, (unsigned char *)r_str.c_str(), r_str.length());
    element_from_hash(this->tmp_Zn_2, (unsigned char *)A.c_str(), A.length());
    HASH::hash(u1, this->tmp_Zn);
    HASH::hash(u2, this->tmp_Zn_2);
}

/**
 * input : pkRPCH, _st, _rl, t
 * output: kut
 */
void RPCH_XNM_2021::KUpt(RPCH_XNM_2022_kut &kut, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2022_Binary_tree &st, RPCH_XNM_2021_RevokedPresonList &rl, time_t t){
    rabe.KUpt(kut.get_kut(), pkRPCH.get_mpkRABE(), st.get_st(), rl.get_rl(), t);
}

/**
 * input : pkRPCH, skidRPCH, kut
 * output: dkidtRPCH
 */
void RPCH_XNM_2021::DKGen(RPCH_XNM_2021_dkidt &dkidtRPCH, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2021_skid &skidRPCH, RPCH_XNM_2022_kut &kut){
    dkidtRPCH.get_skCHET().init(1);
    
    rabe.DKGen(dkidtRPCH.get_dkidtRABE(), pkRPCH.get_mpkRABE(), skidRPCH.get_skidRABE(), kut.get_kut());
    dkidtRPCH.get_skCHET().set(d1, skidRPCH.get_skCHET()[d1]);
}

/**
 * input : _rl, id, t
 */
void RPCH_XNM_2021::Rev(RPCH_XNM_2021_RevokedPresonList &rl, element_t id, time_t t){
    this->rabe.Rev(rl.get_rl(), id, t);
}


/**
 * input : pkRPCH, policy_str, t
 * output: h
 */
void RPCH_XNM_2021::Hash(RPCH_XNM_2021_h &h, mpz_t m, RPCH_XNM_2021_pk &pkRPCH, std::string policy_str, time_t t) {
    h.get_r().init(4); 
    h.get_h().init(2);
    
    mpz_t tmp1, tmp2, d2;
    mpz_inits(tmp1, tmp2, d2, N1, NULL);

    rsa.KeyGen_E(h.get_r()[N2], d2, pkRPCH.get_pkCHET()[e], this->k);
    
    // r1 ∈ ZN1*
    RandomGenerator::RandomCoprimeN(h.get_r()[r1], pkRPCH.get_pkCHET()[N1]);
    // r2 ∈ ZN2*
    RandomGenerator::RandomCoprimeN(h.get_r()[r2], h.get_r()[N2]);

    // h1 = H1(m,N1,N2)* r1^e mod N1
    H1(tmp1, m, pkRPCH.get_pkCHET()[N1], h.get_r()[N2], pkRPCH.get_pkCHET()[N1]);
    // r1^e mod N1
    mpz_powm(tmp2, h.get_r()[r1], pkRPCH.get_pkCHET()[e], pkRPCH.get_pkCHET()[N1]);
    mpz_mul(h.get_h()[h1], tmp1, tmp2);
    mpz_mod(h.get_h()[h1], h.get_h()[h1], pkRPCH.get_pkCHET()[N1]);

    // h2 = H2(m,N1,N2)* r2^e mod N2
    H2(tmp1, m, pkRPCH.get_pkCHET()[N1], h.get_r()[N2], h.get_r()[N2]);
    // r2^e mod N2
    mpz_powm(tmp2, h.get_r()[r2], pkRPCH.get_pkCHET()[e], h.get_r()[N2]);
    mpz_mul(h.get_h()[h2], tmp1, tmp2);
    mpz_mod(h.get_h()[h2], h.get_h()[h2], h.get_r()[N2]);

    this->aes.KGen(this->K, 256);

    element_random(this->s1);
    element_random(this->s2);

    this->rabe.Enc(h.get_r().get_ct(), pkRPCH.get_mpkRABE(), this->K, policy_str, t, this->s1, this->s2);

    // cSE = EncSE(kk, d2)
    this->aes.Enc(h.get_r()[cSE], K, d2);

    mpz_clears(tmp1, tmp2, d2, NULL);
}

/**
 * input : pkRPCH, h
 * output: bool
 */
bool RPCH_XNM_2021::Check(RPCH_XNM_2021_pk &pkRPCH, mpz_t m, RPCH_XNM_2021_h &h) {
    mpz_t tmp1, tmp2, tmp3;
    mpz_inits(tmp1, tmp2, tmp3, NULL);

    // h1 = H1(m,N1,N2)* r1^e mod N1
    H1(tmp1, m, pkRPCH.get_pkCHET()[N1], h.get_r()[N2], pkRPCH.get_pkCHET()[N1]);
    // r1^e mod N1
    mpz_powm(tmp2, h.get_r()[r1], pkRPCH.get_pkCHET()[e], pkRPCH.get_pkCHET()[N1]);
    mpz_mul(tmp3, tmp1, tmp2);
    mpz_mod(tmp3, tmp3, pkRPCH.get_pkCHET()[N1]);
    if(mpz_cmp(tmp3, h.get_h()[h1]) != 0){
        mpz_clears(tmp1, tmp2, tmp3, NULL);
        return false;
    }
    // h2 = H2(m,N1,N2)* r2^e mod N2
    H2(tmp1, m, pkRPCH.get_pkCHET()[N1], h.get_r()[N2], h.get_r()[N2]);
    // r2^e mod N2
    mpz_powm(tmp2, h.get_r()[r2], pkRPCH.get_pkCHET()[e], h.get_r()[N2]);
    mpz_mul(tmp3, tmp1, tmp2);
    mpz_mod(tmp3, tmp3, h.get_r()[N2]);
    if(mpz_cmp(tmp3, h.get_h()[h2]) != 0){
        mpz_clears(tmp1, tmp2, tmp3, NULL);
        return false;
    }
    mpz_clears(tmp1, tmp2, tmp3, NULL);
    return true;
}

/**
 * input : pkRPCH, dkidtRPCH, h
 * output: h_p
 */
void RPCH_XNM_2021::Adapt(RPCH_XNM_2021_h &h_p, mpz_t m_p, mpz_t m, RPCH_XNM_2021_h &h, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2021_dkidt &dkidtRPCH) {
    h_p.get_r().init(4);
    h_p.get_h().init(2);
    
    mpz_t kk,rr,d2,x1,x1_p,y1,x2,x2_p,y2, r1_p, r2_p;
    mpz_inits(kk,rr,d2,x1,x1_p,y1,x2,x2_p,y2, r1_p, r2_p, NULL);

    rabe.Dec(this->K, pkRPCH.get_mpkRABE(), h.get_r().get_ct(), dkidtRPCH.get_dkidtRABE());

    // DecSE(kk, ct_) -> d2
    this->aes.Dec(d2, this->K, h.get_r()[cSE]);


    // x1 = H1(m, N1, N2)
    H1(x1, m, pkRPCH.get_pkCHET()[N1], h.get_r()[N2], pkRPCH.get_pkCHET()[N1]);
    // x1' = H1(m', N1, N2)
    H1(x1_p, m_p, pkRPCH.get_pkCHET()[N1], h.get_r()[N2], pkRPCH.get_pkCHET()[N1]);
    // y1 = x1 r1^e mod N1
    mpz_powm(y1, h.get_r()[r1], pkRPCH.get_pkCHET()[e], pkRPCH.get_pkCHET()[N1]);
    mpz_mul(y1, x1, y1);
    mpz_mod(y1, y1, pkRPCH.get_pkCHET()[N1]);
    // x2 = H2(m, N1, N2)
    H2(x2, m, pkRPCH.get_pkCHET()[N1], h.get_r()[N2], h.get_r()[N2]);
    // x2' = H2(m', N1, N2)
    H2(x2_p, m_p, pkRPCH.get_pkCHET()[N1], h.get_r()[N2], h.get_r()[N2]);
    // y2 = x2 r2^e mod N2
    mpz_powm(y2, h.get_r()[r2], pkRPCH.get_pkCHET()[e], h.get_r()[N2]);
    mpz_mul(y2, x2, y2);
    mpz_mod(y2, y2, h.get_r()[N2]);
    // r1' = (y1(x1'^(-1)))^d1 mod N1
    mpz_invert(x1_p, x1_p, pkRPCH.get_pkCHET()[N1]);
    mpz_mul(r1_p, y1, x1_p);
    mpz_mod(r1_p, r1_p, pkRPCH.get_pkCHET()[N1]);
    mpz_powm(r1_p, r1_p, dkidtRPCH.get_skCHET()[d1], pkRPCH.get_pkCHET()[N1]);
    h_p.get_r().set(r1, r1_p);

    // r2' = (y2(x2'^(-1)))^d2 mod N2
    mpz_invert(x2_p, x2_p, h.get_r()[N2]);
    mpz_mul(r2_p, y2, x2_p);
    mpz_mod(r2_p, r2_p, h.get_r()[N2]);
    mpz_powm(r2_p, r2_p, d2, h.get_r()[N2]);
    h_p.get_r().set(r2, r2_p);

    h_p.get_r().set(N2, h.get_r()[N2]);

    h_p.get_h().set(h1, h.get_h()[h1]);
    h_p.get_h().set(h2, h.get_h()[h2]);

    mpz_clears(kk,rr,d2,x1,x1_p,y1,x2,x2_p,y2, r1_p, r2_p, NULL);
}

/**
 * input : pkPCH, h_p
 * output: bool
 */
bool RPCH_XNM_2021::Verify(RPCH_XNM_2021_pk &pkRPCH, mpz_t m_p, RPCH_XNM_2021_h &h_p) {
    return this->Check(pkRPCH, m_p, h_p);
}


RPCH_XNM_2021::~RPCH_XNM_2021() {
    element_clear(this->s1);
    element_clear(this->s2);
    element_clear(this->K);
}