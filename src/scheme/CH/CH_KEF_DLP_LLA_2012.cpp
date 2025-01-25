#include <scheme/CH/CH_KEF_DLP_LLA_2012.h>

CH_KEF_DLP_LLA_2012::CH_KEF_DLP_LLA_2012(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn): PbcScheme(_G1, _G2, _GT, _Zn) {
    element_init_same_as(this->y, G1);
    element_init_same_as(this->y1, G1);
    element_init_same_as(this->w1, G1);
    element_init_same_as(this->t, G1);
}

/**
 * input : 
 * output: 
 */
void CH_KEF_DLP_LLA_2012::SetUp(CH_KEF_DLP_LLA_2012_pp *pp) {
    element_random(tmp_G);
    pp->insertElement("g","G1",tmp_G);
}

/**
 * input : 
 * output: sk, pk, label
 */
void CH_KEF_DLP_LLA_2012::KeyGen(CH_KEF_DLP_LLA_2012_sk *sk, CH_KEF_DLP_LLA_2012_pk *pk, CH_KEF_DLP_LLA_2012_label *label, CH_KEF_DLP_LLA_2012_pp *pp) {  
    element_random(tmp_Zn);
    sk->insertElement("a","Zn",tmp_Zn);
    element_random(tmp_Zn_2);
    sk->insertElement("x1","Zn",tmp_Zn_2);
    element_random(tmp_Zn_3);
    sk->insertElement("x2","Zn",tmp_Zn_3);

    // y = g^a
    element_pow_zn(this->y, pp->getElement("g"), tmp_Zn);

    // y1 = g^x1
    element_pow_zn(this->y1, pp->getElement("g"), tmp_Zn_2);
    // y2 = g^x2
    element_pow_zn(tmp_G, pp->getElement("g"), tmp_Zn_3);
    pk->insertElement("y2","G1", tmp_G);
    // w1 = y^x1
    element_pow_zn(this->w1, this->y, tmp_Zn_2);

    // obtain a label
    element_random(this->t);
    LabelManager(label, this->y1, this->w1, this->t);
}

/**
 * input: y1, w1, t
 * output: label
 */
void CH_KEF_DLP_LLA_2012::LabelManager(CH_KEF_DLP_LLA_2012_label *label, element_t y1, element_t w1, element_t t) {
    // H2(t)
    this->H2(this->tmp_Zn, t);
    // L = y1^H2(t)
    element_pow_zn(tmp_G, y1, this->tmp_Zn);
    label->insertElement("L","G1",tmp_G);

    // R = t*(w1^H2(t))
    element_pow_zn(this->tmp_G, w1, this->tmp_Zn);
    element_mul(tmp_G, t, this->tmp_G);
    label->insertElement("R","G1",tmp_G);
}

/**
 * input : m1,m2,m3
 * output: res
 */
void CH_KEF_DLP_LLA_2012::H1(element_t res, element_t m1, element_t m2, element_t m3) {
    HASH::hash(res, m1, m2, m3);
}

/**
 * input : m
 * output: res
 */
void CH_KEF_DLP_LLA_2012::H2(element_t res, element_t m) {
    HASH::hash(res, m);
}

/**
 * input : pk, m, r, lable
 * output: S
 */
void CH_KEF_DLP_LLA_2012::Hash(element_t h, element_t r, CH_KEF_DLP_LLA_2012_pk *pk, element_t m, CH_KEF_DLP_LLA_2012_label *label, CH_KEF_DLP_LLA_2012_pp *pp) {
    element_random(r);
    
    // c = H1(label, L)
    this->H1(this->tmp_Zn, label->getElement("L"), label->getElement("R"), label->getElement("L"));
    // y2^c
    element_pow_zn(this->tmp_G, pk->getElement("y2"), this->tmp_Zn);
    // L * (y2^c)
    element_mul(this->tmp_G, label->getElement("L"), this->tmp_G);
    // (L * (y2^c)) ^ r
    element_pow_zn(this->tmp_G, this->tmp_G, r);
    // g^m
    element_pow_zn(this->tmp_G_2, pp->getElement("g"), m);
    // S = (g^m) * ((L * (y2^c)) ^ r)
    element_mul(h, this->tmp_G_2, this->tmp_G);   
}

/**
 * input : m, r, pk, lable, S
 * output: bool
 */
bool CH_KEF_DLP_LLA_2012::Check(element_t m, element_t r, CH_KEF_DLP_LLA_2012_pk *pk, CH_KEF_DLP_LLA_2012_label *label, element_t h, CH_KEF_DLP_LLA_2012_pp *pp) {
    // c = H1(label, L)
    this->H1(this->tmp_Zn, label->getElement("L"), label->getElement("R"), label->getElement("L"));
    // y2^c
    element_pow_zn(this->tmp_G, pk->getElement("y2"), this->tmp_Zn);
    // L * (y2^c)
    element_mul(this->tmp_G, label->getElement("L"), this->tmp_G);
    // (L * (y2^c)) ^ r
    element_pow_zn(this->tmp_G, this->tmp_G, r);
    // g^m
    element_pow_zn(this->tmp_G_2, pp->getElement("g"), m);
    // S = (g^m) * ((L * (y2^c)) ^ r)
    element_mul(this->tmp_G, this->tmp_G_2, this->tmp_G);   
    // compare
    return element_cmp(h, this->tmp_G) == 0;
}

/**
 * input: sk,pk,label, h, m, m_p, r
 * output: r_p
 */
void CH_KEF_DLP_LLA_2012::UForge(element_t r_p, CH_KEF_DLP_LLA_2012_sk *sk, CH_KEF_DLP_LLA_2012_pk *pk, CH_KEF_DLP_LLA_2012_label *label, element_t h, element_t m, element_t m_p, element_t r, CH_KEF_DLP_LLA_2012_pp *pp) {
    // check
    if(!this->Check(m, r, pk, label, h, pp)){ 
        throw std::invalid_argument("UForge failed, h is not correct");
    }
    // t = R / (L^a)
    element_pow_zn(this->tmp_G, label->getElement("L"), sk->getElement("a"));
    element_div(this->t, label->getElement("R"), this->tmp_G);
    
    // TODO
    // check if y1^H2(t) = L
    this->H2(this->tmp_Zn, this->t);
    element_pow_zn(this->tmp_G, this->y1, this->tmp_Zn);

    if(element_cmp(this->tmp_G, label->getElement("L")) != 0){
        throw std::invalid_argument("UForge failed, label is not correct");
    }

    // c = H1(label, L)
    this->H1(this->tmp_Zn_2, label->getElement("L"), label->getElement("R"), label->getElement("L"));
    // s = x1 *H2(t) + x2 *c
    element_mul(this->tmp_Zn, sk->getElement("x1"), this->tmp_Zn);
    element_mul(this->tmp_Zn_2, sk->getElement("x2"), this->tmp_Zn_2);
    element_add(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    
    // r_p = s^-1(m-m_p) + r
    element_sub(this->tmp_Zn_2, m, m_p);
    element_invert(this->tmp_Zn, this->tmp_Zn);
    element_mul(this->tmp_Zn_2, this->tmp_Zn, this->tmp_Zn_2);
    element_add(r_p, this->tmp_Zn_2, r);
}

/**
 * input: label, m, m_p, r,r_p, m_pp
 * output: r_pp
 */
void CH_KEF_DLP_LLA_2012::IForge(element_t r_pp, CH_KEF_DLP_LLA_2012_label *label, element_t m, element_t m_p, element_t r, element_t r_p, element_t m_pp){
    // s = (m-m')/(r'-r)
    element_sub(this->tmp_Zn, m, m_p);
    element_sub(this->tmp_Zn_2, r_p, r);
    element_div(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);

    // r'' = (s^-1)(m'-m'') + r'
    element_sub(this->tmp_Zn_2, m_p, m_pp);
    element_invert(this->tmp_Zn, this->tmp_Zn);
    element_mul(this->tmp_Zn_2, this->tmp_Zn, this->tmp_Zn_2);
    element_add(r_pp, this->tmp_Zn_2, r_p);
}

/**
 * input : m_p, r_p, pk, label, S
 * output: bool
 */
bool CH_KEF_DLP_LLA_2012::Verify(element_t m_p, element_t r_p, CH_KEF_DLP_LLA_2012_pk *pk, CH_KEF_DLP_LLA_2012_label *label, element_t h, CH_KEF_DLP_LLA_2012_pp *pp) {
    return this->Check(m_p, r_p, pk, label, h, pp);
}

CH_KEF_DLP_LLA_2012::~CH_KEF_DLP_LLA_2012() {
    element_clear(this->y);
    element_clear(this->y1);
    element_clear(this->w1);
    element_clear(this->t);
}