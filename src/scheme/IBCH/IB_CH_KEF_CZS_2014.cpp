#include <scheme/IBCH/IB_CH_KEF_CZS_2014.h>

IB_CH_KEF_CZS_2014::IB_CH_KEF_CZS_2014(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn):PbcScheme(_G1, _G2, _GT, _Zn) {
    element_init_same_as(this->x, Zn);
}

/**
 * input : 
 * output: x, Ppub
 */
void IB_CH_KEF_CZS_2014::SetUp(IB_CH_KEF_CZS_2014_pp *pp) {
    // P
    element_random(tmp_G);
    pp->insertElement("P", "G1", tmp_G);
    
    // x
    element_random(x);
    // Ppub = x * P
    element_mul_zn(tmp_G, tmp_G, x);
    pp->insertElement("Ppub", "G1", tmp_G);
}

/**
 * input : x, ID
 * output: SID
 */
void IB_CH_KEF_CZS_2014::Extract(IB_CH_KEF_CZS_2014_td *td, std::string ID) {  
    // QID = H(ID)
    this->H(tmp_H, ID);
    // SID = x * QID
    element_mul_zn(tmp_H, tmp_H, x);
    td->insertElement("SID", "G2", tmp_H);
}
 

/**
 * input : (y,h,m),(u11,u12,u2)
 * output: res
 */
void IB_CH_KEF_CZS_2014::H(element_t res, std::string m) {
    HASH::hash(res, m);
}

/**
 * input : ID, L, m
 * output: r(r1,r2), h
 */
void IB_CH_KEF_CZS_2014::Hash(IB_CH_KEF_CZS_2014_h *h, IB_CH_KEF_CZS_2014_r *r, std::string ID, std::string L, element_t m, IB_CH_KEF_CZS_2014_pp *pp) {
    // a
    element_random(tmp_Zn);
    // QID = H(ID)
    this->H(tmp_H, ID);

    // r1 = a * P
    element_mul_zn(tmp_G, pp->getElement("P"), tmp_Zn);
    r->insertElement("r1", "G1", tmp_G);
    // r2 = e(a * Ppub, QID)
    element_mul_zn(tmp_G, pp->getElement("Ppub"), tmp_Zn);
    element_pairing(tmp_GT, tmp_G, tmp_H);
    r->insertElement("r2", "GT", tmp_GT);

    // h = a * P + m * H(L)
    this->H(tmp_G, L);
    element_mul_zn(tmp_G, tmp_G, m);
    element_add(tmp_G, r->getElement("r1"), tmp_G);
    h->insertElement("h", "G1", tmp_G);
}

/**
 * input : h, L, m, r1
 * output: bool
 */
bool IB_CH_KEF_CZS_2014::Check(IB_CH_KEF_CZS_2014_h *h, IB_CH_KEF_CZS_2014_r *r, std::string L,element_t m, IB_CH_KEF_CZS_2014_td *td){
    // h = r1 + m * H(L)
    this->H(tmp_G, L);
    element_mul_zn(tmp_G, tmp_G, m);
    element_add(tmp_G, r->getElement("r1"), tmp_G);
    if(element_cmp(h->getElement("h"), tmp_G) != 0){
        return false;
    }

    // check the correctness of the r
    // e(a * P,SID) == e(a * Ppub, QID)
    // e(r1, SID) == r2
    element_pairing(this->tmp_GT, r->getElement("r1"), td->getElement("SID"));
    if(element_cmp(this->tmp_GT, r->getElement("r2")) != 0){
        return false;
    }
    return true;
}

/**
 * input : SID, ID, L, h, m, r1, r2, m_p
 * output: r_p(r1_p, r2_p)
 */
void IB_CH_KEF_CZS_2014::Adapt(IB_CH_KEF_CZS_2014_r *r_p, element_t m_p, IB_CH_KEF_CZS_2014_h *h, IB_CH_KEF_CZS_2014_r *r, std::string L, element_t m, IB_CH_KEF_CZS_2014_td *td) {
    // r1_p = r1 + (m - m_p) * H(L)
    element_sub(this->tmp_Zn, m, m_p);
    this->H(this->tmp_G, L);
    element_mul_zn(this->tmp_G_2, this->tmp_G, this->tmp_Zn);
    element_add(tmp_G_2, r->getElement("r1"), this->tmp_G_2);
    r_p->insertElement("r1", "G1", tmp_G_2);
    
    // r2_p = r2 * e(SID, H(L))^(m-m_p)
    element_pairing(this->tmp_GT, tmp_G, td->getElement("SID"));
    element_pow_zn(this->tmp_GT_2, this->tmp_GT, this->tmp_Zn);
    element_mul(tmp_GT_2, r->getElement("r2"), this->tmp_GT_2);
    r_p->insertElement("r2", "GT", tmp_GT_2);

    // check the correctness of the r_p
    // e(r1_p, SID) == r2_p
    element_pairing(this->tmp_GT_3, r_p->getElement("r1"), td->getElement("SID"));
    if(element_cmp(this->tmp_GT_3, r_p->getElement("r2")) != 0){
        throw std::runtime_error("IB_CH_KEF_CZS_2014::Adapt(): Adapt failed, r_p is invaid");
    }
}

/**
 * input : h, L, m_p, r1_p
 * output: bool
 */
bool IB_CH_KEF_CZS_2014::Verify(IB_CH_KEF_CZS_2014_h *h, IB_CH_KEF_CZS_2014_r *r, std::string L,element_t m, IB_CH_KEF_CZS_2014_td *td) {
    return this->Check(h, r, L, m, td);
}


IB_CH_KEF_CZS_2014::~IB_CH_KEF_CZS_2014() {
    element_clear(x);
}