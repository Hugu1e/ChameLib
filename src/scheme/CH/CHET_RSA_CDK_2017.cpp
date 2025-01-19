#include <scheme/CHET_RSA_CDK_2017.h>

void CHET_RSA_CDK_2017::H(mpz_t *m, mpz_t *res, mpz_t *n){
    Hm_n(*m,*res,*n);  
}

CHET_RSA_CDK_2017::CHET_RSA_CDK_2017(mpz_t *n, mpz_t *e, mpz_t *d){
    this->rsa = new MyRSA(n,e,d);
}

void CHET_RSA_CDK_2017::CParGen(mpz_t *n, mpz_t *e, mpz_t *d){

}

void CHET_RSA_CDK_2017::CKGen(mpz_t *n, mpz_t *e, mpz_t *d){
    // Generate two primes p and q using RSAKGen(1λ)
    // n,e
    this->rsa->rsa_generate_keys_2(1024, 3, &this->phi);
    
}

void CHET_RSA_CDK_2017::CHash(mpz_t *h, mpz_t *etd_n, mpz_t *r,mpz_t *etd_p, mpz_t *etd_q, mpz_t *n,mpz_t *e, mpz_t *m){
    // Generate two primes p0 and q0 using RSAKGen(1λ). Set etd ← (p0, q0),and n0 ← p0q0.
    this->rsa->rsa_generate_keys_pqn(1024, etd_p, etd_q, etd_n);
    mpz_t gcd_result;
    mpz_init(gcd_result);
    mpz_gcd(gcd_result, *n, *etd_n);
    while (mpz_cmp_ui(gcd_result, 1) != 0)
    {
        // If gcd(n, n0) != 1 , go to 1.
        this->rsa->rsa_generate_keys_pqn(1024, etd_p, etd_q, etd_n);
        mpz_gcd(gcd_result, *n, *etd_n);
        gmp_printf("gcd_result: %Zd\n", gcd_result);
    }
    mpz_clear(gcd_result);

    // Draw r ← Znn'*
    mpz_t tmp_nn;
    mpz_init(tmp_nn);
    mpz_mul(tmp_nn, *n, *etd_n);
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));
    mpz_urandomm(*r, state, tmp_nn);

    // Let g ← Hnn' (m), and h ← gr^e mod nn'.
    mpz_t g;
    mpz_init(g);
    this->H(m, &g, &tmp_nn);
    
    // h ← gr^e mod nn'
    mpz_t tmp;
    mpz_init(tmp);
    mpz_powm(tmp, *r, *e, tmp_nn);
    mpz_mul(*h, g, tmp);
    mpz_mod(*h, *h, tmp_nn);

    mpz_clear(tmp);
    mpz_clear(g);
    mpz_clear(tmp_nn);
}

bool CHET_RSA_CDK_2017::CHashCheck(mpz_t *h_, mpz_t *m, mpz_t *n, mpz_t *etd_n,mpz_t *e, mpz_t *r){
    // If r ∈ Znn'*, return false
    mpz_t tmp_nn;
    mpz_init(tmp_nn);
    mpz_mul(tmp_nn, *n, *etd_n);
    if(mpz_cmp_ui(*r, 0) <= 0 || mpz_cmp(*r, tmp_nn) >= 0){
        return false;
    }
    mpz_t gcd_result;
    mpz_init(gcd_result);
    mpz_gcd(gcd_result, *r, tmp_nn);
    if(mpz_cmp_ui(gcd_result, 1) != 0){
        mpz_clear(gcd_result);
        return false;
    }
    mpz_clear(gcd_result);

    // Let g ← Hnn' (m), and h ← gr^e mod nn'.
    mpz_t g;
    mpz_init(g);
    this->H(m, &g, &tmp_nn);
    
    // h ← gr^e mod n
    mpz_t tmp;
    mpz_t tmp_2;
    mpz_init(tmp);
    mpz_init(tmp_2);
    mpz_powm(tmp, *r, *e, tmp_nn);
    mpz_mul(tmp_2, g, tmp);
    mpz_mod(tmp_2, tmp_2, tmp_nn);
    mpz_clear(tmp);
    mpz_clear(g);
    mpz_clear(tmp_nn);
    
    gmp_printf("tmp_2: %Zd\n", tmp_2);
    gmp_printf("h_: %Zd\n", *h_);

    if(mpz_cmp(tmp_2, *h_) == 0){
        mpz_clear(tmp_2);
        return true;
    }else{
        mpz_clear(tmp_2);
        return false;
    }
}

bool CHET_RSA_CDK_2017::Adapt(mpz_t *r_p, mpz_t *m_p, mpz_t *m, mpz_t *r, mpz_t *h, mpz_t *n,mpz_t *etd_n,mpz_t *etd_p,mpz_t *etd_q,mpz_t *e){
    // Check that n0 = p0q0, where p0 and q0 is taken from etd. If this is not thecase, return ⊥.
    mpz_t tmp;
    mpz_init(tmp);
    mpz_mul(tmp, *etd_p, *etd_q);
    if(mpz_cmp(tmp, *etd_n) != 0){
        return false;
    }
    
    // If CHashCheck(pkch, m, r, h) = false, return ⊥.
    if(this->CHashCheck(h, m, n, etd_n, e, r) == false){
        return false;
    }

    // Compute d s.t. de ≡ 1 mod ϕ(nn').
    mpz_t tmp_nn,tmp_d;
    mpz_inits(tmp_nn, tmp_d, NULL);
    mpz_mul(tmp_nn, *n, *etd_n);
    // ϕ(nn') = (p-1)(q-1)(etd_p-1)(etd_q-1)
    mpz_t etd_p_minus_1, etd_q_minus_1, phi;
    mpz_inits(etd_p_minus_1, etd_q_minus_1, phi, NULL);
    mpz_sub_ui(etd_p_minus_1, *etd_p, 1);
    mpz_sub_ui(etd_q_minus_1, *etd_q, 1);
    mpz_mul(phi, etd_p_minus_1, etd_q_minus_1);
    mpz_mul(phi, phi, this->phi);
    // compute d
    mpz_invert(tmp_d, *e, phi);

    // 验证 d 计算是否正确
    mpz_t tmp_test;
    mpz_init(tmp_test);
    mpz_mul(tmp_test, *e, tmp_d);
    mpz_mod(tmp_test, tmp_test, phi);
    gmp_printf("tmp_test: %Zd\n", tmp_test);
    if(mpz_cmp_ui(tmp_test, 1) != 0){
        return false;
    }
    
    gmp_printf("tmp_d: %Zd\n", tmp_d);
    gmp_printf("phi: %Zd\n", phi);

    // Let g' ← Hnn' (m') and r' ← (h(g'−1))d mod nn0.
    mpz_t g_p,tmp_1,tmp_2;
    mpz_inits(g_p, tmp_1,tmp_2,NULL);   
    this->H(m_p, &g_p, &tmp_nn);
 
    mpz_invert(tmp_1, g_p, tmp_nn);  
    mpz_mul(tmp_2, *h, tmp_1);
    mpz_mod(tmp_2, tmp_2, tmp_nn);
    mpz_powm(*r_p, tmp_2, tmp_d, tmp_nn);

    mpz_clears(tmp,tmp_d,tmp_nn,etd_p_minus_1,etd_q_minus_1,phi,g_p,tmp_1,tmp_2,NULL);

    return true;
}

void CHET_RSA_CDK_2017::CHET_RSA_CDK_2017_clear(){
    this->rsa->rsa_clear();
}
