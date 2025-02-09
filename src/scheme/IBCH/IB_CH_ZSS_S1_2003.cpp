#include <scheme/IBCH/IB_CH_ZSS_S1_2003.h>

IB_CH_ZSS_S1_2003::IB_CH_ZSS_S1_2003(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn) : PbcScheme(_G1, _G2, _GT, _Zn) {}

void IB_CH_ZSS_S1_2003::SetUp(IB_CH_ZSS_S1_2003_pp &pp, IB_CH_ZSS_S1_2003_msk &msk, IB_CH_ZSS_S1_2003_pk &pk, IB_CH_ZSS_S1_2003_sk &sk, IB_CH_ZSS_S1_2003_h &h, IB_CH_ZSS_S1_2003_h &h_p) {
    pp.init(2);
    msk.init(1);
    pk.init(1);
    sk.init(1);
    h.get_h().init(1);
    h.get_r().init(1);
    h_p.get_h().init(1);
    h_p.get_r().init(1);

    
    // s
    element_random(tmp_Zn);
    msk.set(s, tmp_Zn);
    // P
    element_random(tmp_H);
    pp.set(P, tmp_H);
    // Ppub = sP
    element_mul_zn(tmp_H, tmp_H, tmp_Zn);
    pp.set(Ppub, tmp_H);
}

void IB_CH_ZSS_S1_2003::Extract(IB_CH_ZSS_S1_2003_pk &pk, IB_CH_ZSS_S1_2003_sk &sk, IB_CH_ZSS_S1_2003_msk &msk, element_t ID) {
    // QID = H(ID)
    this->H(this->tmp_G, ID);
    pk.set(QID, this->tmp_G);

    // SID = s*QID
    element_mul_zn(tmp_G, this->tmp_G, msk[s]);
    sk.set(SID, tmp_G);
}

void IB_CH_ZSS_S1_2003::H(element_t res, element_t m) {
    HASH::hash(res, m);
}

void IB_CH_ZSS_S1_2003::Hash(IB_CH_ZSS_S1_2003_h &h, element_t m, element_t ID, IB_CH_ZSS_S1_2003_pp &pp) {
    // r
    element_random(tmp_G);
    h.get_r().set(r1, tmp_G);

    // e(R, P)
    element_pairing(tmp_GT, tmp_G, pp[P]);
    
    this->H(this->tmp_G, ID);
    this->H(this->tmp_Zn, m);
    element_mul_zn(this->tmp_G, this->tmp_G, this->tmp_Zn);
    element_pairing(this->tmp_GT_2, this->tmp_G, pp[Ppub]);
    element_mul(tmp_GT, tmp_GT, tmp_GT_2);
    h.get_h().set(h1, tmp_GT);
}

bool IB_CH_ZSS_S1_2003::Check(IB_CH_ZSS_S1_2003_h &h, element_t m, element_t ID, IB_CH_ZSS_S1_2003_pp &pp){
    // e(R, P)
    element_pairing(tmp_GT, h.get_r()[r1], pp[P]);
    
    this->H(this->tmp_G, ID);
    this->H(this->tmp_Zn, m);
    element_mul_zn(this->tmp_G, this->tmp_G, this->tmp_Zn);
    element_pairing(this->tmp_GT_2, this->tmp_G, pp[Ppub]);
    element_mul(tmp_GT, tmp_GT, tmp_GT_2);
    
    return element_cmp(h.get_h()[h1], tmp_GT) == 0;
}

void IB_CH_ZSS_S1_2003::Adapt(IB_CH_ZSS_S1_2003_h &h_p, element_t m_p, IB_CH_ZSS_S1_2003_h &h, element_t m, element_t ID, IB_CH_ZSS_S1_2003_sk &sk, IB_CH_ZSS_S1_2003_pp &pp) {
    this->H(this->tmp_Zn, m);
    this->H(this->tmp_Zn_2, m_p);
    element_sub(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);

    element_mul_zn(this->tmp_G, sk[SID], this->tmp_Zn);
    element_add(tmp_G, this->tmp_G, h.get_r()[r1]);
    h_p.get_r().set(r1, tmp_G);

    h_p.get_h().set(h1, h.get_h()[h1]);
}

bool IB_CH_ZSS_S1_2003::Verify(IB_CH_ZSS_S1_2003_h &h_p, element_t m_p, element_t ID, IB_CH_ZSS_S1_2003_pp &pp) {
    return this->Check(h_p, m_p, ID, pp);
}

IB_CH_ZSS_S1_2003::~IB_CH_ZSS_S1_2003() {}
