#include <scheme/PBCH/DPCH_MXN_2022.h>

DPCH_MXN_2022::DPCH_MXN_2022(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn):PbcScheme(_G1, _G2, _GT, _Zn), ma_abe(_G1, _G2, _GT, _Zn), bls(_G1, _G2, _GT, _Zn) {}


/**
 * @param pp: public parameters
 * @param pkDPCH: public key of DPCH
 * @param skDPCH: secret key of DPCH
 * @param k: key size
 */
void DPCH_MXN_2022::SetUp(DPCH_MXN_2022_pp *pp, DPCH_MXN_2022_pk *pkDPCH, DPCH_MXN_2022_sk *skDPCH, int k) {
    element_random(tmp_G);

    ch_et.SetUp(pp->getPPCH(), k);
    ch_et.KeyGen(pkDPCH->getPkCH(), skDPCH->getSkCH(), pp->getPPCH());

    ma_abe.GlobalSetup(pp->getGpkMA_ABE(), tmp_G);

    bls.Setup(pp->getPPBLS(), tmp_G);
    bls.KeyGen(pkDPCH->getPkBLS(), skDPCH->getSkBLS(), pp->getPPBLS());
}

/**
 * @param skGid: secret key of gid
 * @param sigmaGid: signature of gid
 * @param skDPCH: secret key of DPCH
 * @param gid: global id
 */
void DPCH_MXN_2022::ModSetUp(DPCH_MXN_2022_skGid *skGid, DPCH_MXN_2022_sigmaGid *sigmaGid, DPCH_MXN_2022_sk *skDPCH, std::string gid){
    skGid->getSkCH()->insertElement("d0", skDPCH->getSkCH()->getElement("d0"));
    bls.Sign(sigmaGid->getSignature(), skDPCH->getSkBLS(), gid);
}

/**
 * @param pkTheta: public key of Theta
 * @param skTheta: secret key of Theta
 * @param pp: public parameters
 * @param A: attribute
 */
void DPCH_MXN_2022::AuthSetUp(DPCH_MXN_2022_pkTheta *pkTheta, DPCH_MXN_2022_skTheta *skTheta, DPCH_MXN_2022_pp *pp, std::string A){
    ma_abe.AuthSetup(pkTheta->getPk(), skTheta->getSk(), pp->getGpkMA_ABE(), A);
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
void DPCH_MXN_2022::ModKeyGen(DPCH_MXN_2022_skGidA *skGidA, DPCH_MXN_2022_pp *pp, DPCH_MXN_2022_pk *pkDPCH, std::string gid, DPCH_MXN_2022_sigmaGid *sigmaGid, DPCH_MXN_2022_skTheta *skTheta, std::string A){
    if(!(bls.Verify(pp->getPPBLS(), pkDPCH->getPkBLS(), gid, sigmaGid->getSignature()))){
        throw std::runtime_error("ModKeyGen(): Signature Verify failed!");
    }

    ma_abe.KeyGen(skGidA->getSk(), pp->getGpkMA_ABE(), skTheta->getSk(), gid, A);
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
void DPCH_MXN_2022::Hash(DPCH_MXN_2022_h *h, DPCH_MXN_2022_r *r, DPCH_MXN_2022_c *c, DPCH_MXN_2022_pp *pp, DPCH_MXN_2022_pk *pkDPCH, std::string m, std::vector<DPCH_MXN_2022_pkTheta *> *pkThetas, std::string polocy){
    CH_ET_BC_CDK_2017_etd etd;

    ch_et.Hash(h->getH(), r->getR(), &etd, pp->getPPCH(), pkDPCH->getPkCH(), m);

    aes.KGen(tmp_GT, 256);
    mpz_t c_etd;
    mpz_init(c_etd);
    aes.Enc(c_etd, tmp_GT, etd.getElement("d1"));
    c->getC_etd()->insertElement("d1", c_etd);
    mpz_clear(c_etd);

    std::vector<MA_ABE_pkTheta *> pkThetas_ABE;
    for(int i=0;i<pkThetas->size();i++){
        pkThetas_ABE.push_back(pkThetas->at(i)->getPk());
    }
    ma_abe.Encrypt(c->getC_abe(), pp->getGpkMA_ABE(), &pkThetas_ABE, polocy, tmp_GT);
}

/**
 * @param pkDPCH: public key of DPCH
 * @param m: message
 * @param h: hash value
 * @param r: random value
 * @return bool
 */
bool DPCH_MXN_2022::Check(DPCH_MXN_2022_pk *pkDPCH, std::string m, DPCH_MXN_2022_h *h, DPCH_MXN_2022_r *r){
    return ch_et.Check(h->getH(), r->getR(), pkDPCH->getPkCH(), m);
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
void DPCH_MXN_2022::Adapt(DPCH_MXN_2022_r *r_p, DPCH_MXN_2022_pk *pkDPCH, DPCH_MXN_2022_skGid *skGid, std::vector<DPCH_MXN_2022_skGidA *> *skGidAs, DPCH_MXN_2022_c *c, std::string m, std::string m_p, DPCH_MXN_2022_h *h, DPCH_MXN_2022_r *r){
    if(m == m_p){
        r_p->getR()->insertElement("r0", r->getR()->getElement("r0"));
        r_p->getR()->insertElement("r1", r->getR()->getElement("r1"));
        return;
    }

    std::vector<MA_ABE_skgidA *> skgidAs_ABE;
    for(int i=0;i<skGidAs->size();i++){
        skgidAs_ABE.push_back(skGidAs->at(i)->getSk());
    }
    ma_abe.Decrypt(tmp_GT, &skgidAs_ABE, c->getC_abe());

    CH_ET_BC_CDK_2017_etd etd;
    mpz_t c_etd;
    mpz_init(c_etd);
    aes.Dec(c_etd, tmp_GT, c->getC_etd()->getElement("d1"));
    etd.insertElement("d1", c_etd);
    mpz_clear(c_etd);

    ch_et.Adapt(r_p->getR(), skGid->getSkCH(), &etd, pkDPCH->getPkCH(), h->getH(), r->getR(), m, m_p);
}

/**
 * @param pkDPCH: public key of DPCH
 * @param m_p: message
 * @param h: hash value
 * @param r_p: random value
 * @return bool
 */
bool DPCH_MXN_2022::Verify(DPCH_MXN_2022_pk *pkDPCH, std::string m_p, DPCH_MXN_2022_h *h, DPCH_MXN_2022_r *r_p){
    return Check(pkDPCH, m_p, h, r_p);
}


DPCH_MXN_2022::~DPCH_MXN_2022() {}