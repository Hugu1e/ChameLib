#include <scheme/MCH_CDK_2017.h>

void MCH_CDK_2017::H(mpz_t *m, mpz_t *res, mpz_t *n){
    Hm_n(*m,*res,*n);  
}

MCH_CDK_2017::MCH_CDK_2017(mpz_t *n, mpz_t *e, mpz_t *d){
    this->rsa = new MyRSA(n,e,d);
}

void MCH_CDK_2017::CParGen(mpz_t *n, mpz_t *e, mpz_t *d){
    // Call RSAKGen with the restriction e > n, and e prime. Return e.
    // this->rsa->rsa_generate_keys_2(1024, 1);

    // printf("%d\n",mpz_probab_prime_p(*e, 25));
    
    // while (mpz_cmp(*e, *n) <= 0 || mpz_probab_prime_p(*e, 25) == 0)  // 素性测试25次
    // {
    //     printf("1");
    //     this->rsa->rsa_generate_keys(1024);
    // }

    // return e
}

void MCH_CDK_2017::CKGen(mpz_t *n, mpz_t *e, mpz_t *d){
    // Generate two primes p and q using RSAKGen(1λ)
    this->rsa->rsa_generate_keys_2(1024, 1);
    // 输出d的大小(bytes)
    size_t bits = mpz_sizeinbase(*d, 2);
    size_t bytes = (bits + 7) / 8;
    printf("sizeof(d): %zu bytes\n", bytes);
    // return n,d
}

void MCH_CDK_2017::CHash(mpz_t *h, mpz_t *r, mpz_t *n,mpz_t *e, mpz_t *m){
    // Draw r ← Zn*
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));
    mpz_urandomm(*r, state, *n);

    mpz_t g;
    mpz_init(g);
    // Let g ← Hn(m)
    this->H(m, &g, n);
    
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

bool MCH_CDK_2017::CHashCheck(mpz_t *h_, mpz_t *m, mpz_t *n, mpz_t *e, mpz_t *r){
    // If r ∈ Zn*, return false
    if(mpz_cmp_ui(*r, 0) <= 0 || mpz_cmp(*r, *n) >= 0){
        return false;
    }
    mpz_t gcd_result;
    mpz_init(gcd_result);
    mpz_gcd(gcd_result, *r, *n);
    if(mpz_cmp_ui(gcd_result, 1) != 0){
        mpz_clear(gcd_result);
        return false;
    }
    mpz_clear(gcd_result);

    mpz_t g;
    mpz_init(g);
    // Let g ← Hn(m)
    this->H(m, &g, n);
    
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

void MCH_CDK_2017::Adapt(mpz_t *r_p, mpz_t *m_p, mpz_t *m, mpz_t *r, mpz_t *h, mpz_t *n,mpz_t *e,mpz_t *d){
    if(this->CHashCheck(h, m, n, e, r) == false){
        return;
    }
    // if m = m0, return r.
    if(mpz_cmp(*m, *m_p) == 0){
        mpz_set(*r_p, *r);
        return;
    }

    mpz_t g,tmp,y;
    mpz_init(g);   
    mpz_init(tmp);
    mpz_init(y);
    // Let g ← Hn(m), and y ← gre mod n.
    this->H(m, &g, n);
    
    mpz_powm(tmp, *r, *e, *n);
    mpz_mul(y, g, tmp);
    mpz_mod(y, y, *n);
    mpz_clear(tmp);
    mpz_clear(g);
    
    // Let g' ← Hn(m')
    mpz_t g_p;
    mpz_init(g_p);
    this->H(m_p, &g_p, n);

    // Return r0' ← (y(g'−1))d mod n.

    mpz_t tmp_1;  
    mpz_t tmp_2;
    mpz_init(tmp_1);
    mpz_init(tmp_2);
    mpz_invert(tmp_1, g_p, *n);  
    mpz_mul(tmp_2, y, tmp_1);
    mpz_mod(tmp_2, tmp_2, *n);
    mpz_powm(*r_p, tmp_2, *d, *n);

    mpz_clear(tmp_1);
    mpz_clear(tmp_2);
    mpz_clear(g_p);
    mpz_clear(y);

}

void MCH_CDK_2017::MCH_CDK_2017_clear(){
    this->rsa->rsa_clear();
}
