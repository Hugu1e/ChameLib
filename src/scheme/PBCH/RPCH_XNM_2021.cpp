#include <scheme/RPCH_XNM_2021.h>

RPCH_XNM_2021::RPCH_XNM_2021(mpz_t *_n,mpz_t *_e, mpz_t *_d, element_t *_G, element_t *_H, element_t *_Zn, element_t *_GT) 
    : rsa(_n, _e, _d), rabe(_G, _H, _GT, _Zn){
    
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
   
}

/**
 * input : k, n
 * output: skRPCH, pkRPCH, _rl, _st
 */
void RPCH_XNM_2021::PG(int k, int n, skRPCH *skRPCH, pkRPCH *pkRPCH, vector<RABE::revokedPreson *> *rl, binary_tree_RABE *&st) {
    this->k = k;
    // e > N
    this->rsa.rsa_generate_keys_2(k, 1);
    // set d1
    mpz_set(skRPCH->skCHET.d1, *(this->rsa.getD()));
    // set N1,e
    mpz_set(pkRPCH->pkCHET.N1, *(this->rsa.getN()));
    mpz_set(pkRPCH->pkCHET.e, *(this->rsa.getE()));

    this->rabe.Setup(n, &pkRPCH->mpkRABE, &skRPCH->mskRABE, rl, st);
}

/**
 * input : pkRPCH, skRPCH, _st, id, attr_list
 * output: skidRPCH
 */
void RPCH_XNM_2021::KG(pkRPCH *pkRPCH, skRPCH *skRPCH, binary_tree_RABE *st, element_t *id, vector<string> *attr_list, skidRPCH *skidRPCH) {
    this->rabe.KGen(&pkRPCH->mpkRABE, &skRPCH->mskRABE, st, id, attr_list, &skidRPCH->skidRABE);
    mpz_set(skidRPCH->skCHET.d1, skRPCH->skCHET.d1);
}

void RPCH_XNM_2021::H1(mpz_t *m, mpz_t *N1, mpz_t *N2, mpz_t * n, mpz_t *res){
    Hgsm_n_2(*m, *N1, *N2, *n, *res);
}
void RPCH_XNM_2021::H2(mpz_t *m, mpz_t *N1, mpz_t *N2, mpz_t * n, mpz_t *res){
    Hgsm_n_2(*m, *N1, *N2, *n, *res);
}

/**
 * (u1,u2)<-H4((r,A))
 */
void RPCH_XNM_2021::H4(mpz_t *r, string A, element_t *u1, element_t *u2){
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
void RPCH_XNM_2021::KUpt(pkRPCH *pkRPCH, binary_tree_RABE *st, vector<RABE::revokedPreson *> *rl, time_t t, RABE::kut *kut){
    this->rabe.KUpt(&pkRPCH->mpkRABE, st, rl, t, kut);
}

/**
 * input : pkRPCH, skidRPCH, kut
 * output: dkidtRPCH
 */
void RPCH_XNM_2021::DKGen(pkRPCH *pkRPCH, skidRPCH *skidRPCH, RABE::kut *kut, dkidtRPCH *dkidtRPCH){
    this->rabe.DKGen(&pkRPCH->mpkRABE, &skidRPCH->skidRABE, kut, &dkidtRPCH->dkidtRABE);
    mpz_set(dkidtRPCH->skCHET.d1, skidRPCH->skCHET.d1);
}

/**
 * input : _rl, id, t
 */
void RPCH_XNM_2021::Rev(vector<RABE::revokedPreson *> *rl, element_t *id, time_t t){
    this->rabe.Rev(rl, id, t);
}


/**
 * input : pkRPCH, m, policy_str, t
 * output: h, r
 */
void RPCH_XNM_2021::Hash(pkRPCH *pkRPCH, mpz_t *m, string policy_str, time_t t, h *h, r *r) {
    mpz_t tmp1, tmp2;
    mpz_inits(tmp1, tmp2, NULL);
    this->rsa.rsa_generate_keys_with_e(this->k, &pkRPCH->pkCHET.e);

    // h.N2
    mpz_set(h->N2, *this->rsa.getN());
    
    // r1 ∈ ZN1*
    GenerateRandomInZnStar(r->r1, pkRPCH->pkCHET.N1);
    // r2 ∈ ZN2*
    GenerateRandomInZnStar(r->r2, *this->rsa.getN());

    // h1 = H1(m,N1,N2)* r1^e mod N1
    H1(m, &pkRPCH->pkCHET.N1, this->rsa.getN(), &pkRPCH->pkCHET.N1, &tmp1);
    // r1^e mod N1
    mpz_powm(tmp2, r->r1, pkRPCH->pkCHET.e, pkRPCH->pkCHET.N1);
    mpz_mul(h->h1, tmp1, tmp2);
    mpz_mod(h->h1, h->h1, pkRPCH->pkCHET.N1);
    // h2 = H2(m,N1,N2)* r2^e mod N2
    H2(m, &pkRPCH->pkCHET.N1, this->rsa.getN(), this->rsa.getN(), &tmp1);
    // r2^e mod N2
    mpz_powm(tmp2, r->r2, pkRPCH->pkCHET.e, *this->rsa.getN());
    mpz_mul(h->h2, tmp1, tmp2);
    mpz_mod(h->h2, h->h2, *this->rsa.getN());

    this->aes.KGen(256, &this->K);

    // TODO s1,s2
    element_random(this->s1);
    element_random(this->s2);

    this->rabe.Enc(&pkRPCH->mpkRABE, &this->K, policy_str, t, &this->s1, &this->s2, &h->ct);

    // cSE = EncSE(kk, d2)
    this->aes.Enc(&K, this->rsa.getD(), &h->cSE);

    mpz_clears(tmp1, tmp2, NULL);
}

/**
 * input : pkRPCH, m, h, r
 * output: bool
 */
bool RPCH_XNM_2021::Check(pkRPCH *pkRPCH, mpz_t *m, h *h, r *r) {
    mpz_t tmp1, tmp2, tmp3;
    mpz_inits(tmp1, tmp2, tmp3, NULL);
    // h1 = H1(m,N1,N2)* r1^e mod N1
    H1(m, &pkRPCH->pkCHET.N1, &h->N2, &pkRPCH->pkCHET.N1, &tmp1);
    // r1^e mod N1
    mpz_powm(tmp2, r->r1, pkRPCH->pkCHET.e, pkRPCH->pkCHET.N1);
    mpz_mul(tmp3, tmp1, tmp2);
    mpz_mod(tmp3, tmp3, pkRPCH->pkCHET.N1);
    if(mpz_cmp(tmp3, h->h1) != 0){
        mpz_clears(tmp1, tmp2,tmp3, NULL);
        return false;
    }
    // h2 = H2(m,N1,N2)* r2^e mod N2
    H2(m, &pkRPCH->pkCHET.N1, &h->N2, &h->N2, &tmp1);
    // r2^e mod N2
    mpz_powm(tmp2, r->r2, pkRPCH->pkCHET.e, h->N2);
    mpz_mul(tmp3, tmp1, tmp2);
    mpz_mod(tmp3, tmp3, h->N2);
    if(mpz_cmp(tmp3, h->h2) != 0){
        mpz_clears(tmp1, tmp2,tmp3, NULL);
        return false;
    }
    mpz_clears(tmp1, tmp2, tmp3, NULL);
    return true;
}

/**
 * input : pkRPCH, dkidtRPCH, m, m', h, r
 * output: r'
 */
void RPCH_XNM_2021::Forge(pkRPCH * pkRPCH, dkidtRPCH *dkidtRPCH, mpz_t *m, mpz_t *m_p, h *h, RPCH_XNM_2021::r *r, RPCH_XNM_2021::r *r_p) {
    mpz_t kk,rr,d2,x1,x1_p,y1,x2,x2_p,y2;
    mpz_inits(kk,rr,d2,x1,x1_p,y1,x2,x2_p,y2,NULL);

    rabe.Dec(&pkRPCH->mpkRABE, &h->ct, &dkidtRPCH->dkidtRABE, &this->K);

    // DecSE(kk, ct_) -> d2
    this->aes.Dec(&this->K, &h->cSE, &d2);

    // x1 = H1(m, N1, N2)
    H1(m, &pkRPCH->pkCHET.N1, &h->N2, &pkRPCH->pkCHET.N1, &x1);
    // x1' = H1(m', N1, N2)
    H1(m_p, &pkRPCH->pkCHET.N1, &h->N2, &pkRPCH->pkCHET.N1, &x1_p);
    // y1 = x1 r1^e mod N1
    mpz_powm(y1, r->r1, pkRPCH->pkCHET.e, pkRPCH->pkCHET.N1);
    mpz_mul(y1, x1, y1);
    mpz_mod(y1, y1, pkRPCH->pkCHET.N1);
    // x2 = H2(m, N1, N2)
    H2(m, &pkRPCH->pkCHET.N1, &h->N2, &h->N2, &x2);
    // x2' = H2(m', N1, N2)
    H2(m_p, &pkRPCH->pkCHET.N1, &h->N2, &h->N2, &x2_p);
    // y2 = x2 r2^e mod N2
    mpz_powm(y2, r->r2, pkRPCH->pkCHET.e, h->N2);
    mpz_mul(y2, x2, y2);
    mpz_mod(y2, y2, h->N2);
    // r1' = (y1(x1'^(-1)))^d1 mod N1
    mpz_invert(x1_p, x1_p, pkRPCH->pkCHET.N1);
    mpz_mul(r_p->r1, y1, x1_p);
    mpz_mod(r_p->r1, r_p->r1, pkRPCH->pkCHET.N1);
    mpz_powm(r_p->r1, r_p->r1, dkidtRPCH->skCHET.d1, pkRPCH->pkCHET.N1);
    // r2' = (y2(x2'^(-1)))^d2 mod N2
    mpz_invert(x2_p, x2_p, h->N2);
    mpz_mul(r_p->r2, y2, x2_p);
    mpz_mod(r_p->r2, r_p->r2, h->N2);
    mpz_powm(r_p->r2, r_p->r2, d2, h->N2);
  

    mpz_clears(kk,rr,d2,x1,x1_p,y1,x2,x2_p,y2,NULL);
}

/**
 * input : pkPCH, m', h, r'
 * output: bool
 */
bool RPCH_XNM_2021::Verify(pkRPCH *pkRPCH, mpz_t *m_p, h *h, r *r_p) {
    return this->Check(pkRPCH, m_p, h, r_p);
}


RPCH_XNM_2021::~RPCH_XNM_2021() {
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
   
}