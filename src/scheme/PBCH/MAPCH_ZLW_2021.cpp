#include <scheme/MAPCH_ZLW_2021.h>

MAPCH_ZLW_2021::MAPCH_ZLW_2021(element_t *_G, element_t *_H, element_t *_Zn, element_t *_GT) 
    :ma_abe(_G, _H, _GT, _Zn){
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
 * @param mhks: master public keys
 * @param mtks: master secret keys
 * @param k: security parameter
 * @param As: attributes
 */
void MAPCH_ZLW_2021::SetUp(pp *pp, vector<mhk *> *mhks, vector<mtk *> *mtks, int k, vector<string> *As) {
    int numOfAuthority = As->size();

    CH_ET_BC_CDK_2017::pk pkCH;
    CH_ET_BC_CDK_2017::sk skCH;
    MA_ABE::gpk gpkABE;
    gpkABE.Init(this->G1);

    ch_et.SetUp(&pp->ppCH, k);
    ch_et.KeyGen(&pkCH, &skCH, &pp->ppCH);

    element_random(tmp_G);
    ma_abe.GlobalSetup(&gpkABE, &tmp_G);

    for(int i=0;i<numOfAuthority;i++){
        MAPCH_ZLW_2021::mhk *mhk = new MAPCH_ZLW_2021::mhk();
        MAPCH_ZLW_2021::mtk *mtk = new MAPCH_ZLW_2021::mtk();
        mhk->Init(this->G1, this->GT);
        mtk->Init(this->Zn);

        mhk->hk.setValues(&pkCH);
        mhk->gpkABE.setValues(&gpkABE);
        mtk->tk.setValues(&skCH);

        ma_abe.AuthSetup(&mhk->gpkABE, As->at(i), &mhk->pkj, &mtk->skj);

        mhks->push_back(mhk);
        mtks->push_back(mtk);
    }
}


/**
 * @param mskis: secret keys of gid and attributes
 * @param mtks: master secret keys
 * @param mhks: master public keys
 * @param As: attributes
 * @param GID: global identifier
 */
void MAPCH_ZLW_2021::KeyGen(vector<mski *> *mskis, vector<mtk *> *mtks, vector<mhk *> *mhks, vector<string> *As, string GID){
    int numOfAuthority = As->size();
    for(int i=0;i<numOfAuthority;i++){
        MAPCH_ZLW_2021::mski *mski = new MAPCH_ZLW_2021::mski();
        mski->Init(this->G1);

        mski->tk.setValues(&mtks->at(i)->tk);

        ma_abe.KeyGen(&mhks->at(i)->gpkABE, &mtks->at(i)->skj, GID, As->at(i), &mski->KiGid);

        mskis->push_back(mski);
    }
}

/**
 * @param h: hash value
 * @param pp: public parameters
 * @param mhks: master public keys
 * @param m: message
 * @param polocy: policy
 */
void MAPCH_ZLW_2021::Hash(h *h, pp *pp, vector<mhk *> *mhks, string m, string policy){
    CH_ET_BC_CDK_2017::etd etd;
    vector<MA_ABE::pkTheta *> pkThetas;
    for(int i=0;i<mhks->size();i++){
        MA_ABE::pkTheta *pkTheta = new MA_ABE::pkTheta();
        pkTheta->Init(this->G1, this->GT);
        pkTheta->setValues(&mhks->at(i)->pkj);
        pkThetas.push_back(pkTheta);
    }

    ch_et.Hash(&h->h, &h->r, &etd, &pp->ppCH, &mhks->at(0)->hk, m);

    mpz_to_element(tmp_GT, etd.d1);

    PrintMpzAndSize("etd", etd.d1);
    PrintElementAndSize("etd_GT", tmp_GT);
    
    ma_abe.Encrypt(&mhks->at(0)->gpkABE, &pkThetas, policy, &tmp_GT, &h->c);
}

/**
 * @param mhks: master public keys
 * @param m: message
 * @param h: hash value
 * @return bool
 */
bool MAPCH_ZLW_2021::Check(vector<mhk *> *mhks, string m, h *h){
    return ch_et.Check(&h->h, &h->r, &mhks->at(0)->hk, m);
}

/**
 * @param h_p: h with adapted random value
 * @param mhks: master public keys
 * @param msks: secret keys of gid and attributes
 * @param m: message
 * @param m_p: message
 * @param h: hash value
 */
void MAPCH_ZLW_2021::Forge(h *h_p,  vector<mhk *> *mhks, vector<mski *> *msks, string m, string m_p, h *h){
    if(!Check(mhks, m, h)){
        throw std::runtime_error("Forge: Hash Check failed!");
    }
    vector<MA_ABE::skgidA *> skgidAs;
    for(int i=0;i<msks->size();i++){
        MA_ABE::skgidA *skgidA = new MA_ABE::skgidA();
        skgidA->Init(this->G1);
        skgidA->setValues(&msks->at(i)->KiGid);
        skgidAs.push_back(skgidA);
    }

    ma_abe.Decrypt(&skgidAs, &h->c, &tmp_GT);

    CH_ET_BC_CDK_2017::etd etd;
    mpz_from_element(etd.d1, tmp_GT);

    PrintMpz("etd", etd.d1);
    PrintElement("etd_GT", tmp_GT);

    ch_et.Adapt(&h_p->r, &msks->at(0)->tk, &etd, &mhks->at(0)->hk, &h->h, &h->r, m, m_p);

    h_p->c.setValues(&h->c);
    h_p->h.setValues(&h->h);
}

/**
 * @param pkDPCH: public key of DPCH
 * @param m_p: message
 * @param h: hash value
 * @param r_p: random value
 * @return bool
 */
bool MAPCH_ZLW_2021::Verify(vector<mhk *> *mhks, string m_p, h *h_p){
    return Check(mhks, m_p, h_p);
}


MAPCH_ZLW_2021::~MAPCH_ZLW_2021() {
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