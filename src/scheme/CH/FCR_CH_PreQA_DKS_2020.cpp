#include <scheme/CH/FCR_CH_PreQA_DKS_2020.h>

FCR_CH_PreQA_DKS_2020::FCR_CH_PreQA_DKS_2020(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn) : PbcScheme(_G1, _G2, _GT, _Zn) {
    // xi,k11,k12,k2,e2,s2;
    element_init_same_as(this->xi, Zn);
    element_init_same_as(this->k11, Zn);
    element_init_same_as(this->k12, Zn);
    element_init_same_as(this->k2, Zn);
    element_init_same_as(this->e2, Zn);
    element_init_same_as(this->s2, Zn);

    element_init_same_as(this->u1, G1);
    element_init_same_as(this->u2, G1);

    element_init_same_as(this->e, Zn);
    element_init_same_as(this->e1, Zn);
    element_init_same_as(this->s11, Zn);
    element_init_same_as(this->s12, Zn);
}

void FCR_CH_PreQA_DKS_2020::SetUp(FCR_CH_PreQA_DKS_2020_pp *pp) {
    element_random(tmp_G);
    pp->insertElement("g1", "G1", tmp_G);

    element_random(tmp_G);
    pp->insertElement("g2", "G1", tmp_G);
}

void FCR_CH_PreQA_DKS_2020::KeyGen(FCR_CH_PreQA_DKS_2020_pk *pk, FCR_CH_PreQA_DKS_2020_sk *sk, FCR_CH_PreQA_DKS_2020_pp *pp) {  
    // secret key x ∈ Zp
    element_random(tmp_Zn);
    sk->insertElement("x", "Zn", tmp_Zn);

    // public key y = g1^x
    element_pow_zn(tmp_G, pp->getElement("g1"), tmp_Zn);
    pk->insertElement("y", "G1", tmp_G);
}

void FCR_CH_PreQA_DKS_2020::H(element_t res, element_t y, element_t h, element_t m, element_t u1,element_t u2) {
    HASH::hash(res, y, h, m, u1, u2);
}

void FCR_CH_PreQA_DKS_2020::Hash(FCR_CH_PreQA_DKS_2020_h *h, FCR_CH_PreQA_DKS_2020_r *r, element_t m, FCR_CH_PreQA_DKS_2020_pk *pk, FCR_CH_PreQA_DKS_2020_pp *pp) {
    element_random(this->xi);
    element_random(this->k11);
    element_random(this->k12);
    element_random(this->k2);
    element_random(this->e2);
    element_random(this->s2);
    // u1 = (g1^k11) *(g2^k12)
    element_pow_zn(this->tmp_G, pp->getElement("g1"), this->k11); 
    element_pow_zn(this->tmp_G_2, pp->getElement("g2"), this->k12);
    element_mul(this->u1, this->tmp_G, this->tmp_G_2);
    // u2 = (g1^s2) *(y^-e2)   
    element_pow_zn(this->tmp_G, pp->getElement("g1"), this->s2); 
    element_pow_zn(this->tmp_G_2, pk->getElement("y"), this->e2);
    element_div(this->u2, this->tmp_G, this->tmp_G_2);
    // h = (g1^m) * (g2^xi)
    element_pow_zn(this->tmp_G, pp->getElement("g1"), m);
    element_pow_zn(this->tmp_G_2, pp->getElement("g2"), this->xi);
    element_mul(tmp_G, this->tmp_G, this->tmp_G_2);
    h->insertElement("h", "G1", tmp_G);
    
    
    // e = H((y,h,m),(u1,u2))
    this->H(this->e, pk->getElement("y"), tmp_G, m, this->u1, this->u2);
    // e1 = e - e2
    element_sub(this->e1, this->e, this->e2);
    // s11 = k11 + e1 * m
    element_mul(this->tmp_Zn, this->e1, m);
    element_add(this->s11, this->k11, this->tmp_Zn);
    // s12 = k12 + e1 * xi
    element_mul(this->tmp_Zn, this->e1, this->xi);
    element_add(this->s12, this->k12, this->tmp_Zn);

    // return pai(e1,e2,s11,s12,s2)
    r->insertElement("e1", "Zn", this->e1);
    r->insertElement("e2", "Zn", this->e2);
    r->insertElement("s11", "Zn", this->s11);
    r->insertElement("s12", "Zn", this->s12);
    r->insertElement("s2", "Zn", this->s2);
}

bool FCR_CH_PreQA_DKS_2020::Check(FCR_CH_PreQA_DKS_2020_h *h, FCR_CH_PreQA_DKS_2020_r *r, element_t m, FCR_CH_PreQA_DKS_2020_pk *pk, FCR_CH_PreQA_DKS_2020_pp *pp) {
    // e1 + e2
    element_add(this->tmp_Zn, r->getElement("e1"), r->getElement("e2"));
    
    // (g1^s11)*(g2^s12)*(h^-e1)
    element_pow_zn(this->tmp_G, pp->getElement("g1"), r->getElement("s11"));
    element_pow_zn(this->tmp_G_2, pp->getElement("g2"), r->getElement("s12"));
    element_pow_zn(this->tmp_G_3, h->getElement("h"), r->getElement("e1"));
    element_mul(this->tmp_G, this->tmp_G, this->tmp_G_2);
    element_div(this->tmp_G, this->tmp_G, this->tmp_G_3);
    
    // (g1^s2)*(y^-e2)
    element_pow_zn(this->tmp_G_2, pp->getElement("g1"), r->getElement("s2"));
    element_pow_zn(this->tmp_G_3, pk->getElement("y"), r->getElement("e2"));
    element_div(this->tmp_G_2, this->tmp_G_2, this->tmp_G_3);

    // H((y,h,m),(tmp_G1, tmp_G1_2))
    this->H(this->tmp_Zn_2, pk->getElement("y"), h->getElement("h"), m, this->tmp_G, this->tmp_G_2);

    return element_cmp(this->tmp_Zn, this->tmp_Zn_2) == 0;
}

void FCR_CH_PreQA_DKS_2020::Adapt(FCR_CH_PreQA_DKS_2020_r *r_p, element_t m_p, element_t m, FCR_CH_PreQA_DKS_2020_h *h, FCR_CH_PreQA_DKS_2020_r *r, FCR_CH_PreQA_DKS_2020_sk *sk, FCR_CH_PreQA_DKS_2020_pp *pp) {
    FCR_CH_PreQA_DKS_2020_pk tmp_pk;
    // y = g1^x
    element_pow_zn(tmp_G, pp->getElement("g1"), sk->getElement("x"));
    tmp_pk.insertElement("y", "G1", tmp_G);
    // Check
    if(!this->Check(h, r, m, &tmp_pk, pp)){
        printf("Forge(): Check failed !!\n");
        throw std::invalid_argument("FCR_CH_PreQA_DKS_2020::Adapt(): Hash check failed !!");
    }
    
    element_random(this->k11);
    element_random(this->k12);
    element_random(this->e1);
    element_random(this->s11);
    element_random(this->s12);
    // u1 = (g1^s11) *(g2^s12) * (h^-e1)
    element_pow_zn(this->tmp_G, pp->getElement("g1"), this->s11);
    element_pow_zn(this->tmp_G_2, pp->getElement("g2"), this->s12);
    element_pow_zn(this->tmp_G_3, h->getElement("h"), this->e1);
    element_mul(this->u1, this->tmp_G, this->tmp_G_2);
    element_div(this->u1, this->u1, this->tmp_G_3);
    // ? u2 = (g1^k12)
    element_pow_zn(this->u2, pp->getElement("g1"), this->k12);
    // e = H((y,h,m_p),(u1,u2))
    this->H(this->e, tmp_pk.getElement("y"), h->getElement("h"), m_p, this->u1, this->u2);
    // e2 = e - e1
    element_sub(this->e2, this->e, this->e1);
    // s2 = k12 + e2 * x
    element_mul(this->tmp_Zn, this->e2, sk->getElement("x"));
    element_add(this->s2, this->k12, this->tmp_Zn);
    // return pai(e1,e2,s11,s12,s2)
    r_p->insertElement("e1", "Zn", this->e1);
    r_p->insertElement("e2", "Zn", this->e2);
    r_p->insertElement("s11", "Zn", this->s11);
    r_p->insertElement("s12", "Zn", this->s12);
    r_p->insertElement("s2", "Zn", this->s2);
}

bool FCR_CH_PreQA_DKS_2020::Verify(FCR_CH_PreQA_DKS_2020_h *h, FCR_CH_PreQA_DKS_2020_r *r_p, element_t m_p, FCR_CH_PreQA_DKS_2020_pk *pk, FCR_CH_PreQA_DKS_2020_pp *pp) {
    return this->Check(h, r_p, m_p, pk, pp);
}

FCR_CH_PreQA_DKS_2020::~FCR_CH_PreQA_DKS_2020() {
    element_clear(this->xi);
    element_clear(this->k11);
    element_clear(this->k12);
    element_clear(this->k2);
    element_clear(this->e2);
    element_clear(this->s2);

    element_clear(this->u1);
    element_clear(this->u2);

    element_clear(this->e);
    element_clear(this->e1);
    element_clear(this->s11);
    element_clear(this->s12);
}
