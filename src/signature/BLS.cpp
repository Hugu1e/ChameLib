#include <signature/BLS.h>

BLS::BLS(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn): PbcScheme(_G1,_G2,_GT,_Zn){}

/**
 * @param[out] pp public parameters
 */
void BLS::Setup(BLS_pp *pp)
{
    element_t g;
    element_init_same_as(g, G1);
    element_random(g);
    pp->insertElement("g", "G1", g);
    element_clear(g);
}
/**
 * @param[out] pp public parameters
 * @param[in] g generator g
 */
void BLS::Setup(BLS_pp *pp, element_t g)
{
    pp->insertElement("g", "G1", g);
}

/**
 * @param[out] pk public key
 * @param[out] sk secret key
 * @param[in] pp public parameters
 */
void BLS::KeyGen(BLS_pk *pk, BLS_sk *sk, BLS_pp *pp)
{
    element_t a;
    element_init_same_as(a, Zn);
    element_random(a);
    sk->insertElement("a", "Zn", a);
    // g^a
    element_t y;
    element_init_same_as(y, G1);
    element_pow_zn(y, pp->getElement("g"), sk->getElement("a"));
    pk->insertElement("y", "G1", y);
    element_clear(y);
    element_clear(a);
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
void BLS::Sign(BLS_signature *signature, BLS_sk *sk, std::string message)
{
    element_t tmp_H;
    element_init_same_as(tmp_H, G2);
    H(tmp_H, message);
    element_t sigma;
    element_init_same_as(sigma, G2);
    element_pow_zn(sigma, tmp_H, sk->getElement("a"));
    signature->insertElement("sigma", "G2", sigma);
    element_clear(tmp_H);
    element_clear(sigma);
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
bool BLS::Verify(BLS_pp *pp, BLS_pk *pk, std::string message, BLS_signature *signature)
{
    element_t tmp_GT, tmp_H, tmp_GT_2;
    element_init_same_as(tmp_GT, GT);
    element_init_same_as(tmp_H, G2);
    element_init_same_as(tmp_GT_2, GT);
    element_pairing(tmp_GT, pp->getElement("g"), signature->getElement("sigma"));
    H(tmp_H, message);
    element_pairing(tmp_GT_2, pk->getElement("y"), tmp_H);
    bool res = element_cmp(tmp_GT, tmp_GT_2) == 0;
    element_clear(tmp_GT);
    element_clear(tmp_H);
    element_clear(tmp_GT_2);
    return res;
}

BLS::~BLS() {}
