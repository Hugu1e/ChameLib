#include <scheme/IBCH/ID_B_CollRes_XSL_2021.h>

ID_B_CollRes_XSL_2021::ID_B_CollRes_XSL_2021(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn):PbcScheme(_G1, _G2, _GT, _Zn) {
    element_init_same_as(this->a, Zn); 
    element_init_same_as(this->t, Zn);  
}


void ID_B_CollRes_XSL_2021::SetUp(ID_B_CollRes_XSL_2021_pp &pp, ID_B_CollRes_XSL_2021_msk &msk, ID_B_CollRes_XSL_2021_tk &tk, ID_B_CollRes_XSL_2021_h &h, ID_B_CollRes_XSL_2021_r &r, ID_B_CollRes_XSL_2021_r &r_p, unsigned long int n) {
    pp.init(3);
    pp.get_u().init(n+1);
    msk.init(1);
    tk.init(2);
    h.init(1);
    r.init(2);
    r_p.init(2);

    pp.set_n(n);
    element_random(tmp_G);
    pp.set(g, tmp_G);
    element_random(this->a);
    // g1 = g^a
    element_pow_zn(tmp_G, tmp_G, this->a);
    pp.set(g1, tmp_G);
    // g2 ∈ G
    element_random(tmp_G);
    pp.set(g2, tmp_G);

    // msk
    element_pow_zn(tmp_G, tmp_G, this->a);
    msk.set(s, tmp_G);
     
    for(unsigned long int i = 0; i <= n; i++) {
        element_random(tmp_G);
        pp.get_u().set(i, tmp_G);
    }
}

/**
 * input : msk, I
 * output: tk1, tk2
 */
void ID_B_CollRes_XSL_2021::KeyGen(ID_B_CollRes_XSL_2021_tk &tk, ID_B_CollRes_XSL_2021_msk &msk, const char *I, ID_B_CollRes_XSL_2021_pp &pp) {
    element_random(this->t);

    // compute tk1
    // tmp_G1 = u0
    element_set(this->tmp_G, pp.get_u()[0]);
    for(unsigned long int i = 1; i <= pp.get_n(); i++) {
        if(getBit(I, i-1)) {
            element_mul(this->tmp_G, this->tmp_G, pp.get_u()[i]);
        }
    }
    element_pow_zn(tmp_G_2, this->tmp_G, this->t);
    element_mul(tmp_G_2, msk[s], tmp_G_2);
    tk.set(tk1, tmp_G_2);

    // compute tk2
    element_pow_zn(tmp_G_2, pp[g], this->t);
    tk.set(tk2, tmp_G_2);
}
 
/**
 * 从element中获取第index二进制位的值
 */
bool ID_B_CollRes_XSL_2021::getBit(const char *bytes, int index) {
    unsigned long int byte_index = index / 8;
    unsigned long int bit_index = index % 8;
    return (bytes[byte_index] >> (7 - bit_index)) & 1;
}

/**
 * input : I, m
 * output: h, r1, r2
 */
void ID_B_CollRes_XSL_2021::Hash(ID_B_CollRes_XSL_2021_h &h, ID_B_CollRes_XSL_2021_r &r, element_t m, const char *I, ID_B_CollRes_XSL_2021_pp &pp) {
    element_random(tmp_G);
    r.set(r1, tmp_G);
    element_random(tmp_G_2);
    r.set(r2, tmp_G_2);

    // compute h
    element_pairing(this->tmp_GT, pp[g1], pp[g2]);
    element_pow_zn(this->tmp_GT, this->tmp_GT, m);
    element_pairing(this->tmp_GT_2, tmp_G, pp[g]);

    element_set(this->tmp_G_3, pp.get_u()[0]);
    for(unsigned long int i = 1; i <= pp.get_n(); i++) {
        if(getBit(I, i-1)) {
            element_mul(this->tmp_G_3, this->tmp_G_3, pp.get_u()[i]);
        }
    }
    element_pairing(this->tmp_GT_3, tmp_G_2, this->tmp_G_3);

    element_mul(tmp_GT, this->tmp_GT, this->tmp_GT_2);
    element_div(tmp_GT, tmp_GT, this->tmp_GT_3);
    h.set(h1, tmp_GT);
}

/**
 * input : I, m, r1, r2
 * output: h
 */
bool ID_B_CollRes_XSL_2021::Check(ID_B_CollRes_XSL_2021_h &h, element_t m, ID_B_CollRes_XSL_2021_r &r, const char *I, ID_B_CollRes_XSL_2021_pp &pp) {
    // compute h
    element_pairing(this->tmp_GT, pp[g1], pp[g2]);
    element_pow_zn(this->tmp_GT, this->tmp_GT, m);
    element_pairing(this->tmp_GT_2, r[r1], pp[g]);

    element_set(this->tmp_G_3, pp.get_u()[0]);
    for(unsigned long int i = 1; i <= pp.get_n(); i++) {
        if(getBit(I, i-1)) {
            element_mul(this->tmp_G_3, this->tmp_G_3, pp.get_u()[i]);
        }
    }

    element_pairing(this->tmp_GT_3, r[r2], this->tmp_G_3);
    element_mul(tmp_GT, this->tmp_GT, this->tmp_GT_2);
    element_div(tmp_GT, tmp_GT, this->tmp_GT_3);

    return element_cmp(h[h1], this->tmp_GT) == 0;
}

/**
 * input : tk1, tk2, h, m, r1, r2, m_p
 * output: r1_p, r2_p
 */
void ID_B_CollRes_XSL_2021::Adapt(ID_B_CollRes_XSL_2021_r &r_p, element_t m_p, ID_B_CollRes_XSL_2021_h &h, element_t m, ID_B_CollRes_XSL_2021_r &r, ID_B_CollRes_XSL_2021_tk &tk) {
    // compute r1_p
    element_sub(this->tmp_Zn, m, m_p);
    element_pow_zn(tmp_G, tk[tk1], this->tmp_Zn);
    element_mul(tmp_G, r[r1], tmp_G);
    r_p.set(r1, tmp_G);
    
    // compute r2_p
    element_pow_zn(tmp_G, tk[tk2], this->tmp_Zn);
    element_mul(tmp_G, r[r2], tmp_G);
    r_p.set(r2, tmp_G);
}

/**
 * input : I, m_p, r1_p, r2_p, h
 * output: bool
 */
bool ID_B_CollRes_XSL_2021::Verify(ID_B_CollRes_XSL_2021_h &h, element_t m_p, ID_B_CollRes_XSL_2021_r &r_p, const char *I, ID_B_CollRes_XSL_2021_pp &pp) {
    return Check(h, m_p, r_p, I, pp);
}

ID_B_CollRes_XSL_2021::~ID_B_CollRes_XSL_2021() {
    element_clear(this->a);
    element_clear(this->t);
}
