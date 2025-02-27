#include <scheme/IBCH/ID_B_CollRes_XSL_2021.h>

ID_B_CollRes_XSL_2021::ID_B_CollRes_XSL_2021(int curve, bool swap):PbcScheme(curve) {
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

    element_init_same_as(this->a, Zn); 
    element_init_same_as(this->t, Zn);

    element_init_same_as(tmp_G, G1);
    element_init_same_as(tmp_G_2, G1);
    element_init_same_as(tmp_H, G2);
    element_init_same_as(tmp_H_2, G2);
    element_init_same_as(tmp_GT, GT);
    element_init_same_as(tmp_GT_2, GT);
    element_init_same_as(tmp_GT_3, GT);
    element_init_same_as(tmp_Zn, Zn);
}

bool ID_B_CollRes_XSL_2021::Pairing(element_t res, element_t g1, element_t g2){
    if(swap){
        element_pairing(res, g2, g1);
    }else{
        element_pairing(res, g1, g2);
    }
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
    // g ∈ G1
    element_random(tmp_G);
    pp.set(g, tmp_G);

    element_random(this->a);

    // g1 = g^a
    element_pow_zn(tmp_G, tmp_G, this->a);
    pp.set(g1, tmp_G);

    // g2 ∈ G2
    element_random(tmp_H);
    pp.set(g2, tmp_H);

    // msk = g2^a
    element_pow_zn(tmp_H, tmp_H, this->a);
    msk.set(s, tmp_H);
     
    // u0, u1, ..., un ∈ G2
    for(unsigned long int i = 0; i <= n; i++) {
        element_random(tmp_H);
        pp.get_u().set(i, tmp_H);
    }
}

/**
 * input : msk, I
 * output: tk1, tk2
 */
void ID_B_CollRes_XSL_2021::KeyGen(ID_B_CollRes_XSL_2021_tk &tk, ID_B_CollRes_XSL_2021_msk &msk, const char *I, ID_B_CollRes_XSL_2021_pp &pp) {
    element_random(this->t);

    // compute tk1
    // tmp = u0
    element_set(this->tmp_H, pp.get_u()[0]);
    for(unsigned long int i = 1; i <= pp.get_n(); i++) {
        if(getBit(I, i-1)) {
            element_mul(this->tmp_H, this->tmp_H, pp.get_u()[i]);
        }
    }
    element_pow_zn(tmp_H_2, this->tmp_H, this->t);
    element_mul(tmp_H_2, msk[s], tmp_H_2);
    tk.set(tk1, tmp_H_2);

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
    element_random(tmp_H);
    r.set(r1, tmp_H);
    element_random(tmp_G);
    r.set(r2, tmp_G);

    // compute h
    Pairing(this->tmp_GT, pp[g1], pp[g2]);
    element_pow_zn(this->tmp_GT, this->tmp_GT, m);
    Pairing(this->tmp_GT_2, pp[g], tmp_H);  // !!!

    // tmp
    element_set(this->tmp_H_2, pp.get_u()[0]);
    for(unsigned long int i = 1; i <= pp.get_n(); i++) {
        if(getBit(I, i-1)) {
            element_mul(this->tmp_H_2, this->tmp_H_2, pp.get_u()[i]);
        }
    }
    Pairing(this->tmp_GT_3, tmp_G, this->tmp_H_2);

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
    Pairing(this->tmp_GT, pp[g1], pp[g2]);
    element_pow_zn(this->tmp_GT, this->tmp_GT, m);
    Pairing(this->tmp_GT_2, pp[g], r[r1]);

    element_set(this->tmp_H, pp.get_u()[0]);
    for(unsigned long int i = 1; i <= pp.get_n(); i++) {
        if(getBit(I, i-1)) {
            element_mul(this->tmp_H, this->tmp_H, pp.get_u()[i]);
        }
    }

    Pairing(this->tmp_GT_3, r[r2], this->tmp_H);
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
    element_pow_zn(tmp_H, tk[tk1], this->tmp_Zn);
    element_mul(tmp_H, r[r1], tmp_H);
    r_p.set(r1, tmp_H);
    
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

    element_clear(tmp_G);
    element_clear(tmp_G_2);
    element_clear(tmp_H);
    element_clear(tmp_H_2);
    element_clear(tmp_GT);
    element_clear(tmp_GT_2);
    element_clear(tmp_GT_3);
    element_clear(tmp_Zn);

    element_clear(G1);
    element_clear(G2);
    element_clear(GT);
    element_clear(Zn);
}
