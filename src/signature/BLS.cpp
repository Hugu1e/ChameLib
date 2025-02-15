#include "signature/BLS.h"

BLS::BLS(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn): PbcScheme(_G1,_G2,_GT,_Zn){}

/**
 * @param[out] pp public parameters
 */
void BLS::Setup(BLS_pp &pp, BLS_pk &pk, BLS_sk &sk, BLS_signature &signature)
{
    pp.init(1);
    pk.init(1);
    sk.init(1);
    signature.init(1);

    element_random(tmp_G);
    pp.set(g, tmp_G);
}
/**
 * @param[out] pp public parameters
 * @param[in] g generator g
 */
void BLS::Setup(BLS_pp &pp, BLS_pk &pk, BLS_sk &sk, BLS_signature &signature, element_t _g)
{
    pp.init(1);
    pk.init(1);
    sk.init(1);
    signature.init(1);
    
    pp.set(g, _g);
}

/**
 * @param[out] pk public key
 * @param[out] sk secret key
 * @param[in] pp public parameters
 */
void BLS::KeyGen(BLS_pk &pk, BLS_sk &sk, BLS_pp &pp)
{
    element_random(tmp_Zn);
    sk.set(a, tmp_Zn);
    // g^a
    element_pow_zn(tmp_G, pp[g], sk[a]);
    pk.set(y, tmp_G);
}

void BLS::H(element_t res, std::string m)
{
    HASH::hash(res, m);
}

/**
 * @brief Sign a message
 *
 * @param[out] signature: signature of message
 * @param[in] sk: secret key
 * @param[in] message: message to sign
 */
void BLS::Sign(BLS_signature &signature, BLS_sk &sk, std::string message)
{
    H(tmp_H, message);
    element_pow_zn(tmp_H_2, tmp_H, sk[a]);
    signature.set(sigma, tmp_H_2);
}

/**
 * @brief Verify a message
 *
 * @param[in] pp: public parameters
 * @param[in] pk: public key
 * @param[in] message: message to verify
 * @param[in] signature: signature of message
 * @return true if signature is valid
 */
bool BLS::Verify(BLS_pp &pp, BLS_pk &pk, std::string message, BLS_signature &signature)
{
    element_t tmp_GT, tmp_H, tmp_GT_2;
    element_init_same_as(tmp_GT, GT);
    element_init_same_as(tmp_H, G2);
    element_init_same_as(tmp_GT_2, GT);
    element_pairing(tmp_GT, pp[g], signature[sigma]);
    H(tmp_H, message);
    element_pairing(tmp_GT_2, pk[y], tmp_H);
    bool res = element_cmp(tmp_GT, tmp_GT_2) == 0;
    element_clear(tmp_GT);
    element_clear(tmp_H);
    element_clear(tmp_GT_2);
    return res;
}

BLS::~BLS() {}
