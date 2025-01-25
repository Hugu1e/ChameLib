#include <scheme/IBCH/IB_CH_ZSS_S2_2003.h>

IB_CH_ZSS_S2_2003::IB_CH_ZSS_S2_2003(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn): PbcScheme(_G1, _G2, _GT, _Zn) {}

void IB_CH_ZSS_S2_2003::SetUp(IB_CH_ZSS_S2_2003_msk *mks, IB_CH_ZSS_S2_2003_pp *pp) {
    element_random(tmp_Zn);
    mks->insertElement("s", "Zn", tmp_Zn);
    
    element_random(tmp_G);
    pp->insertElement("P", "G1", tmp_G);

    element_mul_zn(tmp_G, tmp_G, tmp_Zn);
    pp->insertElement("Ppub", "G1", tmp_G);
}


void IB_CH_ZSS_S2_2003::Extract(IB_CH_ZSS_S2_2003_sk *sk, IB_CH_ZSS_S2_2003_msk *msk, element_t ID, IB_CH_ZSS_S2_2003_pp *pp) {
    H1(this->tmp_Zn, ID);
    element_add(this->tmp_Zn, msk->getElement("s"), this->tmp_Zn);
    element_invert(this->tmp_Zn, this->tmp_Zn);
    element_mul_zn(tmp_G, pp->getElement("P"), this->tmp_Zn);
    sk->insertElement("SID", "G1", tmp_G);
}

void IB_CH_ZSS_S2_2003::H1(element_t res, element_t m) {
    HASH::hash(res, m);
}

void IB_CH_ZSS_S2_2003::Hash(IB_CH_ZSS_S2_2003_h *h, element_t m, element_t ID, IB_CH_ZSS_S2_2003_pp *pp) {
    // r
    element_random(tmp_G);
    h->get_r()->insertElement("r", "G1", tmp_G);

    H1(this->tmp_Zn, ID);
    element_mul_zn(this->tmp_G_2, pp->getElement("P"), this->tmp_Zn);
    element_add(this->tmp_G_2, this->tmp_G_2, pp->getElement("Ppub"));
    element_pairing(tmp_GT, tmp_G_2, tmp_G);

    H1(this->tmp_Zn, m);
    element_pow_zn(tmp_GT, tmp_GT, this->tmp_Zn);

    element_pairing(this->tmp_GT_2, pp->getElement("P"), pp->getElement("P"));
    element_pow_zn(this->tmp_GT_2, this->tmp_GT_2, this->tmp_Zn);
    
    element_mul(tmp_GT, tmp_GT, tmp_GT_2);
    h->get_h()->insertElement("h", "GT", tmp_GT);
}

bool IB_CH_ZSS_S2_2003::Check(IB_CH_ZSS_S2_2003_h *h, element_t m, element_t ID, IB_CH_ZSS_S2_2003_pp *pp){
    H1(this->tmp_Zn, ID);
    element_mul_zn(this->tmp_G_2, pp->getElement("P"), this->tmp_Zn);
    element_add(this->tmp_G_2, this->tmp_G_2, pp->getElement("Ppub"));
    element_pairing(tmp_GT, tmp_G_2, h->get_r()->getElement("r"));

    H1(this->tmp_Zn, m);
    element_pow_zn(tmp_GT, tmp_GT, this->tmp_Zn);

    element_pairing(this->tmp_GT_2, pp->getElement("P"), pp->getElement("P"));
    element_pow_zn(this->tmp_GT_2, this->tmp_GT_2, this->tmp_Zn);
    
    element_mul(tmp_GT, tmp_GT, tmp_GT_2);
    
    return element_cmp(h->get_h()->getElement("h"), tmp_GT) == 0;
}

void IB_CH_ZSS_S2_2003::Adapt(IB_CH_ZSS_S2_2003_h *h_p, element_t m_p, IB_CH_ZSS_S2_2003_h *h, element_t m, element_t ID, IB_CH_ZSS_S2_2003_sk *sk, IB_CH_ZSS_S2_2003_pp *pp) {
    this->H1(this->tmp_Zn, m);
    this->H1(this->tmp_Zn_2, m_p);
    element_sub(this->tmp_Zn_3, this->tmp_Zn, this->tmp_Zn_2);

    element_mul_zn(this->tmp_G, sk->getElement("SID"), this->tmp_Zn_3);
    element_mul_zn(this->tmp_G_2, h->get_r()->getElement("r"), this->tmp_Zn);
    element_add(this->tmp_G, this->tmp_G, this->tmp_G_2);

    element_invert(tmp_Zn_2, tmp_Zn_2);
    element_mul_zn(this->tmp_G, tmp_G, this->tmp_Zn_2);

    h_p->get_r()->insertElement("r", "G1", tmp_G);
    h_p->get_h()->insertElement("h", "GT", h->get_h()->getElement("h"));
}

bool IB_CH_ZSS_S2_2003::Verify(IB_CH_ZSS_S2_2003_h *h_p, element_t m_p, element_t ID, IB_CH_ZSS_S2_2003_pp *pp) {
    return this->Check(h_p, m_p, ID, pp);
}


IB_CH_ZSS_S2_2003::~IB_CH_ZSS_S2_2003(){}