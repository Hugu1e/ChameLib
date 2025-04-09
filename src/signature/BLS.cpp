#include "signature/BLS.h"

BLS::BLS(int curve, bool swap): PbcScheme(curve){
    this->swap = swap;
    if(swap){
        element_init_G2(G1, pairing);
        element_init_G1(G2, pairing);
    }else{
        element_init_G1(G1, pairing);
        element_init_G2(G2, pairing);
    }
    element_init_GT(GT, pairing);
    element_init_Zr(Zn, pairing);

    element_init_same_as(tmp_G, G1);
    element_init_same_as(tmp_H, G2);
    element_init_same_as(tmp_H_2, G2);
    element_init_same_as(tmp_GT, GT);
    element_init_same_as(tmp_GT_2, GT);
    element_init_same_as(tmp_Zn, Zn);
}

void BLS::init(element_t _G, element_t _H, element_t _GT, element_t _Zn){
    this->swap = false;
    element_init_same_as(G1, _G);
    element_init_same_as(G2, _H);
    element_init_same_as(GT, _GT);
    element_init_same_as(Zn, _Zn);

    element_init_same_as(tmp_G, G1);
    element_init_same_as(tmp_H, G2);
    element_init_same_as(tmp_H_2, G2);
    element_init_same_as(tmp_GT, GT);
    element_init_same_as(tmp_GT_2, GT);
    element_init_same_as(tmp_Zn, Zn);
}

void BLS::Pairing(element_t res, element_t a, element_t b){
    if(swap){
        element_pairing(res, b, a);
    }else{
        element_pairing(res, a, b);
    }
}

/**
 * @brief Setup the public parameters
 * 
 * @param  pp[out]  public parameters
 * 
 */
void BLS::Setup(BLS_pp &pp)
{
    element_random(tmp_G);
    pp.set(g, tmp_G);
}

/**
 * @brief Setup the public parameters
 * 
 * @param  pp[out]  public parameters
 * @param  _g[in]   generator g
 * 
 */
void BLS::Setup(BLS_pp &pp, element_t _g)
{    
    pp.set(g, _g);
}

/**
 * @brief Generate the public key and secret key
 * 
 * @param  pk[out]  public key
 * @param  sk[out]  secret key
 * @param  pp[in]   public parameters
 * 
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
 * @param  signature[out]  signature of message
 * @param  sk[in]          secret key
 * @param  message[in]     message to sign
 * 
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
 * @param  pp[in]         public parameters
 * @param  pk[in]         public key
 * @param  message[in]    message to verify
 * @param  signature[in]  signature of message
 * 
 * @return true if signature is valid
 */
bool BLS::Verify(BLS_pp &pp, BLS_pk &pk, std::string message, BLS_signature &signature)
{
    element_t tmp_GT, tmp_H, tmp_GT_2;
    element_init_same_as(tmp_GT, GT);
    element_init_same_as(tmp_H, G2);
    element_init_same_as(tmp_GT_2, GT);
    Pairing(tmp_GT, pp[g], signature[sigma]);
    H(tmp_H, message);
    Pairing(tmp_GT_2, pk[y], tmp_H);
    bool res = element_cmp(tmp_GT, tmp_GT_2) == 0;
    element_clear(tmp_GT);
    element_clear(tmp_H);
    element_clear(tmp_GT_2);
    return res;
}

BLS::~BLS() {
    element_clear(tmp_G);
    element_clear(tmp_H);
    element_clear(tmp_H_2);
    element_clear(tmp_GT);
    element_clear(tmp_GT_2);
    element_clear(tmp_Zn);

    element_clear(G1);
    element_clear(G2);
    element_clear(GT);
    element_clear(Zn);
}
