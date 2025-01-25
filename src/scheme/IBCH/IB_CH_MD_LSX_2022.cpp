#include <scheme/IBCH/IB_CH_MD_LSX_2022.h>

IB_CH_MD_LSX_2022::IB_CH_MD_LSX_2022(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn) : PbcScheme(_G1, _G2, _GT, _Zn) {}

void IB_CH_MD_LSX_2022::SetUp(IB_CH_MD_LSX_2022_pp *pp, IB_CH_MD_LSX_2022_msk *msk){
    element_random(tmp_G);
    pp->insertElement("g", "G1", tmp_G);

    element_random(tmp_Zn);
    msk->insertElement("a", "Zn", tmp_Zn);
    element_random(tmp_Zn_2);
    msk->insertElement("b", "Zn", tmp_Zn_2);

    element_pow_zn(tmp_G_2, tmp_G, tmp_Zn);
    pp->insertElement("g1", "G1", tmp_G_2);
    element_pow_zn(tmp_G_2, tmp_G, tmp_Zn_2);
    pp->insertElement("g2", "G1", tmp_G_2);

    element_pairing(tmp_GT, tmp_G, tmp_G);
    pp->insertElement("egg", "GT", tmp_GT);
    element_pairing(tmp_GT, tmp_G_2, tmp_G);
    pp->insertElement("eg2g", "GT", tmp_GT);
}

void IB_CH_MD_LSX_2022::KeyGen(IB_CH_MD_LSX_2022_td *td, element_t ID, IB_CH_MD_LSX_2022_msk *msk, IB_CH_MD_LSX_2022_pp *pp) {
    // t
    element_random(tmp_Zn);
    
    // td1
    td->insertElement("td1", "Zn", tmp_Zn);

    // td2
    element_sub(this->tmp_Zn, msk->getElement("b"), tmp_Zn);
    element_sub(this->tmp_Zn_2, msk->getElement("a"), ID);
    element_div(this->tmp_Zn_3, this->tmp_Zn, this->tmp_Zn_2);
    element_pow_zn(tmp_G, pp->getElement("g"), this->tmp_Zn_3);
    td->insertElement("td2", "G1", tmp_G);
}

void IB_CH_MD_LSX_2022::Hash(IB_CH_MD_LSX_2022_h *h, IB_CH_MD_LSX_2022_r *r, element_t ID, element_t m, IB_CH_MD_LSX_2022_pp *pp) {
    // r1
    element_random(tmp_Zn);
    r->insertElement("r1", "Zn", tmp_Zn);
    // r2
    element_random(tmp_G);
    r->insertElement("r2", "G1", tmp_G);

    element_pow_zn(this->tmp_GT, pp->getElement("eg2g"), m);
    element_pow_zn(this->tmp_GT_2, pp->getElement("egg"), tmp_Zn);
    //g1 / g^ID
    element_pow_zn(this->tmp_G_2, pp->getElement("g"), ID);
    element_div(this->tmp_G_3, pp->getElement("g1"), this->tmp_G_2);
    element_pairing(this->tmp_GT_3, tmp_G, this->tmp_G_3);

    element_mul(tmp_GT, this->tmp_GT, this->tmp_GT_2);
    element_mul(tmp_GT, tmp_GT, this->tmp_GT_3);
    h->insertElement("h", "GT", tmp_GT);
}

bool IB_CH_MD_LSX_2022::Check(IB_CH_MD_LSX_2022_h h, IB_CH_MD_LSX_2022_r *r, element_t ID, element_t m, IB_CH_MD_LSX_2022_pp *pp){
    element_set(tmp_Zn, r->getElement("r1"));
    element_set(tmp_G, r->getElement("r2"));

    element_pow_zn(this->tmp_GT, pp->getElement("eg2g"), m);
    element_pow_zn(this->tmp_GT_2, pp->getElement("egg"), tmp_Zn);
    //g1 / g^ID
    element_pow_zn(this->tmp_G_2, pp->getElement("g"), ID);
    element_div(this->tmp_G_3, pp->getElement("g1"), this->tmp_G_2);
    element_pairing(this->tmp_GT_3, tmp_G, this->tmp_G_3);

    element_mul(tmp_GT, this->tmp_GT, this->tmp_GT_2);
    element_mul(tmp_GT, tmp_GT, this->tmp_GT_3);
    
    return element_cmp(h.getElement("h"), tmp_GT) == 0;
}

void IB_CH_MD_LSX_2022::Adapt(IB_CH_MD_LSX_2022_r *r_p, IB_CH_MD_LSX_2022_h *h, element_t m, IB_CH_MD_LSX_2022_r *r, element_t m_p, IB_CH_MD_LSX_2022_td *td) {
    element_sub(this->tmp_Zn, m, m_p);
    element_mul(this->tmp_Zn_2, this->tmp_Zn, td->getElement("td1"));
    element_add(tmp_Zn_2, r->getElement("r1"), this->tmp_Zn_2);
    r_p->insertElement("r1", "Zn", tmp_Zn_2);

    element_pow_zn(this->tmp_G, td->getElement("td2"), this->tmp_Zn);
    element_mul(tmp_G, r->getElement("r2"), this->tmp_G);
    r_p->insertElement("r2", "G1", tmp_G);
}

bool IB_CH_MD_LSX_2022::Verify(IB_CH_MD_LSX_2022_h h, IB_CH_MD_LSX_2022_r *r_p, element_t ID, element_t m_p, IB_CH_MD_LSX_2022_pp *pp) {
    return Check(h, r_p, ID, m_p, pp);
}

IB_CH_MD_LSX_2022::~IB_CH_MD_LSX_2022() {}