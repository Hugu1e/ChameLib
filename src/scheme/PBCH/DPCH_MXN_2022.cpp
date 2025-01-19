#include <scheme/DPCH_MXN_2022.h>

DPCH_MXN_2022::DPCH_MXN_2022(element_t *_G, element_t *_H, element_t *_Zn, element_t *_GT) 
    :ma_abe(_G, _H, _GT, _Zn), aes(), bls(_G, _H, _GT, _Zn) {
    this->G1 = _G;
    this->G2 = _H;
    this->GT = _GT;
    this->Zn = _Zn;
    element_init_same_as(this->tmp_G, *this->G1);
    element_init_same_as(this->tmp_G_2, *this->G1);
    element_init_same_as(this->tmp_G_3, *this->G1);
    element_init_same_as(this->tmp_G_4, *this->G1);
    element_init_same_as(this->tmp_H, *this->G2);
    element_init_same_as(this->tmp_H_2, *this->G2);
    element_init_same_as(this->tmp_H_3, *this->G2);
    element_init_same_as(this->tmp_GT, *this->GT);
    element_init_same_as(this->tmp_GT_2, *this->GT);
    element_init_same_as(this->tmp_GT_3, *this->GT);
    element_init_same_as(this->tmp_Zn, *this->Zn);
    element_init_same_as(this->tmp_Zn_2, *this->Zn);
    element_init_same_as(this->tmp_Zn_3, *this->Zn);
   
}

/**
 * @param pp: public parameters
 * @param pkDPCH: public key of DPCH
 * @param skDPCH: secret key of DPCH
 * @param k: key size
 */
void DPCH_MXN_2022::SetUp(pp *pp, pkDPCH *pkDPCH, skDPCH *skDPCH, int k) {
    element_random(tmp_G);

    ch_et.SetUp(&pp->ppCH, k);
    ch_et.KeyGen(&pkDPCH->pkCH, &skDPCH->skCH, &pp->ppCH);

    ma_abe.GlobalSetup(&pp->gpkMA_ABE, &tmp_G);

    bls.Setup(&pp->ppBLS, &tmp_G);
    bls.KeyGen(&pp->ppBLS, &pkDPCH->pkBLS, &skDPCH->skBLS);
}

/**
 * @param skGid: secret key of gid
 * @param sigmaGid: signature of gid
 * @param skDPCH: secret key of DPCH
 * @param gid: global id
 */
void DPCH_MXN_2022::ModSetUp(skGid *skGid, sigmaGid *sigmaGid, skDPCH *skDPCH, string gid){
    mpz_set(skGid->skCH.d0, skDPCH->skCH.d0);
    bls.Sign(&skDPCH->skBLS, gid, &sigmaGid->signature);
}

/**
 * @param pkTheta: public key of Theta
 * @param skTheta: secret key of Theta
 * @param pp: public parameters
 * @param A: attribute
 */
void DPCH_MXN_2022::AuthSetUp(pkTheta *pkTheta, skTheta *skTheta, pp *pp, string A){
    ma_abe.AuthSetup(&pp->gpkMA_ABE, A, &pkTheta->pk, &skTheta->sk);
}

/**
 * @param skGidA: secret key of gid and attribute
 * @param pp: public parameters
 * @param pkDPCH: public key of DPCH
 * @param gid: global id
 * @param sigmaGid: signature of gid
 * @param skTheta: secret key of Theta
 * @param A: attribute
 */
void DPCH_MXN_2022::ModKeyGen(skGidA *skGidA, pp *pp, pkDPCH *pkDPCH, string gid, sigmaGid *sigmaGid, skTheta *skTheta, string A){
    if(!(bls.Verify(&pp->ppBLS, &pkDPCH->pkBLS, gid, &sigmaGid->signature))){
        throw std::runtime_error("ModKeyGen(): Signature Verify failed!");
    }

    ma_abe.KeyGen(&pp->gpkMA_ABE, &skTheta->sk, gid, A, &skGidA->sk);
}

/**
 * @param h: hash value
 * @param r: random value
 * @param c: cyphertext
 * @param pp: public parameters
 * @param pkDPCH: public key of DPCH
 * @param m: message
 * @param pkThetas: public keys of Theta
 * @param polocy: policy
 */
void DPCH_MXN_2022::Hash(h *h, r *r, c *c, pp *pp, pkDPCH *pkDPCH, string m, vector<DPCH_MXN_2022::pkTheta *> *pkThetas, string polocy){
    CH_ET_BC_CDK_2017::etd etd;

    ch_et.Hash(&h->h, &r->r, &etd, &pp->ppCH, &pkDPCH->pkCH, m);

    aes.KGen(256, &tmp_GT);
    aes.Enc(&tmp_GT, &etd.d1, &c->c_etd);

    vector<MA_ABE::pkTheta *> pkThetas_ABE;
    for(int i=0;i<pkThetas->size();i++){
        pkThetas_ABE.push_back(&pkThetas->at(i)->pk);
    }
    ma_abe.Encrypt(&pp->gpkMA_ABE, &pkThetas_ABE, polocy, &tmp_GT, &c->c_abe);
}

/**
 * @param pkDPCH: public key of DPCH
 * @param m: message
 * @param h: hash value
 * @param r: random value
 * @return bool
 */
bool DPCH_MXN_2022::Check(pkDPCH *pkDPCH, string m, h *h, r *r){
    return ch_et.Check(&h->h, &r->r, &pkDPCH->pkCH, m);
}

/**
 * @param r_p: random value
 * @param pkDPCH: public key of DPCH
 * @param skGid: secret key of gid
 * @param skGidAs: secret keys of gid and attributes
 * @param c: cyphertext
 * @param m: message
 * @param m_p: message
 * @param h: hash value
 * @param r: random value
 */
void DPCH_MXN_2022::Forge(r *r_p, pkDPCH *pkDPCH, skGid *skGid, vector<DPCH_MXN_2022::skGidA *> *skGidAs, c *c, string m, string m_p, h *h, r *r){
    if(m == m_p){
        mpz_set(r_p->r.r0, r->r.r0);
        mpz_set(r_p->r.r1, r->r.r1);
        return;
    }

    vector<MA_ABE::skgidA *> skgidAs_ABE;
    for(int i=0;i<skGidAs->size();i++){
        skgidAs_ABE.push_back(&skGidAs->at(i)->sk);
    }
    ma_abe.Decrypt(&skgidAs_ABE, &c->c_abe, &tmp_GT);

    CH_ET_BC_CDK_2017::etd etd;
    aes.Dec(&tmp_GT, &c->c_etd, &etd.d1);

    ch_et.Adapt(&r_p->r, &skGid->skCH, &etd, &pkDPCH->pkCH, &h->h, &r->r, m, m_p);
}

/**
 * @param pkDPCH: public key of DPCH
 * @param m_p: message
 * @param h: hash value
 * @param r_p: random value
 * @return bool
 */
bool DPCH_MXN_2022::Verify(pkDPCH *pkDPCH, string m_p, h *h, r *r_p){
    return Check(pkDPCH, m_p, h, r_p);
}


DPCH_MXN_2022::~DPCH_MXN_2022() {
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
}