#include <scheme/PBCH/PCHBA_TLL_2020.h>

PCHBA_TLL_2020::PCHBA_TLL_2020(int curve, bool swap): PbcScheme(curve){
    this->swap = swap;
    if(swap){
        element_init_G1(G2, pairing);
        element_init_G2(G1, pairing);
    }else{
        element_init_G1(G1, pairing);
        element_init_G2(G2, pairing);
    }
    element_init_GT(GT, pairing);
    element_init_Zr(Zn, pairing);

    abet.init(G1, G2, GT, Zn, swap);

    element_init_same_as(this->r, Zn);
    element_init_same_as(this->R, Zn);
    element_init_same_as(this->s1, Zn);
    element_init_same_as(this->s2, Zn);
    element_init_same_as(this->esk, Zn);

    element_init_same_as(this->tmp_G, G1);
    element_init_same_as(this->tmp_G_2, G1);
    element_init_same_as(this->tmp_H, G2);
    element_init_same_as(this->tmp_H_2, G2);
    element_init_same_as(this->tmp_GT, GT);
    element_init_same_as(this->tmp_GT_2, GT);
    element_init_same_as(this->tmp_GT_3, GT);
    element_init_same_as(this->tmp_Zn, Zn);
    element_init_same_as(this->tmp_Zn_2, Zn);
    element_init_same_as(this->tmp_Zn_3, Zn);
}

bool PCHBA_TLL_2020::Pairing(element_t res, element_t a, element_t b){
    if(swap){
        element_pairing(res, b, a);
    }else{
        element_pairing(res, a, b);
    }
}

/**
 * input: k
 * output: skPCHBA, pkPCHBA
 */
void PCHBA_TLL_2020::SetUp(PCHBA_TLL_2020_pk &pkPCHBA, PCHBA_TLL_2020_sk &skPCHBA, PCHBA_TLL_2020_sks &sksPCHBA, PCHBA_TLL_2020_h &h, PCHBA_TLL_2020_h &h_p, int k) {
    pkPCHBA.get_pkCHET().init(1);
    skPCHBA.get_skCHET().init(1);
    sksPCHBA.get_skCHET().init(1);
    h.get_h().init(1);
    h.get_r().init(5);
    h_p.get_h().init(1);
    h_p.get_r().init(5);
    
    this->k = k;

    abet.Setup(skPCHBA.get_skABET(), pkPCHBA.get_pkABET(), sksPCHBA.get_sksABET(), h.get_r().get_C(), k);
    h_p.get_r().get_C().init_same_as(h.get_r().get_C());

    element_random(tmp_Zn);
    skPCHBA.get_skCHET().set(x, tmp_Zn);

    // h^x
    element_pow_zn(tmp_H, pkPCHBA.get_pkABET()[ABET::h], tmp_Zn);
    pkPCHBA.get_pkCHET().set(h_pow_x, tmp_H);
}

/**
 * input : skPCHBA, pkPCHBA, attr_list, ID, mi
 * output: sksPCHBA
 */
void PCHBA_TLL_2020::KeyGen(PCHBA_TLL_2020_sks &sksPCHBA, PCHBA_TLL_2020_pk &pkPCHBA, PCHBA_TLL_2020_sk &skPCHBA, std::vector<std::string> &attr_list, PCHBA_TLL_2020_ID &ID, int mi) {
    sksPCHBA.get_skCHET().set(x, skPCHBA.get_skCHET()[x]);
   
    abet.KeyGen(sksPCHBA.get_sksABET(), skPCHBA.get_skABET(), pkPCHBA.get_pkABET(), attr_list, ID.get_IDABET(), mi);
}
 
/**
 * input : pkPCHBA, skPCHBA, m, policy_str, ID, oj
 * output: p, h', b(hash), C, c, epk, sigma
 */
void PCHBA_TLL_2020::Hash(PCHBA_TLL_2020_h &h, element_t m, PCHBA_TLL_2020_pk &pkPCHBA, PCHBA_TLL_2020_sk &skPCHBA, std::string policy_str, PCHBA_TLL_2020_ID &ID, int oj) {
    element_random(this->r);
    element_random(this->R);
    // p = pk^r
    element_pow_zn(tmp_H_2, pkPCHBA.get_pkCHET()[h_pow_x], this->r);
    h.get_r().set(p, tmp_H_2);
    // e = H2(R)
    abet.Hash2(this->tmp_Zn, this->R);
    // h' = h^e
    element_pow_zn(tmp_H, pkPCHBA.get_pkABET()[ABET::h], this->tmp_Zn);
    h.get_r().set(h_, tmp_H);
    // b = p * (h'^m)
    element_pow_zn(this->tmp_H, tmp_H, m);
    element_mul(tmp_H_2, tmp_H_2, this->tmp_H);
    h.get_h().set(b, tmp_H_2);

    element_random(this->s1);
    element_random(this->s2);
    // C
    // Logger::PrintPbc("Encrypt:R", this->R);
    // Logger::PrintPbc("Encrypt:r", this->r);
    abet.Encrypt(h.get_r().get_C(), pkPCHBA.get_pkABET(), skPCHBA.get_skABET(), this->r, this->R, policy_str, ID.get_IDABET(), oj, this->s1, this->s2);

    // c = h^(s1+s2+R)
    element_add(this->tmp_Zn, this->s1, this->s2);
    element_add(this->tmp_Zn_2, this->tmp_Zn, this->R);
    element_pow_zn(tmp_H, pkPCHBA.get_pkABET()[ABET::h], this->tmp_Zn_2);
    h.get_r().set(c, tmp_H);

    // esk
    element_random(this->esk);
    // epk = g^esk
    element_pow_zn(tmp_G, pkPCHBA.get_pkABET()[ABET::g], this->esk);
    h.get_r().set(epk, tmp_G);

    // epk_str + c_str
    unsigned char bytes_epk[element_length_in_bytes(tmp_G)];
    unsigned char bytes_c[element_length_in_bytes(tmp_H)];
    element_to_bytes(bytes_epk, tmp_G);
    element_to_bytes(bytes_c, tmp_H);
    std::string epk_str((char *)bytes_epk, element_length_in_bytes(tmp_G));
    std::string c_str((char *)bytes_c, element_length_in_bytes(tmp_H));
    std::string combine = epk_str + c_str;
    // sigma = esk + (s1 + s2) * H2(epk||c)
    this->abet.Hash(this->tmp_Zn_2, combine);
    element_mul(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    element_add(tmp_Zn, this->esk, this->tmp_Zn);
    h.get_r().set(sigma, tmp_Zn);
}

/**
 * input : pkPCHBA, m, p, h', b, C, c, epk, sigma
 * output: bool
 */
bool PCHBA_TLL_2020::Check(PCHBA_TLL_2020_h &h, element_t m, PCHBA_TLL_2020_pk &pkPCHBA) {
    // b =? p * (h'^m)
    element_pow_zn(this->tmp_H, h.get_r()[h_], m);
    element_mul(this->tmp_H_2, h.get_r()[p], this->tmp_H);
    if (element_cmp(h.get_h()[b], this->tmp_H_2) != 0) {
        return false;
    }
    // e(g^a, ct2)^sigma =? e(epk,ct1) * e(g,ct3)^(H2(epk||c))
    Pairing(this->tmp_GT, pkPCHBA.get_pkABET()[ABET::g_pow_a], h.get_r().get_C().get_ct2()[0]);
    element_pow_zn(this->tmp_GT, this->tmp_GT, h.get_r()[sigma]);

    Pairing(this->tmp_GT_2, h.get_r()[epk], h.get_r().get_C().get_ct1()[0]);

    Pairing(this->tmp_GT_3, pkPCHBA.get_pkABET()[ABET::g], h.get_r().get_C().get_ct3()[0]);
    // epk_str + c_str
    unsigned char bytes_epk[element_length_in_bytes(h.get_r()[epk])];
    unsigned char bytes_c[element_length_in_bytes(h.get_r()[c])];
    element_to_bytes(bytes_epk, h.get_r()[epk]);
    element_to_bytes(bytes_c, h.get_r()[c]);
    std::string epk_str((char *)bytes_epk, element_length_in_bytes(h.get_r()[epk]));
    std::string c_str((char *)bytes_c, element_length_in_bytes(h.get_r()[c]));
    std::string combine = epk_str + c_str;
    this->abet.Hash(this->tmp_Zn, combine);
    element_pow_zn(this->tmp_GT_3, this->tmp_GT_3, this->tmp_Zn);

    element_mul(this->tmp_GT_2, this->tmp_GT_2, this->tmp_GT_3);

    if (element_cmp(this->tmp_GT, this->tmp_GT_2) != 0) {
        return false;
    }
    return true;
}

/**
 * input : pkPCHBA, skPCHBA, sksPCHBA, m, p, h', b, C, c, epk, sigma, m_p, policy_str, ID, mi
 * output: p_p, C_p, c_p, epk_p, sigma_p
 */
void PCHBA_TLL_2020::Adapt(PCHBA_TLL_2020_h &h_p, element_t m_p, PCHBA_TLL_2020_h &h, element_t m, std::string policy_str, PCHBA_TLL_2020_ID &ID, int mi, PCHBA_TLL_2020_pk &pkPCHBA, PCHBA_TLL_2020_sk &skPCHBA, PCHBA_TLL_2020_sks &sksPCHBA) {
    // check
    if (!this->Check(h, m, pkPCHBA)) {
        throw std::runtime_error("Hash Check failed");
    }

    // retrieve R, r
    this->abet.Decrypt(this->R, this->r, pkPCHBA.get_pkABET(), h.get_r().get_C(), sksPCHBA.get_sksABET());
    // PrintElement("Decrypt:R", this->R);
    // PrintElement("Decrypt:r", this->r);

    // e = H2(R)
    this->abet.Hash2(this->tmp_Zn, this->R);
    // r' = r + (m-m')*e/x
    element_sub(this->tmp_Zn_2, m, m_p);
    element_mul(this->tmp_Zn_2, this->tmp_Zn_2, this->tmp_Zn);
    element_div(this->tmp_Zn_2, this->tmp_Zn_2, sksPCHBA.get_skCHET()[x]);
    element_add(this->tmp_Zn_3, this->r, this->tmp_Zn_2);
    // p' = pk^r'
    element_pow_zn(tmp_H, pkPCHBA.get_pkCHET()[h_pow_x], this->tmp_Zn_3);
    h_p.get_r().set(p, tmp_H);

    // s1', s2'
    element_random(this->s1);
    element_random(this->s2);
    // s' = s1' + s2'
    element_add(this->tmp_Zn, this->s1, this->s2);

    // C'
    abet.Encrypt(h_p.get_r().get_C(), pkPCHBA.get_pkABET(), skPCHBA.get_skABET(), this->tmp_Zn_3, this->R, policy_str, ID.get_IDABET(), mi, this->s1, this->s2);

    // esk'
    element_random(this->esk);
    // epk' = g^esk'
    element_pow_zn(tmp_G, pkPCHBA.get_pkABET()[ABET::g], this->esk);
    h_p.get_r().set(epk, tmp_G);

    // c' = h^(s1'+s2'+R)
    element_add(this->tmp_Zn, this->s1, this->s2);
    element_add(this->tmp_Zn_2, this->tmp_Zn, this->R);
    element_pow_zn(tmp_H, pkPCHBA.get_pkABET()[ABET::h], this->tmp_Zn_2);
    h_p.get_r().set(c, tmp_H);

    // epk_str + c_str
    unsigned char bytes_epk[element_length_in_bytes(tmp_G)];
    unsigned char bytes_c[element_length_in_bytes(tmp_H)];
    element_to_bytes(bytes_epk, tmp_G);
    element_to_bytes(bytes_c, tmp_H);
    std::string epk_str((char *)bytes_epk, element_length_in_bytes(tmp_G));
    std::string c_str((char *)bytes_c, element_length_in_bytes(tmp_H));
    std::string combine = epk_str + c_str;
    // sigma' = esk' + (s1' + s2') * H2(epk'||c')
    this->abet.Hash(this->tmp_Zn_2, combine);
    element_mul(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    element_add(tmp_Zn, this->esk, this->tmp_Zn);
    h_p.get_r().set(sigma, tmp_Zn);

    // h_
    h_p.get_r().set(h_, h.get_r()[h_]);
    // b
    h_p.get_h().set(b, h.get_h()[b]);
}

/**
 * input : pkPCHBA, m_p, p_p, h', b, C_p, c_p, epk_p, sigma_p
 * output: bool
 */
bool PCHBA_TLL_2020::Verify(PCHBA_TLL_2020_h &h_p, element_t m_p, PCHBA_TLL_2020_pk &pkPCHBA) {
    return this->Check(h_p, m_p, pkPCHBA);
}

PCHBA_TLL_2020::~PCHBA_TLL_2020() {
    element_clear(this->r);
    element_clear(this->R);
    element_clear(this->s1);
    element_clear(this->s2);
    element_clear(this->esk);   

    element_clear(this->tmp_G);
    element_clear(this->tmp_G_2);
    element_clear(this->tmp_H);
    element_clear(this->tmp_H_2);
    element_clear(this->tmp_GT);
    element_clear(this->tmp_GT_2);
    element_clear(this->tmp_GT_3);
    element_clear(this->tmp_Zn);
    element_clear(this->tmp_Zn_2);
    element_clear(this->tmp_Zn_3);

    element_clear(this->G1);
    element_clear(this->G2);
    element_clear(this->GT);
    element_clear(this->Zn);
}
