#include <scheme/ID_B_CollRes_XSL_2021.h>

ID_B_CollRes_XSL_2021::ID_B_CollRes_XSL_2021(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT) {
    this->G1 = _G1;
    this->G2 = _G2;
    this->Zn = _Zn;
    this->GT = _GT;

    element_init_same_as(this->tmp_G1, *this->G1);
    element_init_same_as(this->tmp_G1_2, *this->G1);  
    element_init_same_as(this->tmp_G2, *this->G2);
    element_init_same_as(this->tmp_Zn, *this->Zn);  
    element_init_same_as(this->tmp_Zn_2, *this->Zn);  
    element_init_same_as(this->tmp_GT, *this->GT);
    element_init_same_as(this->tmp_GT_2, *this->GT);
    element_init_same_as(this->tmp_GT_3, *this->GT);

    element_init_same_as(this->tmp, *this->G1);

    element_init_same_as(this->g, *this->G1); 
    element_init_same_as(this->a, *this->Zn); 
    element_init_same_as(this->g1, *this->G1);
    element_init_same_as(this->g2, *this->G1);  
    element_init_same_as(this->t, *this->Zn);  
}

/**
 * input : n
 * output: msk
 */
void ID_B_CollRes_XSL_2021::PG(unsigned long int n, element_t *msk) {
    if(n < 1) {
        throw std::invalid_argument("n must be greater than 0.");
    }
    element_random(this->g);
    element_random(this->a);
    element_pow_zn(this->g1, this->g, this->a);
    element_random(this->g2);
    element_pow_zn(*msk, this->g2, this->a);
    // 输出msk
    element_printf("msk = %B\n", *msk); 
    
    this->n = n;
    printf("n = %lu\n", n);
    this->array_u = new element_t[n+1];
    for(unsigned long int i = 0;i <= n;i++) {
        element_init_same_as(this->array_u[i], *this->G1);
        element_random(this->array_u[i]);
    }
}

/**
 * input : msk, I
 * output: tk1, tk2
 */
void ID_B_CollRes_XSL_2021::KG(element_t *msk, element_t *I, element_t *tk1, element_t *tk2) {
    unsigned long int I_bit_length = element_length_in_bytes(*I) * 8;
    if(I_bit_length != this->n) {
        throw std::invalid_argument("The bit length of I(identity) must be equal to m(message).");
    }

    element_random(this->t);
    element_printf("t = %B\n", this->t);

    // compute tk1
    // tmp_G1 = u0
    element_set(this->tmp_G1, this->array_u[0]);
    for(unsigned long int i = 1;i <= this->n;i++) {
        if(getBit(I, i-1)) {
            element_mul(this->tmp_G1, this->tmp_G1, this->array_u[i]);
        }
    }
    element_set(this->tmp, this->tmp_G1);  // 暂存
    element_pow_zn(*tk1, this->tmp_G1, this->t);
    element_mul(*tk1, *msk, *tk1);
    element_printf("tk1 = %B\n", *tk1);

    // compute tk2
    element_pow_zn(*tk2, this->g, this->t);
    element_printf("tk2 = %B\n", *tk2); 
}
 
/**
 * 从element中获取第index二进制位的值
 */
bool ID_B_CollRes_XSL_2021::getBit(element_t *element, unsigned long int index) {
    unsigned long int byte_index = index / 8;
    unsigned long int bit_index = index % 8;
    unsigned char bytes[element_length_in_bytes(*element)];
    element_to_bytes(bytes, *element);
    return (bytes[byte_index] >> (7 - bit_index)) & 1;
}

/**
 * input : I, m
 * output: h, r1, r2
 */
void ID_B_CollRes_XSL_2021::Hash(element_t *I, element_t *m, element_t *h, element_t *r1, element_t *r2) {
    element_random(*r1);
    element_random(*r2);
    element_printf("r1 = %B\n", *r1);
    element_printf("r2 = %B\n", *r2);

    // compute h
    element_pairing(this->tmp_GT, this->g1, this->g2);
    element_pow_zn(this->tmp_GT, this->tmp_GT, *m);
    element_pairing(this->tmp_GT_2, *r1, this->g);
    element_pairing(this->tmp_GT_3, *r2, this->tmp);
    element_mul(*h, this->tmp_GT, this->tmp_GT_2);
    element_div(*h, *h, this->tmp_GT_3);

    element_printf("h = %B\n", *h);
}

/**
 * input : I, m, r1, r2
 * output: h
 */
void ID_B_CollRes_XSL_2021::hash_with_r(element_t *I, element_t *m, element_t *r1, element_t *r2, element_t *h) {
    // compute h
    element_pairing(this->tmp_GT, this->g1, this->g2);
    element_pow_zn(this->tmp_GT, this->tmp_GT, *m);
    element_pairing(this->tmp_GT_2, *r1, this->g);
    element_pairing(this->tmp_GT_3, *r2, this->tmp);
    element_mul(*h, this->tmp_GT, this->tmp_GT_2);
    element_div(*h, *h, this->tmp_GT_3);
}

/**
 * input : tk1, tk2, h, m, r1, r2, m_p
 * output: r1_p, r2_p
 */
void ID_B_CollRes_XSL_2021::Forge(element_t *tk1, element_t *tk2, 
                                    element_t *h, element_t *m,
                                    element_t *r1, element_t *r2, 
                                    element_t *m_p,
                                    element_t *r1_p, element_t *r2_p) {
    // compute r1_p
    element_sub(this->tmp_Zn, *m, *m_p);
    element_pow_zn(*r1_p, *tk1, this->tmp_Zn);
    element_mul(*r1_p, *r1, *r1_p);
    element_printf("r1_p = %B\n", *r1_p);
    
    // compute r2_p
    element_sub(this->tmp_Zn, *m, *m_p);
    element_pow_zn(*r2_p, *tk2, this->tmp_Zn);
    element_mul(*r2_p, *r2, *r2_p);
    element_printf("r2_p = %B\n", *r2_p);
}

/**
 * input : I, m_p, r1_p, r2_p, h
 * output: bool
 */
bool ID_B_CollRes_XSL_2021::Verify(element_t *I, element_t *m_p, element_t *r1_p, element_t *r2_p, element_t *h) {
    hash_with_r(I, m_p, r1_p, r2_p, &this->tmp_GT);
    return element_cmp(*h, this->tmp_GT) == 0;
}

ID_B_CollRes_XSL_2021::~ID_B_CollRes_XSL_2021() {
    element_clear(this->tmp_G1);
    element_clear(this->tmp_G1_2);
    element_clear(this->tmp_G2);
    element_clear(this->tmp_Zn);
    element_clear(this->tmp_Zn_2);
    element_clear(this->tmp_GT);
    element_clear(this->tmp_GT_2);
    element_clear(this->tmp_GT_3);
    
    element_clear(this->tmp);

    element_clear(this->g);
    element_clear(this->a);
    element_clear(this->g1);
    element_clear(this->g2);
    element_clear(this->t);
}