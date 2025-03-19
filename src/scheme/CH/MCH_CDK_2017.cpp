#include <scheme/CH/MCH_CDK_2017.h>

MCH_CDK_2017::MCH_CDK_2017(){}

void MCH_CDK_2017::SetUp(){}

void MCH_CDK_2017::KeyGen(MCH_CDK_2017_pk &pk, MCH_CDK_2017_sk &sk, short k){
    rsa.KeyGen(pk[n], pk[e], sk[d], k, 1);
}

void MCH_CDK_2017::H(mpz_t res, mpz_t m, mpz_t n){
    HASH::hash_n(res, m, n);  
}

void MCH_CDK_2017::Hash(MCH_CDK_2017_h &h, MCH_CDK_2017_r &r, mpz_t m, MCH_CDK_2017_pk &pk){
    mpz_t _r,_h,g,tmp;
    mpz_inits(_r,_h,g,tmp,NULL);


    // Draw r ← Zn*
    RandomGenerator::RandomCoprimeN(_r, pk[n]);
    r.set(r1, _r);

    // Let g ← Hn(m)
    this->H(g, m, pk[n]);
    
    // h ← gr^e mod n

    mpz_powm(tmp, _r, pk[e], pk[n]);
    mpz_mul(_h, g, tmp);
    mpz_mod(_h, _h, pk[n]);

    h.set(h1, _h);

    mpz_clears(_r,_h,g,tmp,NULL);
}

bool MCH_CDK_2017::Check(MCH_CDK_2017_h &h, MCH_CDK_2017_r &r, mpz_t m, MCH_CDK_2017_pk &pk){
    mpz_t _r,gcd_result,g,tmp,tmp_2;
    mpz_inits(_r,gcd_result,g,tmp,tmp_2,NULL);

    mpz_set(_r, r[r1]);

    // If r ∈ Zn*, return false
    if(mpz_cmp(_r, pk[n]) >= 0){
        return false;
    }
    mpz_gcd(gcd_result, _r, pk[n]);
    if(mpz_cmp_ui(gcd_result, 1) != 0){
        throw std::runtime_error("MCH_CDK_2017::Check(): r is not coprime with n");
    }

    // Let g ← Hn(m)
    this->H(g, m, pk[n]);
    
    // h ← gr^e mod n
    mpz_powm(tmp, _r, pk[e], pk[n]);
    mpz_mul(tmp_2, g, tmp);
    mpz_mod(tmp_2, tmp_2, pk[n]);

    if(mpz_cmp(tmp_2, h[h1]) == 0){
        mpz_clears(_r,gcd_result,g,tmp,tmp_2,NULL);
        return true;
    }else{
        mpz_clears(_r,gcd_result,g,tmp,tmp_2,NULL);
        return false;
    }
}

void MCH_CDK_2017::Adapt(MCH_CDK_2017_r &r_p, mpz_t m_p, mpz_t m, MCH_CDK_2017_r &r, MCH_CDK_2017_h &h, MCH_CDK_2017_sk &sk, MCH_CDK_2017_pk &pk){
    if(this->Check(h, r, m, pk) == false){
        throw std::runtime_error("MCH_CDK_2017::Adapt(): hash check failed");
    }
    // if m = m0, return r.
    if(mpz_cmp(m, m_p) == 0){
        r_p.set(r1, r[r1]);
        return;
    }

    mpz_t g,tmp,y,_r,g_p,tmp_1,tmp_2,_r_p;
    mpz_inits(g,tmp,y,_r,g_p,tmp_1,tmp_2,_r_p,NULL);

 
    mpz_set(_r, r[r1]);

    // Let g ← Hn(m), and y ← gre mod n.
    this->H(g, m, pk[n]);
    
    mpz_powm(tmp, _r, pk[e], pk[n]);
    mpz_mul(y, g, tmp);
    mpz_mod(y, y, pk[n]);

    
    // Let g' ← Hn(m')
    this->H(g_p, m_p, pk[n]);

    // Return r0' ← (y(g'−1))d mod n.
    mpz_invert(tmp_1, g_p, pk[n]);  
    mpz_mul(tmp_2, y, tmp_1);
    mpz_mod(tmp_2, tmp_2, pk[n]);
    mpz_powm(_r_p, tmp_2, sk[d], pk[n]);

    r_p.set(r1, _r_p);

    mpz_clears(g,tmp,y,_r,g_p,tmp_1,tmp_2,_r_p,NULL);
}

bool MCH_CDK_2017::Verify(MCH_CDK_2017_h &h, MCH_CDK_2017_r &r_p, mpz_t m_p, MCH_CDK_2017_pk &pk){
    return this->Check(h, r_p, m_p, pk);
}

MCH_CDK_2017::~MCH_CDK_2017(){}
