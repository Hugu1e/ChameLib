#include <scheme/CH/FCR_CH_PreQA_DKS_2020.h>

FCR_CH_PreQA_DKS_2020::FCR_CH_PreQA_DKS_2020(int curve, int group) : PbcScheme(curve) {
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

    // xi,k11,k12,k2,e2,s2;
    element_init_same_as(this->xi, Zn);
    element_init_same_as(this->k11, Zn);
    element_init_same_as(this->k12, Zn);
    element_init_same_as(this->k2, Zn);
    element_init_same_as(this->u1, G1);
    element_init_same_as(this->u2, G1);
    element_init_same_as(this->e, Zn);

    element_init_same_as(tmp_G, G1);
    element_init_same_as(tmp_G_2, G1);
    element_init_same_as(tmp_G_3, G1);
    element_init_same_as(tmp_Zn, Zn);
    element_init_same_as(tmp_Zn_2, Zn);
}

void FCR_CH_PreQA_DKS_2020::SetUp(FCR_CH_PreQA_DKS_2020_pp &pp, FCR_CH_PreQA_DKS_2020_pk &pk, FCR_CH_PreQA_DKS_2020_sk &sk, FCR_CH_PreQA_DKS_2020_h &h, FCR_CH_PreQA_DKS_2020_r &r, FCR_CH_PreQA_DKS_2020_r &r_p) {
    pp.init(2);
    pk.init(1);
    sk.init(1);
    h.init(1);
    r.init(5);
    r_p.init(5);
    
    element_random(tmp_G);
    pp.set(g1, tmp_G);

    H2(tmp_G_2, tmp_G);
    pp.set(g2, tmp_G_2);
}

void FCR_CH_PreQA_DKS_2020::KeyGen(FCR_CH_PreQA_DKS_2020_pk &pk, FCR_CH_PreQA_DKS_2020_sk &sk, FCR_CH_PreQA_DKS_2020_pp &pp) {  
    // secret key x ∈ Zp
    element_random(tmp_Zn);
    sk.set(x, tmp_Zn);

    // public key y = g1^x
    element_pow_zn(tmp_G, pp[g1], tmp_Zn);
    pk.set(y, tmp_G);
}

void FCR_CH_PreQA_DKS_2020::H(element_t res, element_t y, element_t h, element_t m, element_t u1,element_t u2) {
    HASH::hash(res, y, h, m, u1, u2);
}

void FCR_CH_PreQA_DKS_2020::H2(element_t res, element_t m){
    HASH::hash(res, m);
}

void FCR_CH_PreQA_DKS_2020::Hash(FCR_CH_PreQA_DKS_2020_h &h, FCR_CH_PreQA_DKS_2020_r &r, element_t m, FCR_CH_PreQA_DKS_2020_pk &pk, FCR_CH_PreQA_DKS_2020_pp &pp) {
    element_init_same_as(r[e2], Zn);
    element_init_same_as(r[s2], Zn);
    element_init_same_as(r[s11], Zn);
    element_init_same_as(r[s12], Zn);
    element_init_same_as(r[e1], Zn);
    
    element_random(this->xi);
    element_random(this->k11);
    element_random(this->k12);
    element_random(this->k2);
    element_random(r[e2]);
    element_random(r[s2]);
    // u1 = (g1^k11) *(g2^k12)
    element_pow_zn(this->tmp_G, pp[g1], this->k11); 
    element_pow_zn(this->tmp_G_2, pp[g2], this->k12);
    element_mul(this->u1, this->tmp_G, this->tmp_G_2);
    // u2 = (g1^s2) *(y^-e2)   
    element_pow_zn(this->tmp_G, pp[g1], r[s2]); 
    element_pow_zn(this->tmp_G_2, pk[y], r[e2]);
    element_div(this->u2, this->tmp_G, this->tmp_G_2);
    // h = (g1^m) * (g2^xi)
    element_pow_zn(this->tmp_G, pp[g1], m);
    element_pow_zn(this->tmp_G_2, pp[g2], this->xi);
    element_mul(tmp_G, this->tmp_G, this->tmp_G_2);
    h.set(h1, tmp_G);
    
    
    // e = H((y,h,m),(u1,u2))
    this->H(this->e, pk[y], tmp_G, m, this->u1, this->u2);
    // e1 = e - e2
    element_sub(r[e1], this->e, r[e2]);
    // s11 = k11 + e1 * m
    element_mul(this->tmp_Zn, r[e1], m);
    element_add(r[s11], this->k11, this->tmp_Zn);
    // s12 = k12 + e1 * xi
    element_mul(this->tmp_Zn, r[e1], this->xi);
    element_add(r[s12], this->k12, this->tmp_Zn);
}

bool FCR_CH_PreQA_DKS_2020::Check(FCR_CH_PreQA_DKS_2020_h &h, FCR_CH_PreQA_DKS_2020_r &r, element_t m, FCR_CH_PreQA_DKS_2020_pk &pk, FCR_CH_PreQA_DKS_2020_pp &pp) {
    // e1 + e2
    element_add(this->tmp_Zn, r[e1], r[e2]);
    
    // (g1^s11)*(g2^s12)*(h^-e1)
    element_pow_zn(this->tmp_G, pp[g1], r[s11]);
    element_pow_zn(this->tmp_G_2, pp[g2], r[s12]);
    element_pow_zn(this->tmp_G_3, h[h1], r[e1]);
    element_mul(this->tmp_G, this->tmp_G, this->tmp_G_2);
    element_div(this->tmp_G, this->tmp_G, this->tmp_G_3);
    
    // (g1^s2)*(y^-e2)
    element_pow_zn(this->tmp_G_2, pp[g1], r[s2]);
    element_pow_zn(this->tmp_G_3, pk[y], r[e2]);
    element_div(this->tmp_G_2, this->tmp_G_2, this->tmp_G_3);

    // H((y,h,m),(tmp_G1, tmp_G1_2))
    this->H(this->tmp_Zn_2, pk[y], h[h1], m, this->tmp_G, this->tmp_G_2);

    return element_cmp(this->tmp_Zn, this->tmp_Zn_2) == 0;
}

void FCR_CH_PreQA_DKS_2020::Adapt(FCR_CH_PreQA_DKS_2020_r &r_p, element_t m_p, element_t m, FCR_CH_PreQA_DKS_2020_h &h, FCR_CH_PreQA_DKS_2020_r &r, FCR_CH_PreQA_DKS_2020_sk &sk, FCR_CH_PreQA_DKS_2020_pp &pp) {
    FCR_CH_PreQA_DKS_2020_pk tmp_pk;
    tmp_pk.init(1);
    // y = g1^x
    element_pow_zn(tmp_G, pp[g1], sk[x]);
    tmp_pk.set(y, tmp_G);
    // Check
    if(!this->Check(h, r, m, tmp_pk, pp)){
        printf("Forge(): Check failed !!\n");
        throw std::runtime_error("FCR_CH_PreQA_DKS_2020::Adapt(): Hash Check failed");
    }

    element_init_same_as(r_p[e1], Zn);
    element_init_same_as(r_p[e2], Zn);
    element_init_same_as(r_p[s11], Zn);
    element_init_same_as(r_p[s12], Zn);
    element_init_same_as(r_p[s2], Zn);
    
    element_random(this->k11);
    element_random(this->k12);
    element_random(r_p[e1]);
    element_random(r_p[s11]);
    element_random(r_p[s12]);
    // u1 = (g1^s11) *(g2^s12) * (h^-e1)
    element_pow_zn(this->tmp_G, pp[g1], r_p[s11]);
    element_pow_zn(this->tmp_G_2, pp[g2], r_p[s12]);
    element_pow_zn(this->tmp_G_3, h[h1], r_p[e1]);
    element_mul(this->u1, this->tmp_G, this->tmp_G_2);
    element_div(this->u1, this->u1, this->tmp_G_3);
    // ? u2 = (g1^k12)
    element_pow_zn(this->u2, pp[g1], this->k12);
    // e = H((y,h,m_p),(u1,u2))
    this->H(this->e, tmp_pk[y], h[h1], m_p, this->u1, this->u2);
    // e2 = e - e1
    element_sub(r_p[e2], this->e, r_p[e1]);
    // s2 = k12 + e2 * x
    element_mul(this->tmp_Zn, r_p[e2], sk[x]);
    element_add(r_p[s2], this->k12, this->tmp_Zn);
}

bool FCR_CH_PreQA_DKS_2020::Verify(FCR_CH_PreQA_DKS_2020_h &h, FCR_CH_PreQA_DKS_2020_r &r_p, element_t m_p, FCR_CH_PreQA_DKS_2020_pk &pk, FCR_CH_PreQA_DKS_2020_pp &pp) {
    return this->Check(h, r_p, m_p, pk, pp);
}

FCR_CH_PreQA_DKS_2020::~FCR_CH_PreQA_DKS_2020() {
    element_clear(this->xi);
    element_clear(this->k11);
    element_clear(this->k12);
    element_clear(this->k2);
    element_clear(this->u1);
    element_clear(this->u2);
    element_clear(this->e);

    element_clear(tmp_G);
    element_clear(tmp_G_2);
    element_clear(tmp_G_3);
    element_clear(tmp_Zn);
    element_clear(tmp_Zn_2);
    
    element_clear(G1);
    element_clear(Zn);
}
