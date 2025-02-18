#include <scheme/PBCH/RPCH_TMM_2022.h>

RPCH_TMM_2022::RPCH_TMM_2022(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn): PbcScheme(_G1, _G2, _GT, _Zn), rabe(_G1, _G2, _GT, _Zn){
    element_init_same_as(this->s1, Zn);
    element_init_same_as(this->s2, Zn);
    element_init_same_as(this->K, GT);
    element_init_same_as(this->R, Zn);
}

/**
 * input : k, n
 * output: skRPCH, pkRPCH, _rl, _st
 */
void RPCH_TMM_2022::SetUp(RPCH_TMM_2022_sk &skRPCH, RPCH_TMM_2022_pk &pkRPCH, RPCH_TMM_2022_RevokedPresonList &rl, RPCH_TMM_2022_Binary_tree &st, int k, int n) {
    skRPCH.get_skCHET().init(1);
    pkRPCH.get_pkCHET().init(1);

    this->k = k;
    
    rabe.Setup(pkRPCH.get_mpkRABE(), skRPCH.get_mskRABE(), rl.get_rl(), st.get_st(), n);

    element_random(tmp_Zn);
    skRPCH.get_skCHET().set(x, tmp_Zn);
    // y = g^x
    element_pow_zn(tmp_G, pkRPCH.get_mpkRABE()[RABE_TMM::g], skRPCH.get_skCHET()[x]);
    pkRPCH.get_pkCHET().set(y, tmp_G);
}

/**
 * input : pkRPCH, skRPCH, _st, id, attr_list
 * output: skidRPCH
 */
void RPCH_TMM_2022::KeyGen(RPCH_TMM_2022_skid &skidRPCH, RPCH_TMM_2022_pk &pkRPCH, RPCH_TMM_2022_sk &skRPCH, RPCH_TMM_2022_Binary_tree &st, std::vector<std::string> &attr_list, element_t id, time_t re_time) {
    skidRPCH.get_skCHET().init(1);
    rabe.KGen(skidRPCH.get_skidRABE(), st.get_st(), pkRPCH.get_mpkRABE(), skRPCH.get_mskRABE(), attr_list, id, re_time);
    skidRPCH.get_skCHET().set(x, skRPCH.get_skCHET()[x]);
}


/**
 * input : pkRPCH, _st, _rl, t
 * output: kut
 */
void RPCH_TMM_2022::KUpt(RPCH_TMM_2022_kut &kut, RPCH_TMM_2022_pk &pkRPCH, RPCH_TMM_2022_Binary_tree &st, RPCH_TMM_2022_RevokedPresonList &rl, time_t t){
    rabe.KUpt(kut.get_kut(), pkRPCH.get_mpkRABE(), st.get_st(), rl.get_rl(), t);
}

/**
 * input : pkRPCH, skidRPCH, kut
 * output: dkidtRPCH
 */
void RPCH_TMM_2022::DKGen(RPCH_TMM_2022_dkidt &dkidtRPCH, RPCH_TMM_2022_pk &pkRPCH, RPCH_TMM_2022_skid &skidRPCH, RPCH_TMM_2022_kut &kut){
    dkidtRPCH.get_skCHET().init(1);
    rabe.DKGen(dkidtRPCH.get_dkidtRABE(), pkRPCH.get_mpkRABE(), skidRPCH.get_skidRABE(), kut.get_kut());
    dkidtRPCH.get_skCHET().set(x, skidRPCH.get_skCHET()[x]);
}

/**
 * input : _rl, id, t
 */
void RPCH_TMM_2022::Rev(RPCH_TMM_2022_RevokedPresonList &rl, element_t id, time_t t){
    rabe.Rev(rl.get_rl(), id, t);
}


/**
 * input : pkRPCH, m, policy_str, t
 * output: h, r
 */
void RPCH_TMM_2022::Hash(RPCH_TMM_2022_h &h, element_t m, RPCH_TMM_2022_pk &pkRPCH, std::string policy_str, time_t t) {
    h.get_r().init(2);
    h.get_h().init(1);
    
    // r
    element_random(tmp_Zn);
    h.get_r().set(r, tmp_Zn);

    element_random(this->R);
    // h = g^R
    element_pow_zn(tmp_G, pkRPCH.get_mpkRABE()[RABE_TMM::g], this->R);
    h.get_r().set(h1, tmp_G);

    // CH b = pk^m * h^r
    element_pow_zn(this->tmp_G, tmp_G, tmp_Zn);
    element_pow_zn(this->tmp_G_2, pkRPCH.get_pkCHET()[y], m);
    element_mul(tmp_G, this->tmp_G, this->tmp_G_2);
    h.get_h().set(b, tmp_G);

    element_random(this->s1);
    element_random(this->s2);

    this->rabe.Enc(h.get_r().get_C(), pkRPCH.get_mpkRABE(), this->R, policy_str, t, this->s1, this->s2);
}

/**
 * input : pkRPCH, m, h, r
 * output: bool
 */
bool RPCH_TMM_2022::Check(RPCH_TMM_2022_pk &pkRPCH, element_t m, RPCH_TMM_2022_h &h) {
    // CH b = pk^m * h^r
    element_pow_zn(this->tmp_G, h.get_r()[h1], h.get_r()[r]);
    element_pow_zn(this->tmp_G_2, pkRPCH.get_pkCHET()[y], m);
    element_mul(this->tmp_G, this->tmp_G, this->tmp_G_2);
    
    return element_cmp(h.get_h().get(b), this->tmp_G) == 0;
}

/**
 * input : pkRPCH, dkidtRPCH, m, m', h, r
 * output: r'
 */
void RPCH_TMM_2022::Adapt(RPCH_TMM_2022_h &h_p, element_t m_p, element_t m, RPCH_TMM_2022_h &h, RPCH_TMM_2022_pk &pkRPCH, RPCH_TMM_2022_dkidt &dkidtRPCH) {
    // Check
    if (!this->Check(pkRPCH, m, h)) {
        printf("Hash Check failed\n");
        return;
    }

    h_p.get_r().init(2);
    h_p.get_h().init(1);

    rabe.Dec(this->R, pkRPCH.get_mpkRABE(), h.get_r().get_C(), dkidtRPCH.get_dkidtRABE());
    
    // r' = r + (m - m')*sk/R
    element_sub(this->tmp_Zn, m, m_p);
    element_mul(this->tmp_Zn_2, this->tmp_Zn, dkidtRPCH.get_skCHET().get(x));
    element_div(this->tmp_Zn_3, this->tmp_Zn_2, this->R);
    element_add(tmp_Zn, h.get_r().get(r), this->tmp_Zn_3);
    h_p.get_r().set(r, tmp_Zn);

    h_p.get_h().set(b, h.get_h().get(b));
    h_p.get_r().set(h1, h.get_r().get(h1));
}

/**
 * input : pkPCH, m', h, r'
 * output: bool
 */
bool RPCH_TMM_2022::Verify(RPCH_TMM_2022_pk &pkRPCH, element_t m_p, RPCH_TMM_2022_h &h_p) {
    return this->Check(pkRPCH, m_p, h_p);
}


RPCH_TMM_2022::~RPCH_TMM_2022() {
    element_clear(this->s1);
    element_clear(this->s2);
    element_clear(this->K);
    element_clear(this->R);
}