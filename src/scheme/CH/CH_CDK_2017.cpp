#include <scheme/CH_CDK_2017.h>

void CH_CDK_2017::H(mpz_t *gs, mpz_t *m, mpz_t *res, mpz_t *n){
    Hgsm_n(*gs,*m,*res,*n);  
}

CH_CDK_2017::CH_CDK_2017(mpz_t *n, mpz_t *e, mpz_t *d){
    this->rsa = new MyRSA(n,e,d);
}

void CH_CDK_2017::CParGen(){
    return;
}

void CH_CDK_2017::CKGen(mpz_t *n, mpz_t *e, mpz_t *d){
    // Generate two primes p and q using RSAKGen(1λ)
    this->rsa->rsa_generate_keys(1024);
    // 输出d的大小(bytes)
    size_t bits = mpz_sizeinbase(*d, 2);
    size_t bytes = (bits + 7) / 8;
    printf("sizeof(d): %zu bytes\n", bytes);
}

void CH_CDK_2017::CHash(mpz_t *h, mpz_t *r, mpz_t *n,mpz_t *e, mpz_t *m, mpz_t *tag){
    // Draw r ← Z∗n
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));
    mpz_urandomm(*r, state, *n);

    mpz_t g;
    mpz_init(g);

    // Let g ← Hn(τ, m)
    this->H(tag, m, &g, n);
    
    // h ← gr^e mod n
    mpz_t tmp;
    mpz_init(tmp);
    mpz_powm(tmp, *r, *e, *n);
    mpz_mul(*h, g, tmp);
    mpz_mod(*h, *h, *n);

    // 输出h的大小
    size_t bits = mpz_sizeinbase(*h, 2);
    size_t bytes = (bits + 7) / 8;
    printf("sizeof(h): %zu bytes\n", bytes);

    mpz_clear(tmp);
    mpz_clear(g);
}

bool CH_CDK_2017::CHashCheck(mpz_t *h_, mpz_t *m, mpz_t *tag, mpz_t *n, mpz_t *e, mpz_t *r){
    mpz_t g;
    mpz_init(g);
    // Let g ← Hn(τ, m)
    this->H(tag, m, &g, n);
    
    // h ← gr^e mod n
    mpz_t tmp;
    mpz_t tmp_2;
    mpz_init(tmp);
    mpz_init(tmp_2);
    mpz_powm(tmp, *r, *e, *n);
    mpz_mul(tmp_2, g, tmp);
    mpz_mod(tmp_2, tmp_2, *n);
    mpz_clear(tmp);
    mpz_clear(g);

    if(mpz_cmp(tmp_2, *h_) == 0){
        mpz_clear(tmp_2);
        return true;
    }else{
        mpz_clear(tmp_2);
        return false;
    }
}

void CH_CDK_2017::Adapt(mpz_t *r_p, mpz_t *m_p, mpz_t *tag_p, mpz_t *m, mpz_t *tag, mpz_t *r, mpz_t *h, mpz_t *n,mpz_t *e,mpz_t *d){
    mpz_t g,tmp;
    mpz_init(g);   
    // 1. Compute g ← Hn(τ, m), and h ← gre mod n.
    this->H(tag, m, &g, n);
    mpz_init(tmp);
    mpz_powm(tmp, *r, *e, *n);
    mpz_mul(*h, g, tmp);
    mpz_mod(*h, *h, *n);
    mpz_clear(tmp);
    mpz_clear(g);
    // 2. Draw τ 0 ← {0, 1}λ.


    // 3. Compute g0 ← Hn(τ 0, m0) and r0 ← (h(g0−1))d mod n.
    mpz_t tmp_1;  // g_p
    mpz_t tmp_2;
    mpz_init(tmp_1);
    mpz_init(tmp_2);
    this->H(tag_p, m_p, &tmp_1, n);
    mpz_invert(tmp_2, tmp_1, *n);  
    mpz_mul(tmp_1, *h, tmp_2);
    mpz_mod(tmp_1, tmp_1, *n);
    // 4. Return r0.
    mpz_powm(*r_p, tmp_1, *d, *n);
}

void CH_CDK_2017::CH_CDK_2017_clear(){
    this->rsa->rsa_clear();
}
