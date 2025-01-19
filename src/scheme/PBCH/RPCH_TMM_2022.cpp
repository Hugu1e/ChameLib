#include <scheme/RPCH_TMM_2022.h>

RPCH_TMM_2022::RPCH_TMM_2022(mpz_t *_n,mpz_t *_e, mpz_t *_d, element_t *_G, element_t *_H, element_t *_Zn, element_t *_GT) 
    : rabe(_G, _H, _GT, _Zn){
    
    this->G = _G;
    this->H = _H;
    this->GT = _GT;
    this->Zn = _Zn;

    element_init_same_as(this->tmp_G, *this->G);
    element_init_same_as(this->tmp_G_2, *this->G);
    element_init_same_as(this->tmp_G_3, *this->G);
    element_init_same_as(this->tmp_G_4, *this->G);
    element_init_same_as(this->tmp_H, *this->H);
    element_init_same_as(this->tmp_H_2, *this->H);
    element_init_same_as(this->tmp_H_3, *this->H);
    element_init_same_as(this->tmp_GT, *this->GT);
    element_init_same_as(this->tmp_GT_2, *this->GT);
    element_init_same_as(this->tmp_GT_3, *this->GT);
    element_init_same_as(this->tmp_Zn, *this->Zn);
    element_init_same_as(this->tmp_Zn_2, *this->Zn);
    element_init_same_as(this->tmp_Zn_3, *this->Zn);

    element_init_same_as(this->s1, *this->Zn);
    element_init_same_as(this->s2, *this->Zn);
    element_init_same_as(this->K, *this->GT);

    element_init_same_as(this->R, *this->Zn);
   
}

/**
 * input : k, n
 * output: skRPCH, pkRPCH, _rl, _st
 */
void RPCH_TMM_2022::PG(int k, int n, skRPCH *skRPCH, pkRPCH *pkRPCH, std::vector<RABE_TMM::revokedPreson *> *rl, binary_tree_RABE *&st) {
    this->k = k;
    
    this->rabe.Setup(n, &pkRPCH->mpkRABE, &skRPCH->mskRABE, rl, st);

    element_random(skRPCH->skCHET.x);
    // y = g^x
    element_pow_zn(pkRPCH->pkCHET.y, pkRPCH->mpkRABE.g, skRPCH->skCHET.x);
}

/**
 * input : pkRPCH, skRPCH, _st, id, attr_list
 * output: skidRPCH
 */
void RPCH_TMM_2022::KG(pkRPCH *pkRPCH, skRPCH *skRPCH, binary_tree_RABE *st, element_t *id, vector<string> *attr_list, skidRPCH *skidRPCH) {
    this->rabe.KGen(&pkRPCH->mpkRABE, &skRPCH->mskRABE, st, id, attr_list, &skidRPCH->skidRABE);
    element_set(skidRPCH->skCHET.x, skRPCH->skCHET.x);
}

void RPCH_TMM_2022::H1(mpz_t *m, mpz_t *N1, mpz_t *N2, mpz_t * n, mpz_t *res){
    Hgsm_n_2(*m, *N1, *N2, *n, *res);
}
void RPCH_TMM_2022::H2(mpz_t *m, mpz_t *N1, mpz_t *N2, mpz_t * n, mpz_t *res){
    Hgsm_n_2(*m, *N1, *N2, *n, *res);
}

/**
 * (u1,u2)<-H4((r,A))
 */
void RPCH_TMM_2022::H4(mpz_t *r, string A, element_t *u1, element_t *u2){
    // r -> string
    string r_str = mpz_get_str(NULL, 10, *r);
    // str -> element_t
    element_from_hash(this->tmp_Zn, (unsigned char *)r_str.c_str(), r_str.length());
    element_from_hash(this->tmp_Zn_2, (unsigned char *)A.c_str(), A.length());
    Hm_1(this->tmp_Zn, *u1);
    Hm_1(this->tmp_Zn_2, *u2);
}

/**
 * input : pkRPCH, _st, _rl, t
 * output: kut
 */
void RPCH_TMM_2022::KUpt(pkRPCH *pkRPCH, binary_tree_RABE *st, std::vector<RABE_TMM::revokedPreson *> *rl, time_t t, RABE_TMM::kut *kut){
    this->rabe.KUpt(&pkRPCH->mpkRABE, st, rl, t, kut);
}

/**
 * input : pkRPCH, skidRPCH, kut
 * output: dkidtRPCH
 */
void RPCH_TMM_2022::DKGen(pkRPCH *pkRPCH, skidRPCH *skidRPCH, RABE_TMM::kut *kut, dkidtRPCH *dkidtRPCH){
    this->rabe.DKGen(&pkRPCH->mpkRABE, &skidRPCH->skidRABE, kut, &dkidtRPCH->dkidtRABE);
    element_set(dkidtRPCH->skCHET.x, skidRPCH->skCHET.x);
}

/**
 * input : _rl, id, t
 */
void RPCH_TMM_2022::Rev(std::vector<RABE_TMM::revokedPreson *> *rl, element_t *id, time_t t){
    this->rabe.Rev(rl, id, t);
}


/**
 * input : pkRPCH, m, policy_str, t
 * output: h, r
 */
void RPCH_TMM_2022::Hash(pkRPCH *pkRPCH, element_t *m, string policy_str, time_t t, 
                            element_t *b, element_t *r, element_t *h, RABE_TMM::ciphertext *C) {
    element_random(*r);
    element_random(this->R);

    // h = g^R
    element_pow_zn(*h, pkRPCH->mpkRABE.g, this->R);
    // CH b = pk^m * h^r
    element_pow_zn(this->tmp_G, *h, *r);
    element_pow_zn(this->tmp_G_2, pkRPCH->pkCHET.y, *m);
    element_mul(*b, this->tmp_G, this->tmp_G_2);


    // TODO s1,s2
    element_random(this->s1);
    element_random(this->s2);

    this->rabe.Enc(&pkRPCH->mpkRABE, &this->R, policy_str, t, &this->s1, &this->s2, C);
}

/**
 * input : pkRPCH, m, h, r
 * output: bool
 */
bool RPCH_TMM_2022::Check(pkRPCH *pkRPCH, element_t *m, element_t *b, element_t *r, element_t *h) {
    // CH b = pk^m * h^r
    element_pow_zn(this->tmp_G, *h, *r);
    element_pow_zn(this->tmp_G_2, pkRPCH->pkCHET.y, *m);
    element_mul(this->tmp_G, this->tmp_G, this->tmp_G_2);
    
    return element_cmp(*b, this->tmp_G) == 0;
}

/**
 * input : pkRPCH, dkidtRPCH, m, m', h, r
 * output: r'
 */
void RPCH_TMM_2022::Forge(pkRPCH * pkRPCH, dkidtRPCH *dkidtRPCH, element_t *m, element_t *m_p, element_t *b, element_t *r, element_t *h, RABE_TMM::ciphertext *C, element_t *r_p) {
    // Check
    if (!this->Check(pkRPCH, m, b, r, h)) {
        printf("Hash Check failed\n");
        return;
    }

    rabe.Dec(&pkRPCH->mpkRABE, C, &dkidtRPCH->dkidtRABE, &this->R);
    
    // r' = r + (m - m')*sk/R
    element_sub(this->tmp_Zn, *m, *m_p);
    element_mul(this->tmp_Zn_2, this->tmp_Zn, dkidtRPCH->skCHET.x);
    element_div(this->tmp_Zn_3, this->tmp_Zn_2, this->R);
    element_add(*r_p, *r, this->tmp_Zn_3);
}

/**
 * input : pkPCH, m', h, r'
 * output: bool
 */
bool RPCH_TMM_2022::Verify(pkRPCH *pkRPCH, element_t *m_p, element_t *b, element_t *r_p, element_t *h) {
    return this->Check(pkRPCH, m_p, b, r_p, h);
}


RPCH_TMM_2022::~RPCH_TMM_2022() {
    element_clear(this->tmp_G);
    element_clear(this->tmp_G_2);
    element_clear(this->tmp_G_3);
    element_clear(this->tmp_G_4);
    element_clear(this->tmp_H);
    element_clear(this->tmp_H_2);
    element_clear(this->tmp_H_3);
    element_clear(this->tmp_GT);
    element_clear(this->tmp_GT_2);
    element_clear(this->tmp_GT_3);
    element_clear(this->tmp_Zn);
    element_clear(this->tmp_Zn_2);
    element_clear(this->tmp_Zn_3);

    element_clear(this->s1);
    element_clear(this->s2);
    element_clear(this->K);
    element_clear(this->R);
   
}