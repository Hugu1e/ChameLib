#include <scheme/CH/CH_ET_KOG_CDK_2017.h>

CH_ET_KOG_CDK_2017_NIZKPOK::CH_ET_KOG_CDK_2017_NIZKPOK(){}

void CH_ET_KOG_CDK_2017_NIZKPOK::init(element_t _G, element_t _Zn){
    PbcScheme::init(_G, _Zn);

    element_init_same_as(tmp_G, _G);
    element_init_same_as(tmp_G_2, _G);
    element_init_same_as(tmp_Zn, _Zn);
    element_init_same_as(tmp_Zn_2, _Zn);
}

/**
 * @param[out] z
 * @param[out] R
 * @param[in] pk
 * @param[in] sk
 * @param[in] g
 */
void CH_ET_KOG_CDK_2017_NIZKPOK::proof(element_t z, element_t R, element_t pk, element_t sk, element_t g){
    // r
    element_random(tmp_Zn);

    // R = g^r
    element_pow_zn(R, g, tmp_Zn);

    // c = H(pk, R)
    HASH::hash(tmp_Zn_2, pk, R);

    // z = r + c*sk
    element_mul(tmp_Zn_2, tmp_Zn_2, sk);
    element_add(z, tmp_Zn, tmp_Zn_2);
}

/**
 * @param[in] z
 * @param[in] R
 * @param[in] pk
 * @param[in] g
 */
bool CH_ET_KOG_CDK_2017_NIZKPOK::verify(element_t z, element_t R, element_t pk, element_t g){
    // g^z
    element_pow_zn(tmp_G, g, z);

    // c = H(pk, R)
    HASH::hash(tmp_Zn, pk, R);
    // R * pk^c
    element_pow_zn(tmp_G_2, pk, tmp_Zn);
    element_mul(tmp_G_2, R, tmp_G_2);

    return element_cmp(tmp_G, tmp_G_2) == 0;
}

CH_ET_KOG_CDK_2017_NIZKPOK::~CH_ET_KOG_CDK_2017_NIZKPOK(){
    element_clear(tmp_G);
    element_clear(tmp_G_2);
    element_clear(tmp_Zn);
    element_clear(tmp_Zn_2);

    element_clear(this->G1);
    element_clear(this->Zn);
}


CH_ET_KOG_CDK_2017::CH_ET_KOG_CDK_2017(int curve, int group): PbcScheme(curve){
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
            throw std::invalid_argument("CH_ET_KOG_CDK_2017::CH_ET_KOG_CDK_2017(): Invalid group type");
    }
    element_init_Zr(Zn, pairing);

    element_init_same_as(tmp_G, G1);
    element_init_same_as(tmp_G_2, G1);
    element_init_same_as(tmp_G_3, G1);
    element_init_same_as(tmp_Zn, Zn);
    element_init_same_as(tmp_Zn_2, Zn);
    element_init_same_as(tmp_Zn_3, Zn);
    element_init_same_as(tmp_Zn_4, Zn);

    nizkpok.init(G1, Zn);
}

void CH_ET_KOG_CDK_2017::SetUp(CH_ET_KOG_CDK_2017_pp &pp, CH_ET_KOG_CDK_2017_sk &sk, CH_ET_KOG_CDK_2017_pk &pk, 
    CH_ET_KOG_CDK_2017_h &h, CH_ET_KOG_CDK_2017_etd &etd, CH_ET_KOG_CDK_2017_r &r, CH_ET_KOG_CDK_2017_r &r_p){
    pp.init(1);
    sk.get_ch_sk().init(1);
    pk.get_ch_pk().init(1);
    pk.get_pai_pk().init(2);

    h.get_hash().init(2);
    h.get_pai_t().init(2);
    r.get_ch_r().init(1);
    r.get_pai_p().init(2);
    r_p.get_ch_r().init(1);
    r_p.get_pai_p().init(2);

    element_random(tmp_G);
    pp.set(g, tmp_G);

    enc.SetUp(pk.get_enc_pk(), sk.get_enc_sk(), r.get_enc_c());
    r_p.get_enc_c().init_same_as(r.get_enc_c());

    etd.init(1);
}

void CH_ET_KOG_CDK_2017::KeyGen(CH_ET_KOG_CDK_2017_sk &sk, CH_ET_KOG_CDK_2017_pk &pk, CH_ET_KOG_CDK_2017_pp &pp){
    // x
    element_random(tmp_Zn);
    sk.get_ch_sk().set(x, tmp_Zn);

    // h = g^x
    element_pow_zn(tmp_G, pp[g], tmp_Zn);
    pk.get_ch_pk().set(h, tmp_G);

    enc.KeyGen(pk.get_enc_pk(), sk.get_enc_sk(), 1024);

    nizkpok.proof(tmp_Zn, tmp_G, pk.get_ch_pk()[h], sk.get_ch_sk()[x], pp[g]);
    pk.get_pai_pk().set(pai_z, tmp_Zn);
    pk.get_pai_pk().set(pai_R, tmp_G);
}

/**
 * @param[out] hash
 * @param[out] r
 * @param[out] etd
 * @param[in] m
 * @param[in] pk
 * @param[in] pp
 */
void CH_ET_KOG_CDK_2017::Hash(CH_ET_KOG_CDK_2017_h &hash, CH_ET_KOG_CDK_2017_r &r, CH_ET_KOG_CDK_2017_etd &etd, 
    element_t m, CH_ET_KOG_CDK_2017_pk &pk, CH_ET_KOG_CDK_2017_pp &pp){
    if(!nizkpok.verify(pk.get_pai_pk()[pai_z], pk.get_pai_pk()[pai_R], pk.get_ch_pk()[h], pp[g])){
        throw std::runtime_error("CH_ET_KOG_CDK_2017::Hash(): NIZKPOK_verify failed");
    }

    // r
    element_random(tmp_Zn);
    // etd
    element_random(tmp_Zn_2);
    etd.set(0, tmp_Zn_2);
    // h' = g^etd
    element_pow_zn(tmp_G, pp[g], tmp_Zn_2);
    hash.get_hash().set(h_, tmp_G);
    // pai_t
    nizkpok.proof(tmp_Zn_3, tmp_G_2, tmp_G, tmp_Zn_2, pp[g]);
    hash.get_pai_t().set(pai_z, tmp_Zn_3);
    hash.get_pai_t().set(pai_R, tmp_G_2);

    // Encrypt r
    mpz_t r_mpz;
    mpz_init(r_mpz);
    element_to_mpz(r_mpz, tmp_Zn);
    enc.Encrypt(r.get_enc_c(), r_mpz, pk.get_enc_pk());
    mpz_clear(r_mpz);

    // a = H(m)

    // p = h^r
    element_pow_zn(tmp_G_2, pk.get_ch_pk()[h], tmp_Zn);
    r.get_ch_r().set(p, tmp_G_2);

    // pai_p
    nizkpok.proof(tmp_Zn_3, tmp_G_3, tmp_G_2, tmp_Zn, pk.get_ch_pk()[h]);
    r.get_pai_p().set(pai_z, tmp_Zn_3);
    r.get_pai_p().set(pai_R, tmp_G_3);

    // b = p * (h'^a)
    element_pow_zn(tmp_G_3, tmp_G, m);
    element_mul(tmp_G_3, tmp_G_2, tmp_G_3);
    hash.get_hash().set(b, tmp_G_3);
}


bool CH_ET_KOG_CDK_2017::Check(CH_ET_KOG_CDK_2017_h &hash, element_t m, CH_ET_KOG_CDK_2017_r &r, CH_ET_KOG_CDK_2017_pk &pk, CH_ET_KOG_CDK_2017_pp &pp){
    // pai_p
    if(!nizkpok.verify(r.get_pai_p()[pai_z], r.get_pai_p()[pai_R], r.get_ch_r()[p], pk.get_ch_pk()[h])){
        return false;
    }
    // pai_t
    if(!nizkpok.verify(hash.get_pai_t()[pai_z], hash.get_pai_t()[pai_R], hash.get_hash()[h_], pp[g])){
        return false;
    }
    // pai_pk
    if(!nizkpok.verify(pk.get_pai_pk()[pai_z], pk.get_pai_pk()[pai_R], pk.get_ch_pk()[h], pp[g])){
        return false;
    }

    // a = H(m)
    // b = p * (h'^a)
    element_pow_zn(tmp_G, hash.get_hash()[h_], m);
    element_mul(tmp_G, r.get_ch_r()[p], tmp_G);
    
    return element_cmp(tmp_G, hash.get_hash()[b]) == 0;
}


void CH_ET_KOG_CDK_2017::Adapt(CH_ET_KOG_CDK_2017_r &r_p, element_t m_p, CH_ET_KOG_CDK_2017_h &hash, element_t m, CH_ET_KOG_CDK_2017_r &r, 
    CH_ET_KOG_CDK_2017_pk &pk, CH_ET_KOG_CDK_2017_pp &pp, CH_ET_KOG_CDK_2017_sk &sk, CH_ET_KOG_CDK_2017_etd &etd){
    
    // Check
    if(!Check(hash, m, r, pk, pp)){
        throw std::runtime_error("CH_ET_KOG_CDK_2017::Adapt(): Hash check failed");
    }

    // Decrypt C
    mpz_t r_mpz;
    mpz_init(r_mpz);
    enc.Decrypt(r_mpz, r.get_enc_c(), sk.get_enc_sk(), pk.get_enc_pk());
    element_set_mpz(tmp_Zn, r_mpz);

    // h' ?= g^etd 
    element_pow_zn(tmp_G, pp[g], etd[0]);
    if(element_cmp(tmp_G, hash.get_hash()[h_]) != 0){
        throw std::runtime_error("CH_ET_KOG_CDK_2017::Adapt(): h' != g^etd");
    }

    // a = H(m)
    // a' = H(m')

    // p ?= g^(xr)
    element_mul(tmp_Zn_2, sk.get_ch_sk()[x], tmp_Zn);
    element_pow_zn(tmp_G, pp[g], tmp_Zn_2);
    if(element_cmp(tmp_G, r.get_ch_r()[p]) != 0){
        throw std::runtime_error("CH_ET_KOG_CDK_2017::Adapt(): p != g^(xr)");
    }

    // a = a'
    if(element_cmp(m, m_p) == 0){
        r_p.get_ch_r().set(p, r.get_ch_r()[p]);
        r_p.get_pai_p().set(pai_z, r.get_pai_p()[pai_z]);
        r_p.get_pai_p().set(pai_R, r.get_pai_p()[pai_R]);
        r_p.get_enc_c() = r.get_enc_c();
        return;
    }

    // r' = (rx + a*etd - a'*etd) / x
    element_mul(tmp_Zn_2, sk.get_ch_sk()[x], tmp_Zn);
    element_mul(tmp_Zn_3, m, etd[0]);
    element_mul(tmp_Zn_4, m_p, etd[0]);
    element_add(tmp_Zn_2, tmp_Zn_2, tmp_Zn_3);
    element_sub(tmp_Zn_2, tmp_Zn_2, tmp_Zn_4);
    element_div(tmp_Zn_2, tmp_Zn_2, sk.get_ch_sk()[x]);
    // p' = h^r'
    element_pow_zn(tmp_G, pk.get_ch_pk()[h], tmp_Zn_2);
    r_p.get_ch_r().set(p, tmp_G);

    // Encrypt r'
    mpz_t r_p_mpz;
    mpz_init(r_p_mpz);
    element_to_mpz(r_p_mpz, tmp_Zn_2);
    enc.Encrypt(r_p.get_enc_c(), r_p_mpz, pk.get_enc_pk());
    mpz_clear(r_p_mpz);

    // pai_p'
    nizkpok.proof(tmp_Zn_3, tmp_G, r_p.get_ch_r()[p], tmp_Zn_2, pk.get_ch_pk()[h]);
    r_p.get_pai_p().set(pai_z, tmp_Zn_3);
    r_p.get_pai_p().set(pai_R, tmp_G);
}

bool CH_ET_KOG_CDK_2017::Verify(CH_ET_KOG_CDK_2017_h &hash, element_t m_p, CH_ET_KOG_CDK_2017_r &r_p, CH_ET_KOG_CDK_2017_pk &pk, CH_ET_KOG_CDK_2017_pp &pp){
    return Check(hash, m_p, r_p, pk, pp);
}

CH_ET_KOG_CDK_2017::~CH_ET_KOG_CDK_2017() {
    element_clear(tmp_G);
    element_clear(tmp_G_2);
    element_clear(tmp_G_3);
    element_clear(tmp_Zn);
    element_clear(tmp_Zn_2);
    element_clear(tmp_Zn_3);
    element_clear(tmp_Zn_4);

    element_clear(this->G1);
    element_clear(this->Zn);
}
