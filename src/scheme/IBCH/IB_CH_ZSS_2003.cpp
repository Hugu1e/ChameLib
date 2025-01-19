#include <scheme/IB_CH_ZSS_2003.h>

void IB_CH::H0(element_t &m, element_t &res) {
    Hm(m, res, this->tmp_Zn, *this->G1);
}

void IB_CH::H1(element_t &m, element_t &res) {
    Hm(m, res, this->tmp_Zn, *this->Zn);
}

void IB_CH::H2(element_t &m, element_t &res) {
    Hm(m, res, this->tmp_Zn, *this->G2);
}

IB_CH::IB_CH(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT, int _rev_G1G2) {
    this->G1 = _G1;
    this->G2 = _G2;
    this->GT = _GT;
    this->Zn = _Zn;
    this->rev_G1G2 = _rev_G1G2;
    element_init_same_as(this->msk, *this->Zn);
    element_init_same_as(this->P, *this->G2);
    element_init_same_as(this->P_pub, *this->G2);
    element_init_same_as(this->tmp_G1, *this->G1);
    element_init_same_as(this->tmp_G2, *this->G2);
    element_init_same_as(this->tmp_GT, *this->GT);
    element_init_same_as(this->tmp_GT_2, *this->GT);
    element_init_same_as(this->tmp_Zn, *this->Zn);
    element_init_same_as(this->tmp_Zn_2, *this->Zn);
}

void IB_CH::Setup() {
    element_random(this->msk);
    element_random(this->P);
    element_mul_zn(this->P_pub, this->P, this->msk);
}

IB_CH::~IB_CH() {
    element_clear(this->P);
    element_clear(this->P_pub);
    element_clear(this->tmp_G1);
    element_clear(this->tmp_G2);
    element_clear(this->tmp_GT);
    element_clear(this->tmp_GT_2);
    element_clear(this->tmp_Zn);
    element_clear(this->tmp_Zn_2);
}

void IB_CH_S1::Extract(element_t *S_ID, element_t *ID) {
    this->H0(*ID, this->tmp_G1);
    element_mul_zn(*S_ID, this->tmp_G1, this->msk);
}

void IB_CH_S1::base_hash(element_t *H, element_t *R, element_t *ID, element_t *m) {
    if(this->rev_G1G2) element_pairing(*H, this->P, *R);
    else element_pairing(*H, *R, this->P);
    this->H1(*m, this->tmp_Zn);
    this->H0(*ID, this->tmp_G1);
    element_mul_zn(this->tmp_G1, this->tmp_G1, this->tmp_Zn);
    if(this->rev_G1G2) element_pairing(this->tmp_GT, this->P_pub, this->tmp_G1);
    else element_pairing(this->tmp_GT, this->tmp_G1, this->P_pub);
    element_mul(*H, *H, this->tmp_GT);
}

void IB_CH_S1::Hash(element_t *H, element_t *R, element_t *ID, element_t *m) {
    element_random(*R);
    this->base_hash(H, R, ID, m);
}

bool IB_CH_S1::Verify(element_t *H, element_t *R, element_t *ID, element_t *m) {
    this->base_hash(&this->tmp_GT_2, R, ID, m);
    return element_cmp(*H, this->tmp_GT_2) == 0;
}

void IB_CH_S1::Forge(element_t *ID, element_t *S_ID, element_t *m, element_t *m_p, element_t *R, element_t *R_p) {
    this->H1(*m, this->tmp_Zn);
    this->H1(*m_p, this->tmp_Zn_2);
    element_sub(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    element_mul_zn(this->tmp_G1, *S_ID, this->tmp_Zn);
    element_add(*R_p, this->tmp_G1, *R);
}

void IB_CH_S2::Setup() {
    IB_CH::Setup();
    element_random(this->P_1);
}

void IB_CH_S2::Extract(element_t *S_ID, element_t *ID) {
    this->H1(*ID, this->tmp_Zn);
    element_add(this->tmp_Zn, this->msk, this->tmp_Zn);
    element_invert(this->tmp_Zn, this->tmp_Zn);
    element_mul_zn(*S_ID, this->P_1, this->tmp_Zn);
}

void IB_CH_S2::base_hash(element_t *H, element_t *R, element_t *ID, element_t *m) {
    this->H1(*ID, this->tmp_Zn);
    element_mul_zn(this->tmp_G2, this->P, this->tmp_Zn);
    element_add(this->tmp_G2, this->tmp_G2, this->P_pub);
    if(this->rev_G1G2) element_pairing(*H, this->tmp_G2, *R);
    else element_pairing(*H, *R, this->tmp_G2);
    if(this->rev_G1G2) element_pairing(this->tmp_GT, this->P, this->P_1);
    else element_pairing(this->tmp_GT, this->P_1, this->P);
    element_mul(*H, this->tmp_GT, *H);
    this->H1(*m, this->tmp_Zn);
    element_pow_zn(*H, *H, this->tmp_Zn);
}

void IB_CH_S2::Hash(element_t *H, element_t *R, element_t *ID, element_t *m) {
    element_random(*R);
    this->base_hash(H, R, ID, m);
}

bool IB_CH_S2::Verify(element_t *H, element_t *R, element_t *ID, element_t *m) {
    this->base_hash(&this->tmp_GT_2, R, ID, m);
    return element_cmp(*H, this->tmp_GT_2) == 0;
}

void IB_CH_S2::Forge(element_t *ID, element_t *S_ID, element_t *m, element_t *m_p, element_t *R, element_t *R_p) {
    this->H1(*m, this->tmp_Zn);
    element_mul_zn(this->tmp_G1, *R, this->tmp_Zn);
    this->H1(*m_p, this->tmp_Zn_2);
    element_sub(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    element_mul_zn(*R_p, *S_ID, this->tmp_Zn);
    element_add(*R_p, *R_p, this->tmp_G1);
    element_invert(this->tmp_Zn_2, this->tmp_Zn_2);
    element_mul_zn(*R_p, *R_p, this->tmp_Zn_2);
}

IB_CH_S2::~IB_CH_S2() {
    element_clear(this->P_1);
}