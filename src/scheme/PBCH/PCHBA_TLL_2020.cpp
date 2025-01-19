#include <scheme/PCHBA_TLL_2020.h>

PCHBA_TLL_2020::PCHBA_TLL_2020(element_t *_G, element_t *_H, element_t *_GT, element_t *_Zn) 
    : abet(_G, _H, _GT, _Zn){
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

    element_init_same_as(this->r, *this->Zn);
    element_init_same_as(this->R, *this->Zn);
    element_init_same_as(this->s1, *this->Zn);
    element_init_same_as(this->s2, *this->Zn);

    element_init_same_as(this->esk, *this->Zn);
}

/**
 * input: k
 * output: skPCHBA, pkPCHBA
 */
void PCHBA_TLL_2020::PG(int k, skPCHBA *skPCHBA, pkPCHBA *pkPCHBA) {
    this->k = k;
    this->abet.Setup(&skPCHBA->skABET, &pkPCHBA->pkABET, k);

    element_random(skPCHBA->skCHET.x);
    // h^x
    element_pow_zn(pkPCHBA->pkCHET.h_pow_x, pkPCHBA->pkABET.h, skPCHBA->skCHET.x);
}

/**
 * input : skPCHBA, pkPCHBA, attr_list, ID, mi
 * output: sksPCHBA
 */
void PCHBA_TLL_2020::KG(skPCHBA *skPCHBA, pkPCHBA *pkPCHBA, std::vector<std::string> *attr_list, ABET::ID *ID, int mi, sksPCHBA *sksPCHBA) {
    element_set(sksPCHBA->x, skPCHBA->skCHET.x);
    this->abet.KeyGen(&skPCHBA->skABET, &pkPCHBA->pkABET, attr_list, ID, mi, &sksPCHBA->sksABET);
}
 

/**
 * input : pkPCHBA, skPCHBA, m, policy_str, ID, oj
 * output: p,h',b(hash),C,c,epk,sigma
 */
void PCHBA_TLL_2020::Hash(pkPCHBA *pkPCHBA, skPCHBA *skPCHBA, element_t *m, string policy_str, ABET::ID *ID, int oj, 
                            element_t *p, element_t *h_, element_t *b, ABET::ciphertext *C, element_t *c, element_t *epk, element_t *sigma) {
    element_random(this->r);
    element_random(this->R);
    // p = pk^r
    element_pow_zn(*p, pkPCHBA->pkCHET.h_pow_x, this->r);
    // e = H2(R)
    this->abet.Hash2(&this->R, &this->tmp_Zn);
    // h' = h^e
    element_pow_zn(*h_, pkPCHBA->pkABET.h, this->tmp_Zn);
    // b = p * (h'^m)
    element_pow_zn(this->tmp_H, *h_, *m);
    element_mul(*b, *p, this->tmp_H);

    element_random(this->s1);
    element_random(this->s2);
    // C
    PrintElement("Encrypt:R", this->R);
    PrintElement("Encrypt:r", this->r);
    this->abet.Encrypt(&pkPCHBA->pkABET, &skPCHBA->skABET, &this->r, &this->R, policy_str, ID, oj, &this->s1, &this->s2, C);

    

    // c = h^(s1+s2+R)
    element_add(this->tmp_Zn, this->s1, this->s2);
    element_add(this->tmp_Zn_2, this->tmp_Zn, this->R);
    element_pow_zn(*c, pkPCHBA->pkABET.h, this->tmp_Zn_2);

    // esk
    element_random(this->esk);
    // epk = g^esk
    element_pow_zn(*epk, pkPCHBA->pkABET.g, this->esk);

    // epk_str + c_str
    unsigned char bytes_epk[element_length_in_bytes(*epk)];
    unsigned char bytes_c[element_length_in_bytes(*c)];
    element_to_bytes(bytes_epk, *epk);
    element_to_bytes(bytes_c, *c);
    string epk_str((char *)bytes_epk, element_length_in_bytes(*epk));
    string c_str((char *)bytes_c, element_length_in_bytes(*c));
    string combine = epk_str + c_str;
    // sigma = esk + (s1 + s2) * H2(epk||c)
    this->abet.Hash(combine, &this->tmp_Zn_2);
    element_mul(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    element_add(*sigma, this->esk, this->tmp_Zn);
}

/**
 * input : pkPCHBA, m, p, h', b, C, c, epk, sigma
 * output: bool
 */
bool PCHBA_TLL_2020::Check(pkPCHBA *pkPCHBA, element_t *m, element_t *p, element_t *h_, element_t *b, ABET::ciphertext *C, element_t *c, element_t *epk, element_t *sigma) {
    // b =? p * (h'^m)
    element_pow_zn(this->tmp_H, *h_, *m);
    element_mul(this->tmp_H_2, *p, this->tmp_H);
    if (element_cmp(*b, this->tmp_H_2) != 0) {
        return false;
    }
    // e(g^a, ct2)^sigma =? e(epk,ct1) * e(g,ct3)^(H2(epk||c))
    element_pairing(this->tmp_GT, pkPCHBA->pkABET.g_pow_a, C->ct2);
    element_pow_zn(this->tmp_GT, this->tmp_GT, *sigma);

    element_pairing(this->tmp_GT_2, *epk, C->ct1);

    element_pairing(this->tmp_GT_3, pkPCHBA->pkABET.g, C->ct3);
    // epk_str + c_str
    unsigned char bytes_epk[element_length_in_bytes(*epk)];
    unsigned char bytes_c[element_length_in_bytes(*c)];
    element_to_bytes(bytes_epk, *epk);
    element_to_bytes(bytes_c, *c);
    string epk_str((char *)bytes_epk, element_length_in_bytes(*epk));
    string c_str((char *)bytes_c, element_length_in_bytes(*c));
    string combine = epk_str + c_str;
    this->abet.Hash(combine, &this->tmp_Zn);
    element_pow_zn(this->tmp_GT_3, this->tmp_GT_3, this->tmp_Zn);

    element_mul(this->tmp_GT_2, this->tmp_GT_2, this->tmp_GT_3);

    if (element_cmp(this->tmp_GT, this->tmp_GT_2) != 0) {
        return false;
    }
    return true;
}

/**
 * input : pkPCHBA, skPCHBA,sksPCHBA, m, p, h', b, C, c, epk, sigma,  m_p, policy_str, ID, mi
 * output: p_p, C_p, c_p, epk_p, sigma_p
 */
void PCHBA_TLL_2020::Forge(pkPCHBA *pkPCHBA, skPCHBA* skPCHBA,sksPCHBA *sksPCHBA, element_t *m, element_t *p, element_t *h_, element_t *b, ABET::ciphertext *C, 
                            element_t *c, element_t *epk, element_t *sigma, string policy_str, ABET::ID *ID, int mi,
                            element_t *m_p, element_t *p_p, ABET::ciphertext *C_p, element_t *c_p, element_t *epk_p, element_t *sigma_p) {
    // check
    if (!this->Check(pkPCHBA, m, p, h_, b, C, c, epk, sigma)) {
        throw std::runtime_error("Hash Check failed");
    }

    // retrive R,r
    this->abet.Decrypt(&pkPCHBA->pkABET, C, &sksPCHBA->sksABET, &this->R, &this->r);
    PrintElement("Decrypt:R", this->R);
    PrintElement("Decrypt:r", this->r);

    // e = H2(R)
    this->abet.Hash2(&this->R, &this->tmp_Zn);
    // r' = r + (m-m')*e/x
    element_sub(this->tmp_Zn_2, *m, *m_p);
    element_mul(this->tmp_Zn_2, this->tmp_Zn_2, this->tmp_Zn);
    element_div(this->tmp_Zn_2, this->tmp_Zn_2, sksPCHBA->x);
    element_add(this->tmp_Zn_3, this->r, this->tmp_Zn_2);
    // p' = pk^r'
    element_pow_zn(*p_p, pkPCHBA->pkCHET.h_pow_x, this->tmp_Zn_3);

    // s1',s2'
    element_random(this->s1);
    element_random(this->s2);
    // s' = s1' + s2'
    element_add(this->tmp_Zn, this->s1, this->s2);

    // C'
    this->abet.Encrypt(&pkPCHBA->pkABET, &skPCHBA->skABET, &this->tmp_Zn_3, &this->R, policy_str, ID, mi, &this->s1, &this->s2, C_p);

    // esk'
    element_random(this->esk);
    // epk' = g^esk'
    element_pow_zn(*epk_p, pkPCHBA->pkABET.g, this->esk);

    // c' = h^(s1'+s2'+R)
    element_add(this->tmp_Zn, this->s1, this->s2);
    element_add(this->tmp_Zn_2, this->tmp_Zn, this->R);
    element_pow_zn(*c_p, pkPCHBA->pkABET.h, this->tmp_Zn_2);

    // epk_str + c_str
    unsigned char bytes_epk[element_length_in_bytes(*epk_p)];
    unsigned char bytes_c[element_length_in_bytes(*c_p)];
    element_to_bytes(bytes_epk, *epk_p);
    element_to_bytes(bytes_c, *c_p);
    string epk_str((char *)bytes_epk, element_length_in_bytes(*epk_p));
    string c_str((char *)bytes_c, element_length_in_bytes(*c_p));
    string combine = epk_str + c_str;
    // sigma' = esk' + (s1' + s2') * H2(epk'||c')
    this->abet.Hash(combine, &this->tmp_Zn_2);
    element_mul(this->tmp_Zn, this->tmp_Zn, this->tmp_Zn_2);
    element_add(*sigma_p, this->esk, this->tmp_Zn);
}

/**
 * input : pkPCHBA, m_p, p_p, h', b, C_p, c_p, epk_p, sigma_p
 * output: bool
 */
bool PCHBA_TLL_2020::Verify(pkPCHBA *pkPCHBA, element_t *m_p, element_t *p_p, element_t *h_, element_t *b, ABET::ciphertext *C_p, element_t *c_p, element_t *epk_p, element_t *sigma_p) {
    return this->Check(pkPCHBA, m_p, p_p, h_, b, C_p, c_p, epk_p, sigma_p);
}

/**
 * input : pkPCHBA, skPCHBA, m, p, h', b, C, c, epk, sigma, m',p',C',c',epk',sigma', ID, mi,
 * output: bool
 */
bool PCHBA_TLL_2020::Judge(pkPCHBA *pkPCHBA, skPCHBA *skPCHBA, 
                            element_t *m, element_t *p, element_t *h_, element_t *b, ABET::ciphertext *C, element_t *c, element_t *epk, element_t *sigma,
                            element_t *m_p, element_t *p_p, ABET::ciphertext *C_p, element_t *c_p, element_t *epk_p, element_t *sigma_p,
                            ABET::ID *ID, int mi) {
    // step 1
    element_pow_zn(this->tmp_H, *h_, *m);
    element_mul(this->tmp_H_2, *p, this->tmp_H);
    if (element_cmp(*b, this->tmp_H_2) != 0) {
        return false;
    }
    element_pow_zn(this->tmp_H, *h_, *m_p);
    element_mul(this->tmp_H_2, *p_p, this->tmp_H);
    if (element_cmp(*b, this->tmp_H_2) != 0) {
        return false;
    }

    // // step 2
    // // g^sigma =? epk * g^(sk*H2(epk||c))
    // // epk_str + c_str
    // unsigned char bytes_epk[element_length_in_bytes(*epk)];
    // unsigned char bytes_c[element_length_in_bytes(*c)];
    // element_to_bytes(bytes_epk, *epk);
    // element_to_bytes(bytes_c, *c);
    // string epk_str((char *)bytes_epk, element_length_in_bytes(*epk));
    // string c_str((char *)bytes_c, element_length_in_bytes(*c));
    // string combine = epk_str + c_str;
    // this->abet.Hash(combine, &this->tmp_Zn);
    // element_mul(this->tmp_Zn, skPCHBA->skCHET.x, this->tmp_Zn);
    // element_pow_zn(this->tmp_G, pkPCHBA->pkABET.g, this->tmp_Zn);
    // element_mul(this->tmp_G, *epk, this->tmp_G);
    // element_pow_zn(this->tmp_G_2, pkPCHBA->pkABET.g, *sigma);
    // if (element_cmp(this->tmp_G, this->tmp_G_2) != 0) {
    //     return false;
    // }

    // // step 3
    // // delta_sk = c'/c
    // element_div(this->tmp_H, *c_p, *c);
    // // ct_0_3 * delta_sk
    // element_mul(this->tmp_H_2, C->ct_0.ct0_3, this->tmp_H);
    // if(element_cmp(C_p->ct_0.ct0_3, this->tmp_H_2) != 0) {
    //     return false;
    // }

    // step 4
    // ct1^(1/a^2)
    element_mul(this->tmp_Zn, skPCHBA->skABET.a, skPCHBA->skABET.a);
    element_invert(this->tmp_Zn, this->tmp_Zn);
    element_pow_zn(this->tmp_H, C->ct1, this->tmp_Zn);
    // e(g,ct1^(1/a^2))
    element_pairing(this->tmp_GT, pkPCHBA->pkABET.g, this->tmp_H);
    // ID_i
    this->abet.GetID_(&pkPCHBA->pkABET, ID, mi, abet.MODIFIER, &this->tmp_G);
    element_pairing(this->tmp_GT_2, this->tmp_G, C->ct_0.ct0_3);
    if (element_cmp(this->tmp_GT, this->tmp_GT_2) != 0) {
        return false;
    }



    return true;
}

PCHBA_TLL_2020::~PCHBA_TLL_2020() {
   
}