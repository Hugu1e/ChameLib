#include <scheme/CR_CH_DSS_2020.h>

/**
 * input : (y,h,m),(u11,u12,u2)
 * output: res
 */
void CR_CH_DSS_2020::H(element_t *y, element_t *h1, element_t *h2, element_t *m,
                        element_t *u11,element_t *u12,element_t *u2, 
                        element_t *res) {
    Hm_3(*y, *h1, *h2, *m, *u11, *u12, *u2, *res);
}

CR_CH_DSS_2020::CR_CH_DSS_2020(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT) {
    this->G1 = _G1;
    this->G2 = _G2;
    this->Zn = _Zn;
    this->GT = _GT;

    element_init_same_as(this->tmp_G1, *this->G1);
    element_init_same_as(this->tmp_G1_2, *this->G1);  
    element_init_same_as(this->tmp_G1_3, *this->G1);  
    element_init_same_as(this->tmp_G1_4, *this->G1);
    element_init_same_as(this->tmp_G2, *this->G2);
    element_init_same_as(this->tmp_Zn, *this->Zn);  
    element_init_same_as(this->tmp_Zn_2, *this->Zn);  
    element_init_same_as(this->tmp_GT, *this->GT);
    element_init_same_as(this->tmp_GT_2, *this->GT);
    element_init_same_as(this->tmp_GT_3, *this->GT);
    


    element_init_same_as(this->g, *this->G1);
    element_init_same_as(this->x, *this->Zn);
    element_init_same_as(this->y, *this->G1);

    element_init_same_as(this->xi, *this->Zn);
    element_init_same_as(this->k1, *this->Zn);
    element_init_same_as(this->e2, *this->Zn);
    element_init_same_as(this->s2, *this->Zn);

    element_init_same_as(this->u11, *this->G1);
    element_init_same_as(this->u12, *this->G1);
    element_init_same_as(this->u2, *this->G1);
    element_init_same_as(this->e, *this->Zn);
    element_init_same_as(this->e1, *this->Zn);
    element_init_same_as(this->s1, *this->Zn);
    element_init_same_as(this->k2, *this->Zn);



}

/**
 * input : 
 * output: 
 */
void CR_CH_DSS_2020::PG() {
    element_random(this->g);
    element_printf("g = %B\n", this->g);
}

/**
 * input : 
 * output: x, y
 */
void CR_CH_DSS_2020::KG(element_t *_x, element_t *_y) {  
    // secret key x ∈ Zp
    element_random(this->x);
    element_printf("x = %B\n", this->x);
    // 输出x的大小
    printf("sizeof(x):  %d bytes\n",element_length_in_bytes(this->x));
    // public key y = g^x
    element_pow_zn(this->y, this->g, this->x);
    element_printf("y = %B\n", this->y);

    element_set(*_x, this->x);
    element_set(*_y, this->y);
}
 

/**
 * input : pk(y), m
 * output: h(h1,h2), pai(e1,e2,s11,s12,s2)
 */
void CR_CH_DSS_2020::Hash(element_t *_y, element_t *m, 
                                element_t *h1,element_t *h2,
                                element_t *_e1, element_t *_e2, element_t *_s1, element_t *_s2) {
    element_random(this->xi);
    element_random(this->k1);
    element_random(this->e2);
    element_random(this->s2);

    // u11 = g^k1
    element_pow_zn(this->u11, this->g, this->k1);
    // u12 = pk^k1
    element_pow_zn(this->u12, *_y, this->k1);
    // u2 = g^s2 * pk^-e2
    element_pow_zn(this->tmp_G1, this->g, this->s2);
    element_pow_zn(this->tmp_G2, *_y, this->e2);
    element_div(this->u2, this->tmp_G1, this->tmp_G2);
    // h1 = g^xi
    element_pow_zn(*h1, this->g, this->xi);
    element_printf("h1 = %B\n", *h1);
    // 输出h1的大小
    printf("sizeof(h1):  %d bytes\n",element_length_in_bytes(*h1));
    // h2 = m * pk^xi
    element_pow_zn(this->tmp_G1, *_y, this->xi);
    element_mul(*h2, *m, this->tmp_G1);
    element_printf("h2 = %B\n", *h2);
    // 输出h2的大小
    printf("sizeof(h2):  %d bytes\n",element_length_in_bytes(*h2));
    // e = H((y,h,m),(u11,u12,u2))
    this->H(_y, h1, h2, m, &this->u11, &this->u12, &this->u2, &this->e);
    // e1 = e - e2
    element_sub(this->e1, this->e, this->e2);
    // s1 = k1 + e1 * xi
    element_mul(this->tmp_Zn, this->e1, this->xi);
    element_add(this->s1, this->k1, this->tmp_Zn);

    // return pai(e1,e2,s1,s2)
    element_set(*_e1, this->e1);
    element_set(*_e2, this->e2);
    element_set(*_s1, this->s1);
    element_set(*_s2, this->s2);
    element_printf("pai:\n");
    element_printf("e1 = %B\n", *_e1);
    element_printf("e2 = %B\n", *_e2);
    element_printf("s1 = %B\n", *_s1);
    element_printf("s2 = %B\n", *_s2);
}

/**
 * input : pk(y), m, h(h1,h2), r(e1,e2,s1,s2)
 * output: bool
 */
bool CR_CH_DSS_2020::Check(element_t *_y, element_t *m, 
                            element_t *h1, element_t *h2,
                            element_t *_e1, element_t *_e2, element_t *_s1, element_t *_s2){
    
    // e1 + e2
    element_add(this->tmp_Zn, *_e1, *_e2);
    
    // (g^s1)*(c1^-e1)
    element_pow_zn(this->tmp_G1, this->g, *_s1);
    element_pow_zn(this->tmp_G1_2, *h1, *_e1);
    element_div(this->tmp_G1, this->tmp_G1, this->tmp_G1_2);
    
    // (y^s1) * ((c2/m)^-e1)
    element_pow_zn(this->tmp_G1_2, *_y, *_s1);
        // c2/m
    element_div(this->tmp_G1_3, *h2, *m);
    element_pow_zn(this->tmp_G1_4, this->tmp_G1_3, *_e1);
    element_div(this->tmp_G1_2, this->tmp_G1_2, this->tmp_G1_4);

    // g^s2 * y^-e2
    element_pow_zn(this->tmp_G1_3, this->g, *_s2);
    element_pow_zn(this->tmp_G1_4, *_y, *_e2);
    element_div(this->tmp_G1_3, this->tmp_G1_3, this->tmp_G1_4);

    // H((y,h,m),(tmp_G1, tmp_G1_2, tmp_G1_3))
    this->H(_y,h1,h2,m,&this->tmp_G1, &this->tmp_G1_2, &this->tmp_G1_3, &this->tmp_Zn_2);

    // element_printf("Check(): tmp_Zn = %B\n", this->tmp_Zn);
    // element_printf("Check(): tmp_Zn_2 = %B\n", this->tmp_Zn_2);
    return element_cmp(this->tmp_Zn, this->tmp_Zn_2) == 0;
}

/**
 * input : x , m,  m_p, r(e1,e2,s1,s2), h(h1,h2)
 * output: r_p(e1_p,e2_p,s1_p,s2_p)
 */
void CR_CH_DSS_2020::Forge(element_t *_x, element_t *m, element_t *m_p, 
                                element_t *_e1, element_t *_e2, element_t *_s1, element_t *_s2,
                                element_t *h1, element_t *h2,
                                element_t *_e1_p,element_t *_e2_p,element_t *_s1_p,element_t *_s2_p) {
    // pk
    element_pow_zn(this->y, this->g, *_x);

    if(!this->Check(&this->y, m, h1, h2, _e1, _e2, _s1, _s2)){
        printf("Forge(): Check failed !!\n");
        return;
    }

    element_random(this->k2);
    element_random(this->e1);
    element_random(this->s1);

    // u11 = g^s1 * c1^-e1
    element_pow_zn(this->tmp_G1, this->g, this->s1);
    element_pow_zn(this->tmp_G1_2, *h1, this->e1);
    element_div(this->u11, this->tmp_G1, this->tmp_G1_2);

    // u12 = y^s1 * (c2/m')^-e1
    element_pow_zn(this->tmp_G1, this->y, this->s1);
    element_div(this->tmp_G1_2, *h2, *m_p);
    element_pow_zn(this->tmp_G1_3, this->tmp_G1_2, this->e1);
    element_div(this->u12, this->tmp_G1, this->tmp_G1_3);

    // u2 = g^k2
    element_pow_zn(this->u2, this->g, this->k2);

    // e = H((y,h,m'),(u11,u12,u2))
    this->H(&this->y, h1, h2, m_p, &this->u11, &this->u12, &this->u2, &this->e);

    // e2 = e - e1
    element_sub(this->e2, this->e, this->e1);

    // s2 = k2 + e2 * x
    element_mul(this->tmp_Zn, this->e2, *_x);
    element_add(this->s2, this->k2, this->tmp_Zn);

    // return pai'(e1,e2,s1,s2)
    element_set(*_e1_p, this->e1);
    element_set(*_e2_p, this->e2);
    element_set(*_s1_p, this->s1);
    element_set(*_s2_p, this->s2);
    element_printf("pai_p:\n");
    element_printf("e1_p = %B\n", *_e1_p);
    element_printf("e2_p = %B\n", *_e2_p);
    element_printf("s1_p = %B\n", *_s1_p);
    element_printf("s2_p = %B\n", *_s2_p);
}

/**
 * input : y, m_p,r_p, h
 * output: bool
 */
bool CR_CH_DSS_2020::Verify(element_t *_y, element_t *m_p, element_t *h1, element_t *h2,
                                 element_t *_e1_p,element_t *_e2_p,element_t *_s1_p,element_t *_s2_p) {

    // 利用Check函数进行比较
    return this->Check(_y, m_p, h1, h2, _e1_p, _e2_p, _s1_p, _s2_p);
}


CR_CH_DSS_2020::~CR_CH_DSS_2020() {
    element_clear(this->tmp_G1);
    element_clear(this->tmp_G1_2);
    element_clear(this->tmp_G2);
    element_clear(this->tmp_Zn);
    element_clear(this->tmp_Zn_2);
    element_clear(this->tmp_GT);
    element_clear(this->tmp_GT_2);
    element_clear(this->tmp_GT_3);



}