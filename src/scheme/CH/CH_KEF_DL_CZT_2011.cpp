#include <scheme/CH/CH_KEF_DL_CZT_2011.h>

CH_KEF_DL_CZT_2011::CH_KEF_DL_CZT_2011(int curve, int group) : PbcScheme(curve) {
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

    element_init_same_as(tmp_G, G1);
    element_init_same_as(tmp_G_2, G1);
    element_init_same_as(tmp_G_3, G1);
    element_init_same_as(tmp_Zn, Zn);
    element_init_same_as(tmp_Zn_2, Zn);
    element_init_same_as(tmp_Zn_3, Zn);
}

void CH_KEF_DL_CZT_2011::SetUp(CH_KEF_DL_CZT_2011_pp &pp) {    
    element_random(tmp_G);
    pp.set(g, tmp_G);
}

void CH_KEF_DL_CZT_2011::KeyGen(CH_KEF_DL_CZT_2011_pk &pk, CH_KEF_DL_CZT_2011_sk &sk, CH_KEF_DL_CZT_2011_pp &pp) {
    element_random(tmp_Zn);
    sk.set(x, tmp_Zn);

    element_pow_zn(tmp_G, pp[g], tmp_Zn);
    pk.set(y, tmp_G);
}

void CH_KEF_DL_CZT_2011::H(element_t res, element_t m1, element_t m2) {
    HASH::hash(res, m1, m2);
}

void CH_KEF_DL_CZT_2011::Hash(element_t h, CH_KEF_DL_CZT_2011_r &r, element_t I, element_t m, CH_KEF_DL_CZT_2011_pk &pk, CH_KEF_DL_CZT_2011_pp &pp) {
    element_random(tmp_Zn);
    
    element_pow_zn(tmp_G, pp[g], tmp_Zn);
    r.set(r1, tmp_G);
    element_pow_zn(tmp_G_3, pk[y], tmp_Zn);
    r.set(r2, tmp_G_3);
   
    this->H(this->tmp_G_3, pk[y], I);
        
    element_pow_zn(this->tmp_G_2, this->tmp_G_3, m);
    element_mul(h, tmp_G, this->tmp_G_2);
}

bool CH_KEF_DL_CZT_2011::Check(element_t h, CH_KEF_DL_CZT_2011_r &r, element_t I, element_t m, CH_KEF_DL_CZT_2011_pk &pk, CH_KEF_DL_CZT_2011_pp &pp) {
    this->H(this->tmp_G, pk[y], I);
        
    element_pow_zn(this->tmp_G_2, this->tmp_G, m);
    element_mul(tmp_G_2, r[r1], this->tmp_G_2);

    return element_cmp(h, tmp_G_2) == 0;
}

void CH_KEF_DL_CZT_2011::Adapt(CH_KEF_DL_CZT_2011_r &r_p, CH_KEF_DL_CZT_2011_sk &sk, element_t h, element_t m, CH_KEF_DL_CZT_2011_r &r, element_t m_p, element_t I, CH_KEF_DL_CZT_2011_pp &pp) {
    // y
    element_pow_zn(this->tmp_G, pp[g], sk[x]);
    // tmp_h
    this->H(this->tmp_G_2, this->tmp_G, I);
    // g^a  r_1
    // h^(m-m')
    element_sub(this->tmp_Zn, m, m_p);
    element_pow_zn(this->tmp_G_3, this->tmp_G_2, this->tmp_Zn);
    element_mul(tmp_G_3, r[r1], this->tmp_G_3);
    r_p.set(r1, tmp_G_3);
    // y^a r_2
    // x(m-m')
    element_mul(this->tmp_Zn_2, sk[x], this->tmp_Zn);
    element_pow_zn(this->tmp_G, this->tmp_G_2, this->tmp_Zn_2);
    element_mul(tmp_G, r[r2], this->tmp_G);
    r_p.set(r2, tmp_G);
}

bool CH_KEF_DL_CZT_2011::Verify(element_t h, CH_KEF_DL_CZT_2011_r &r_p, element_t I, element_t m_p, CH_KEF_DL_CZT_2011_pk &pk, CH_KEF_DL_CZT_2011_pp &pp) {
    return this->Check(h, r_p, I, m_p, pk, pp);
}

CH_KEF_DL_CZT_2011::~CH_KEF_DL_CZT_2011() {
    element_clear(tmp_G);
    element_clear(tmp_G_2);
    element_clear(tmp_G_3);
    element_clear(tmp_Zn);
    element_clear(tmp_Zn_2);
    element_clear(tmp_Zn_3);
    element_clear(G1);
    element_clear(Zn);
}
