#include <scheme/CH/CR_CH_DSS_2020.h>

CR_CH_DSS_2020::CR_CH_DSS_2020(int curve, int group):PbcScheme(curve) {
    switch(group){
        case Group::G1:
            element_init_G1(G1, pairing);
            break;
        case Group::G2:
            element_init_G2(G1, pairing);
            break;
        case Group::GT:
            element_init_GT(G1, pairing);
            break;
        default:
            throw CurveException(CurveException::INVALID_GROUP);
    }
    element_init_Zr(Zn, pairing);

    element_init_same_as(this->xi, Zn);
    element_init_same_as(this->k1, Zn);
    element_init_same_as(this->u11, G1);
    element_init_same_as(this->u12, G1);
    element_init_same_as(this->u2, G1);
    element_init_same_as(this->e, Zn);
    element_init_same_as(this->k2, Zn);

    element_init_same_as(tmp_G, G1);
    element_init_same_as(tmp_G_2, G1);
    element_init_same_as(tmp_G_3, G1);
    element_init_same_as(tmp_G_4, G1);
    element_init_same_as(tmp_Zn, Zn);
    element_init_same_as(tmp_Zn_2, Zn);
}

/**
 * input : 
 * output: 
 */
void CR_CH_DSS_2020::SetUp(CR_CH_DSS_2020_pp &pp, CR_CH_DSS_2020_pk &pk, CR_CH_DSS_2020_sk &sk, CR_CH_DSS_2020_h &h, CR_CH_DSS_2020_r &r, CR_CH_DSS_2020_r &r_p) {
    pp.init(1);
    pk.init(1);
    sk.init(1);
    h.init(2);
    r.init(4);
    r_p.init(4);
    
    element_random(tmp_G);
    pp.set(g, tmp_G);
}

/**
 * input : 
 * output: x, y
 */
void CR_CH_DSS_2020::KeyGen(CR_CH_DSS_2020_pk &pk, CR_CH_DSS_2020_sk &sk, CR_CH_DSS_2020_pp &pp){  
    // secret key x ∈ Zp
    element_random(tmp_Zn);
    sk.set(x, tmp_Zn);
    
    // public key y = g^x
    element_pow_zn(tmp_G, pp[g], tmp_Zn);
    pk.set(y, tmp_G);
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
void CR_CH_DSS_2020::Hash(CR_CH_DSS_2020_h &h, CR_CH_DSS_2020_r &r,  element_t m, CR_CH_DSS_2020_pk &pk, CR_CH_DSS_2020_pp &pp) {
    element_init_same_as(r[e1], Zn);
    element_init_same_as(r[e2], Zn);
    element_init_same_as(r[s1], Zn);
    element_init_same_as(r[s2], Zn);
    element_random(this->xi);
    element_random(this->k1);
    element_random(r[e2]);
    element_random(r[s2]);

    // u11 = g^k1
    element_pow_zn(this->u11, pp[g], this->k1);
    // u12 = pk^k1
    element_pow_zn(this->u12, pk[y], this->k1);
    // u2 = g^s2 * pk^-e2
    element_pow_zn(this->tmp_G, pp[g], r[s2]);
    element_pow_zn(this->tmp_G_2, pk[y], r[e2]);
    element_div(this->u2, this->tmp_G, this->tmp_G_2);
    // h1 = g^xi
    element_pow_zn(tmp_G_3, pp[g], this->xi);
    h.set(h1, tmp_G_3);
    
    // h2 = m * pk^xi
    element_pow_zn(this->tmp_G, pk[y], this->xi);
    element_mul(tmp_G_4, m, this->tmp_G);
    h.set(h2, tmp_G_4);

    // e = H((y,h,m),(u11,u12,u2))
    this->H(this->e, pk[y], tmp_G_3, tmp_G_4, m, this->u11, this->u12, this->u2);
    // e1 = e - e2
    element_sub(r[e1], this->e, r[e2]);
    // s1 = k1 + e1 * xi
    element_mul(this->tmp_Zn, r[e1], this->xi);
    element_add(r[s1], this->k1, this->tmp_Zn);
}

/**
 * input : pk(y), m, h(h1,h2), r(e1,e2,s1,s2)
 * output: bool
 */
bool CR_CH_DSS_2020::Check(CR_CH_DSS_2020_h &h, CR_CH_DSS_2020_r &r,  element_t m, CR_CH_DSS_2020_pk &pk, CR_CH_DSS_2020_pp &pp){
    // e1 + e2
    element_add(this->tmp_Zn, r[e1], r[e2]);
    
    // (g^s1)*(c1^-e1)
    element_pow_zn(this->tmp_G, pp[g], r[s1]);
    element_pow_zn(this->tmp_G_2, h[h1], r[e1]);
    element_div(this->tmp_G, this->tmp_G, this->tmp_G_2);
    
    // (y^s1) * ((c2/m)^-e1)
    element_pow_zn(this->tmp_G_2, pk[y], r[s1]);
        // c2/m
    element_div(this->tmp_G_3, h[h2], m);
    element_pow_zn(this->tmp_G_4, this->tmp_G_3, r[e1]);
    element_div(this->tmp_G_2, this->tmp_G_2, this->tmp_G_4);

    // g^s2 * y^-e2
    element_pow_zn(this->tmp_G_3, pp[g], r[s2]);
    element_pow_zn(this->tmp_G_4, pk[y], r[e2]);
    element_div(this->tmp_G_3, this->tmp_G_3, this->tmp_G_4);

    // H((y,h,m),(tmp_G1, tmp_G1_2, tmp_G1_3))
    this->H(this->tmp_Zn_2, pk[y], h[h1], h[h2], m, this->tmp_G, this->tmp_G_2, this->tmp_G_3);

    // element_printf("Check(): tmp_Zn = %B\n", this->tmp_Zn);
    // element_printf("Check(): tmp_Zn_2 = %B\n", this->tmp_Zn_2);
    return element_cmp(this->tmp_Zn, this->tmp_Zn_2) == 0;
}

/**
 * input : x , m,  m_p, r(e1,e2,s1,s2), h(h1,h2)
 * output: r_p(e1_p,e2_p,s1_p,s2_p)
 */
void CR_CH_DSS_2020::Adapt(CR_CH_DSS_2020_r &r_p, CR_CH_DSS_2020_h &h, CR_CH_DSS_2020_r &r, element_t m, element_t m_p, CR_CH_DSS_2020_sk &sk, CR_CH_DSS_2020_pk &pk, CR_CH_DSS_2020_pp &pp) {
    if(!this->Check(h, r, m, pk, pp)){
        printf("Forge(): Check failed !!\n");
        return;
    }

    element_init_same_as(r_p[e1], Zn);
    element_init_same_as(r_p[e2], Zn);
    element_init_same_as(r_p[s1], Zn);
    element_init_same_as(r_p[s2], Zn);

    element_random(this->k2);
    element_random(r_p[e1]);
    element_random(r_p[s1]);

    // u11 = g^s1 * c1^-e1
    element_pow_zn(this->tmp_G, pp[g], r_p[s1]);
    element_pow_zn(this->tmp_G_2, h[h1], r_p[e1]);
    element_div(this->u11, this->tmp_G, this->tmp_G_2);

    // u12 = y^s1 * (c2/m')^-e1
    element_pow_zn(this->tmp_G, pk[y], r_p[s1]);
    element_div(this->tmp_G_2, h[h2], m_p);
    element_pow_zn(this->tmp_G_3, this->tmp_G_2, r_p[e1]);
    element_div(this->u12, this->tmp_G, this->tmp_G_3);

    // u2 = g^k2
    element_pow_zn(this->u2, pp[g], this->k2);

    // e = H((y,h,m'),(u11,u12,u2))
    this->H(this->e, pk[y], h[h1], h[h2], m_p, this->u11, this->u12, this->u2);

    // e2 = e - e1
    element_sub(r_p[e2], this->e, r_p[e1]);

    // s2 = k2 + e2 * x
    element_mul(this->tmp_Zn, r_p[e2], sk[x]);
    element_add(r_p[s2], this->k2, this->tmp_Zn);
}

/**
 * input : y, m_p,r_p, h
 * output: bool
 */
bool CR_CH_DSS_2020::Verify(CR_CH_DSS_2020_h &h, CR_CH_DSS_2020_r &r_p,  element_t m_p, CR_CH_DSS_2020_pk &pk, CR_CH_DSS_2020_pp &pp) {
    return this->Check(h, r_p, m_p, pk, pp);
}


CR_CH_DSS_2020::~CR_CH_DSS_2020() {
    element_clear(this->xi);
    element_clear(this->k1);
    element_clear(this->u11);
    element_clear(this->u12);
    element_clear(this->u2);
    element_clear(this->e);
    element_clear(this->k2);

    element_clear(tmp_G);
    element_clear(tmp_G_2);
    element_clear(tmp_G_3);
    element_clear(tmp_G_4);
    element_clear(tmp_Zn);
    element_clear(tmp_Zn_2);

    element_clear(G1);
    element_clear(Zn);
}