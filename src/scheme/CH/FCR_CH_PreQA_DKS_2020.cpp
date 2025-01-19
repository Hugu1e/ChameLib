#include <scheme/FCR_CH_PreQA_DKS_2020.h>

/**
 * input : (y,h,m),(u1,u2)
 * output: res
 */
void FCR_CH_PreQA_DKS_2020::H(element_t *y, element_t *h, element_t *m,element_t *u1,element_t *u2, element_t *res) {
    Hm_2(*y, *h, *m,*u1,*u2, *res);
}

FCR_CH_PreQA_DKS_2020::FCR_CH_PreQA_DKS_2020(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT) {
    this->G1 = _G1;
    this->G2 = _G2;
    this->Zn = _Zn;
    this->GT = _GT;

    element_init_same_as(this->tmp_G1, *this->G1);
    element_init_same_as(this->tmp_G1_2, *this->G1);  
    element_init_same_as(this->tmp_G1_3, *this->G1);  
    element_init_same_as(this->tmp_G2, *this->G2);
    element_init_same_as(this->tmp_Zn, *this->Zn);  
    element_init_same_as(this->tmp_Zn_2, *this->Zn);  
    element_init_same_as(this->tmp_GT, *this->GT);
    element_init_same_as(this->tmp_GT_2, *this->GT);
    element_init_same_as(this->tmp_GT_3, *this->GT);
    
    element_init_same_as(this->tmp_y, *this->G1);


    element_init_same_as(this->g1, *this->G1);
    element_init_same_as(this->g2, *this->G1);  
    element_init_same_as(this->x, *this->Zn);
    // xi,k11,k12,k2,e2,s2;
    element_init_same_as(this->xi, *this->Zn);
    element_init_same_as(this->k11, *this->Zn);
    element_init_same_as(this->k12, *this->Zn);
    element_init_same_as(this->k2, *this->Zn);
    element_init_same_as(this->e2, *this->Zn);
    element_init_same_as(this->s2, *this->Zn);

    element_init_same_as(this->u1, *this->G1);
    element_init_same_as(this->u2, *this->G1);

    element_init_same_as(this->e, *this->Zn);
    element_init_same_as(this->e1, *this->Zn);
    element_init_same_as(this->s11, *this->Zn);
    element_init_same_as(this->s12, *this->Zn);
}

/**
 * input : 
 * output: 
 */
void FCR_CH_PreQA_DKS_2020::PG() {
    element_random(this->g1);
    element_printf("g1 = %B\n", this->g1);
    // todo g2: g2=h'(g1)
    element_random(this->g2);
    element_printf("g2 = %B\n", this->g2);
}

/**
 * input : 
 * output: y
 */
void FCR_CH_PreQA_DKS_2020::KG(element_t *y) {  
    // secret key x ∈ Zp
    element_random(this->x);
    element_printf("x = %B\n", this->x);
    // 输出x的大小
    printf("sizeof(x): %d bystes\n",element_length_in_bytes(this->x));
    // public key y = g1^x
    element_pow_zn(*y, this->g1, this->x);
    element_printf("y = %B\n", *y);
}
 

/**
 * input : m, y, 
 * output: h, pai(e1,e2,s11,s12,s2)
 */
void FCR_CH_PreQA_DKS_2020::Hash(element_t *m, element_t *y, 
                                element_t *h,
                                element_t *_e1, element_t *_e2, element_t *_s11, element_t *_s12, element_t *_s2) {
    element_random(this->xi);
    element_random(this->k11);
    element_random(this->k12);
    element_random(this->k2);
    element_random(this->e2);
    element_random(this->s2);
    // u1 = (g1^k11) *(g2^k12)
    element_pow_zn(this->tmp_G1, this->g1, this->k11); 
    element_pow_zn(this->tmp_G2, this->g2, this->k12);
    element_mul(this->u1, this->tmp_G1, this->tmp_G2);
    // u2 = (g1^s2) *(y^-e2)   
    element_pow_zn(this->tmp_G1, this->g1, this->s2); 
    element_pow_zn(this->tmp_G2, *y, this->e2);
    element_div(this->u2, this->tmp_G1, this->tmp_G2);
    // h = (g1^m) * (g2^xi)
    element_pow_zn(this->tmp_G1, this->g1, *m);
    element_pow_zn(this->tmp_G2, this->g2, this->xi);
    element_mul(*h, this->tmp_G1, this->tmp_G2);
    element_printf("h = %B\n", *h);
    // 输出h的大小
    printf("sizeof(h):  %d bytes\n",element_length_in_bytes(*h));
    // e = H((y,h,m),(u1,u2))
    // ? 将 m 哈希成群上的一个点
    element_from_hash(this->tmp_G1, (void *)m, element_length_in_bytes(*m));
    this->H(y,h,m,&this->u1,&this->u2,&this->e);
    // e1 = e - e2
    element_sub(this->e1, this->e, this->e2);
    // s11 = k11 + e1 * m
    element_mul(this->tmp_Zn, this->e1, *m);
    element_add(this->s11, this->k11, this->tmp_Zn);
    // s12 = k12 + e1 * xi
    element_mul(this->tmp_Zn, this->e1, this->xi);
    element_add(this->s12, this->k12, this->tmp_Zn);

    // return pai(e1,e2,s11,s12,s2)
    element_set(*_e1, this->e1);
    element_set(*_e2, this->e2);
    element_set(*_s11, this->s11);
    element_set(*_s12, this->s12);
    element_set(*_s2, this->s2);
    printf("pai:\n");
    element_printf("e1 = %B\n", *_e1);
    element_printf("e2 = %B\n", *_e2);
    element_printf("s11 = %B\n", *_s11);
    element_printf("s12 = %B\n", *_s12);
    element_printf("s2 = %B\n", *_s2);
}

/**
 * input : y, m, h, r(e1,e2,s11,s12,s2)
 * output: bool
 */
bool FCR_CH_PreQA_DKS_2020::Check(element_t *y, element_t *m, element_t *h,
                                element_t *_e1, element_t *_e2, element_t *_s11, element_t *_s12, element_t *_s2){
    // element_printf("Check(): y = %B\n", *y);
    // element_printf("Check(): m = %B\n", *m);
    // element_printf("Check(): h = %B\n", *h);
    // element_printf("Check(): e1 = %B\n", *_e1);
    // element_printf("Check(): e2 = %B\n", *_e2);
    // element_printf("Check(): s11 = %B\n", *_s11);
    // element_printf("Check(): s12 = %B\n", *_s12);
    // element_printf("Check(): s2 = %B\n", *_s2);
    
    // e1 + e2
    element_add(this->tmp_Zn, *_e1, *_e2);
    
    // (g1^s11)*(g2^s12)*(h^-e1)
    element_pow_zn(this->tmp_G1, this->g1, *_s11);
    element_pow_zn(this->tmp_G1_2, this->g2, *_s12);
    element_pow_zn(this->tmp_G1_3, *h, *_e1);
    element_mul(this->tmp_G1, this->tmp_G1, this->tmp_G1_2);
    element_div(this->tmp_G1, this->tmp_G1, this->tmp_G1_3);
    
    // (g1^s2)*(y^-e2)
    element_pow_zn(this->tmp_G1_2, this->g1, *_s2);
    element_pow_zn(this->tmp_G1_3, *y, *_e2);
    element_div(this->tmp_G1_2, this->tmp_G1_2, this->tmp_G1_3);

    // H((y,h,m),(tmp_G1, tmp_G1_2))
    this->H(y,h,m,&this->tmp_G1, &this->tmp_G1_2, &this->tmp_Zn_2);

    return element_cmp(this->tmp_Zn, this->tmp_Zn_2) == 0;
}

/**
 * input : (this->x) , m,  m_p, r(e1,e2,s11,s12,s2), h
 * output: r_p(e1_p,e2_p,s11_p,s12_p,s2_p)
 */
void FCR_CH_PreQA_DKS_2020::Forge(element_t *m, element_t *m_p, 
                                element_t *_e1, element_t *_e2, element_t *_s11, element_t *_s12, element_t *_s2,
                                element_t *h,
                                element_t *e1_p,element_t *e2_p,element_t *s11_p,element_t *s12_p,element_t *s2_p) {
    // y = g1^x
    element_pow_zn(this->tmp_y, this->g1, this->x);
    // Check
    if(!this->Check(&this->tmp_y, m, h, _e1, _e2, _s11, _s12, _s2)){
        printf("Forge(): Check failed !!\n");
        return;
    }
    
    element_random(this->k11);
    element_random(this->k12);
    element_random(this->e1);
    element_random(this->s11);
    element_random(this->s12);
    // u1 = (g1^s11) *(g2^s12) * (h^-e1)
    element_pow_zn(this->tmp_G1, this->g1, this->s11);
    element_pow_zn(this->tmp_G1_2, this->g2, this->s12);
    element_pow_zn(this->tmp_G1_3, *h, this->e1);
    element_mul(this->u1, this->tmp_G1, this->tmp_G1_2);
    element_div(this->u1, this->u1, this->tmp_G1_3);
    // ? u2 = (g1^k12)
    element_pow_zn(this->u2, this->g1, this->k12);
    // e = H((y,h,m_p),(u1,u2))
    this->H(&this->tmp_y,h,m_p,&this->u1,&this->u2,&this->e);
    // e2 = e - e1
    element_sub(this->e2, this->e, this->e1);
    // s2 = k12 + e2 * x
    element_mul(this->tmp_Zn, this->e2, this->x);
    element_add(this->s2, this->k12, this->tmp_Zn);
    // return pai(e1,e2,s11,s12,s2)
    element_set(*e1_p, this->e1);
    element_set(*e2_p, this->e2);
    element_set(*s11_p, this->s11);
    element_set(*s12_p, this->s12);
    element_set(*s2_p, this->s2);
    printf("pai_p:\n");
    element_printf("e1_p = %B\n", *e1_p);
    element_printf("e2_p = %B\n", *e2_p);
    element_printf("s11_p = %B\n", *s11_p);
    element_printf("s12_p = %B\n", *s12_p);
    element_printf("s2_p = %B\n", *s2_p);
}

/**
 * input : y, m_p,r_p, h
 * output: bool
 */
bool FCR_CH_PreQA_DKS_2020::Verify(element_t *y, element_t *m_p, element_t *h,
                                 element_t *e1_p,element_t *e2_p,element_t *s11_p,element_t *s12_p,element_t *s2_p) {

    // 利用Check函数进行比较
    return this->Check(y, m_p, h, e1_p, e2_p, s11_p, s12_p, s2_p);
}

/**
 * input : y, m_p,r_p, h
 * output: bool
 */
bool FCR_CH_PreQA_DKS_2020::Verify2(element_t *y, element_t *m, element_t *m_p) {
    // 利用Hash进行比较
    // m,y -> h
    element_t r1,r2,r3,r4,r5,h;
    element_init_same_as(r1, *this->Zn);
    element_init_same_as(r2, *this->Zn);
    element_init_same_as(r3, *this->Zn);
    element_init_same_as(r4, *this->Zn);
    element_init_same_as(r5, *this->Zn);
    element_init_same_as(h, *this->G1);
    this->Hash(m, y, &h, &r1, &r2, &r3, &r4, &r5);

    // m_p,y -> h_p
    element_t r1_p,r2_p,r3_p,r4_p,r5_p,h_p;
    element_init_same_as(r1_p, *this->Zn);
    element_init_same_as(r2_p, *this->Zn);
    element_init_same_as(r3_p, *this->Zn);
    element_init_same_as(r4_p, *this->Zn);
    element_init_same_as(r5_p, *this->Zn);
    element_init_same_as(h_p, *this->G1);
    this->Hash(m_p, y, &h_p, &r1_p, &r2_p, &r3_p, &r4_p, &r5_p);


    element_printf("Verify2(): h = %B\n", h);
    element_printf("Verify2(): h_p = %B\n", h_p);

    element_clear(r1);
    element_clear(r2);
    element_clear(r3);
    element_clear(r4);
    element_clear(r5);
    element_clear(h);
    element_clear(r1_p);
    element_clear(r2_p);
    element_clear(r3_p);
    element_clear(r4_p);
    element_clear(r5_p);
    
}

FCR_CH_PreQA_DKS_2020::~FCR_CH_PreQA_DKS_2020() {
    element_clear(this->tmp_G1);
    element_clear(this->tmp_G1_2);
    element_clear(this->tmp_G2);
    element_clear(this->tmp_Zn);
    element_clear(this->tmp_Zn_2);
    element_clear(this->tmp_GT);
    element_clear(this->tmp_GT_2);
    element_clear(this->tmp_GT_3);



    element_clear(this->a);
    element_clear(this->x);
}