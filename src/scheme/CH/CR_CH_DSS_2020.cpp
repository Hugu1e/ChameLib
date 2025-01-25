#include <scheme/CH/CR_CH_DSS_2020.h>

CR_CH_DSS_2020::CR_CH_DSS_2020(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn):PbcScheme(_G1, _G2, _GT, _Zn){
    element_init_same_as(this->xi, Zn);
    element_init_same_as(this->k1, Zn);
    element_init_same_as(this->e2, Zn);
    element_init_same_as(this->s2, Zn);

    element_init_same_as(this->u11, G1);
    element_init_same_as(this->u12, G1);
    element_init_same_as(this->u2, G1);
    element_init_same_as(this->e, Zn);
    element_init_same_as(this->e1, Zn);
    element_init_same_as(this->s1, Zn);
    element_init_same_as(this->k2, Zn);
}

/**
 * input : 
 * output: 
 */
void CR_CH_DSS_2020::SetUp(CR_CH_DSS_2020_pp *pp) {
    element_random(tmp_G);
    pp->insertElement("g", "G1", tmp_G);
}

/**
 * input : 
 * output: x, y
 */
void CR_CH_DSS_2020::KeyGen(CR_CH_DSS_2020_pk *pk, CR_CH_DSS_2020_sk *sk, CR_CH_DSS_2020_pp *pp){  
    // secret key x ∈ Zp
    element_random(tmp_Zn);
    sk->insertElement("x", "Zn", tmp_Zn);
    
    // public key y = g^x
    element_pow_zn(tmp_G, pp->getElement("g"), tmp_Zn);
    pk->insertElement("y", "G1", tmp_G);
}


/**
 * input : (y,h,m),(u11,u12,u2)
 * output: res
 */
void CR_CH_DSS_2020::H(element_t res, element_t y, element_t h1, element_t h2, element_t m, element_t u11,element_t u12,element_t u2) {
    HASH::hash(res, y, h1, h2, m, u11, u12, u2);
}


/**
 * input : pk(y), m
 * output: h(h1,h2), pai(e1,e2,s11,s12,s2)
 */
void CR_CH_DSS_2020::Hash(CR_CH_DSS_2020_h *h, CR_CH_DSS_2020_r *r,  element_t m, CR_CH_DSS_2020_pk *pk, CR_CH_DSS_2020_pp *pp) {
    element_random(this->xi);
    element_random(this->k1);
    element_random(this->e2);
    element_random(this->s2);

    // u11 = g^k1
    element_pow_zn(this->u11, pp->getElement("g"), this->k1);
    // u12 = pk^k1
    element_pow_zn(this->u12, pk->getElement("y"), this->k1);
    // u2 = g^s2 * pk^-e2
    element_pow_zn(this->tmp_G, pp->getElement("g"), this->s2);
    element_pow_zn(this->tmp_G_2, pk->getElement("y"), this->e2);
    element_div(this->u2, this->tmp_G, this->tmp_G_2);
    // h1 = g^xi
    element_pow_zn(tmp_G_3, pp->getElement("g"), this->xi);
    h->insertElement("h1","G1", tmp_G_3);
    
    // h2 = m * pk^xi
    element_pow_zn(this->tmp_G, pk->getElement("y"), this->xi);
    element_mul(tmp_G_4, m, this->tmp_G);
    h->insertElement("h2", "G1", tmp_G_4);

    // e = H((y,h,m),(u11,u12,u2))
    this->H(this->e, pk->getElement("y"), tmp_G_3, tmp_G_4, m, this->u11, this->u12, this->u2);
    // e1 = e - e2
    element_sub(this->e1, this->e, this->e2);
    // s1 = k1 + e1 * xi
    element_mul(this->tmp_Zn, this->e1, this->xi);
    element_add(this->s1, this->k1, this->tmp_Zn);

    // return pai(e1,e2,s1,s2)
    r->insertElement("e1", "Zn", this->e1);
    r->insertElement("e2", "Zn", this->e2);
    r->insertElement("s1", "Zn", this->s1);
    r->insertElement("s2", "Zn", this->s2);   
}

/**
 * input : pk(y), m, h(h1,h2), r(e1,e2,s1,s2)
 * output: bool
 */
bool CR_CH_DSS_2020::Check(CR_CH_DSS_2020_h *h, CR_CH_DSS_2020_r *r,  element_t m, CR_CH_DSS_2020_pk *pk, CR_CH_DSS_2020_pp *pp){
    // e1 + e2
    element_add(this->tmp_Zn, r->getElement("e1"), r->getElement("e2"));
    
    // (g^s1)*(c1^-e1)
    element_pow_zn(this->tmp_G, pp->getElement("g"), r->getElement("s1"));
    element_pow_zn(this->tmp_G_2, h->getElement("h1"), r->getElement("e1"));
    element_div(this->tmp_G, this->tmp_G, this->tmp_G_2);
    
    // (y^s1) * ((c2/m)^-e1)
    element_pow_zn(this->tmp_G_2, pk->getElement("y"), r->getElement("s1"));
        // c2/m
    element_div(this->tmp_G_3, h->getElement("h2"), m);
    element_pow_zn(this->tmp_G_4, this->tmp_G_3, r->getElement("e1"));
    element_div(this->tmp_G_2, this->tmp_G_2, this->tmp_G_4);

    // g^s2 * y^-e2
    element_pow_zn(this->tmp_G_3, pp->getElement("g"), r->getElement("s2"));
    element_pow_zn(this->tmp_G_4, pk->getElement("y"), r->getElement("e2"));
    element_div(this->tmp_G_3, this->tmp_G_3, this->tmp_G_4);

    // H((y,h,m),(tmp_G1, tmp_G1_2, tmp_G1_3))
    this->H(this->tmp_Zn_2, pk->getElement("y"), h->getElement("h1"),h->getElement("h2"),m, this->tmp_G, this->tmp_G_2, this->tmp_G_3);

    // element_printf("Check(): tmp_Zn = %B\n", this->tmp_Zn);
    // element_printf("Check(): tmp_Zn_2 = %B\n", this->tmp_Zn_2);
    return element_cmp(this->tmp_Zn, this->tmp_Zn_2) == 0;
}

/**
 * input : x , m,  m_p, r(e1,e2,s1,s2), h(h1,h2)
 * output: r_p(e1_p,e2_p,s1_p,s2_p)
 */
void CR_CH_DSS_2020::Adapt(CR_CH_DSS_2020_r *r_p, CR_CH_DSS_2020_h *h, CR_CH_DSS_2020_r *r, element_t m, element_t m_p, CR_CH_DSS_2020_sk *sk, CR_CH_DSS_2020_pk *pk, CR_CH_DSS_2020_pp *pp) {
    if(!this->Check(h, r, m, pk, pp)){
        printf("Forge(): Check failed !!\n");
        return;
    }

    element_random(this->k2);
    element_random(this->e1);
    element_random(this->s1);

    // u11 = g^s1 * c1^-e1
    element_pow_zn(this->tmp_G, pp->getElement("g"), this->s1);
    element_pow_zn(this->tmp_G_2, h->getElement("h1"), this->e1);
    element_div(this->u11, this->tmp_G, this->tmp_G_2);

    // u12 = y^s1 * (c2/m')^-e1
    element_pow_zn(this->tmp_G, pk->getElement("y"), this->s1);
    element_div(this->tmp_G_2, h->getElement("h2"), m_p);
    element_pow_zn(this->tmp_G_3, this->tmp_G_2, this->e1);
    element_div(this->u12, this->tmp_G, this->tmp_G_3);

    // u2 = g^k2
    element_pow_zn(this->u2, pp->getElement("g"), this->k2);

    // e = H((y,h,m'),(u11,u12,u2))
    this->H(this->e, pk->getElement("y"), h->getElement("h1"), h->getElement("h2"), m_p, this->u11, this->u12, this->u2);

    // e2 = e - e1
    element_sub(this->e2, this->e, this->e1);

    // s2 = k2 + e2 * x
    element_mul(this->tmp_Zn, this->e2, sk->getElement("x"));
    element_add(this->s2, this->k2, this->tmp_Zn);

    // return pai'(e1,e2,s1,s2)
    r_p->insertElement("e1", "Zn", this->e1);
    r_p->insertElement("e2", "Zn", this->e2);
    r_p->insertElement("s1", "Zn", this->s1);
    r_p->insertElement("s2", "Zn", this->s2);
}

/**
 * input : y, m_p,r_p, h
 * output: bool
 */
bool CR_CH_DSS_2020::Verify(CR_CH_DSS_2020_h *h, CR_CH_DSS_2020_r *r_p,  element_t m_p, CR_CH_DSS_2020_pk *pk, CR_CH_DSS_2020_pp *pp) {
    return this->Check(h, r_p, m_p, pk, pp);
}


CR_CH_DSS_2020::~CR_CH_DSS_2020() {
    element_clear(this->xi);
    element_clear(this->k1);
    element_clear(this->e2);
    element_clear(this->s2);

    element_clear(this->u11);
    element_clear(this->u12);
    element_clear(this->u2);
    element_clear(this->e);
    element_clear(this->e1);
    element_clear(this->s1);
    element_clear(this->k2);
}