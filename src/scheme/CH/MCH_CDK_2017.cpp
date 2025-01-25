#include <scheme/CH/MCH_CDK_2017.h>

MCH_CDK_2017::MCH_CDK_2017(){}

void MCH_CDK_2017::KeyGen(MCH_CDK_2017_pk *pk, MCH_CDK_2017_sk *sk, short k){
    mpz_t n, e, d;
    mpz_inits(n, e, d, NULL);

    rsa.KeyGen(n, e, d, k, 1);
    
    pk->insertElement("n", n);
    pk->insertElement("e", e);
    sk->insertElement("d", d);

    mpz_clears(n, e, d, NULL);
}

void MCH_CDK_2017::H(mpz_t res, mpz_t m, mpz_t n){
    HASH::hash_n(res, m, n);  
}

void MCH_CDK_2017::Hash(MCH_CDK_2017_h *h, MCH_CDK_2017_r *r, mpz_t m, MCH_CDK_2017_pk *pk){
    mpz_t _r,_h,g,n,tmp,e;
    mpz_inits(_r,_h,g,n,tmp,e,NULL);

    mpz_set(n, pk->getElement("n"));
    mpz_set(e, pk->getElement("e"));

    // Draw r ← Zn*
    RandomGenerator::RandomCoprimeN(_r, n);
    r->insertElement("r", _r);

    // Let g ← Hn(m)
    this->H(g, m, n);
    
    // h ← gr^e mod n

    mpz_powm(tmp, _r, e, n);
    mpz_mul(_h, g, tmp);
    mpz_mod(_h, _h, n);

    h->insertElement("h", _h);

    mpz_clears(_r,_h,g,n,tmp,e,NULL);
}

bool MCH_CDK_2017::Check(MCH_CDK_2017_h *h, MCH_CDK_2017_r *r, mpz_t m, MCH_CDK_2017_pk *pk){
    mpz_t n,e,_r,gcd_result,g,tmp,tmp_2;
    mpz_inits(n,e,_r,gcd_result,g,tmp,tmp_2,NULL);

    mpz_set(n, pk->getElement("n"));
    mpz_set(e, pk->getElement("e"));
    mpz_set(_r, r->getElement("r"));

    // If r ∈ Zn*, return false
    if(mpz_cmp(_r, n) >= 0){
        return false;
    }
    mpz_gcd(gcd_result, _r, n);
    if(mpz_cmp_ui(gcd_result, 1) != 0){
        throw std::invalid_argument("MCH_CDK_2017::Check(): r is not coprime with n");
    }

    // Let g ← Hn(m)
    this->H(g, m, n);
    
    // h ← gr^e mod n
    mpz_powm(tmp, _r, e, n);
    mpz_mul(tmp_2, g, tmp);
    mpz_mod(tmp_2, tmp_2, n);

    if(mpz_cmp(tmp_2, h->getElement("h")) == 0){
        mpz_clears(n,e,_r,gcd_result,g,tmp,tmp_2,NULL);
        return true;
    }else{
        mpz_clears(n,e,_r,gcd_result,g,tmp,tmp_2,NULL);
        return false;
    }
}

void MCH_CDK_2017::Adapt(MCH_CDK_2017_r *r_p, mpz_t m_p, mpz_t m, MCH_CDK_2017_r *r, MCH_CDK_2017_h *h, MCH_CDK_2017_sk *sk, MCH_CDK_2017_pk *pk){
    if(this->Check(h, r, m, pk) == false){
        throw std::invalid_argument("MCH_CDK_2017::Adapt(): hash check failed");
    }
    // if m = m0, return r.
    if(mpz_cmp(m, m_p) == 0){
        r_p->insertElement("r", r->getElement("r"));
        return;
    }

    mpz_t g,tmp,y,n,e,_r,g_p,tmp_1,tmp_2,_r_p;
    mpz_inits(g,tmp,y,n,e,_r,g_p,tmp_1,tmp_2,_r_p,NULL);

    mpz_set(n, pk->getElement("n"));
    mpz_set(e, pk->getElement("e"));
    mpz_set(_r, r->getElement("r"));

    // Let g ← Hn(m), and y ← gre mod n.
    this->H(g, m, n);
    
    mpz_powm(tmp, _r, e, n);
    mpz_mul(y, g, tmp);
    mpz_mod(y, y, n);

    
    // Let g' ← Hn(m')
    this->H(g_p, m_p, n);

    // Return r0' ← (y(g'−1))d mod n.
    mpz_invert(tmp_1, g_p, n);  
    mpz_mul(tmp_2, y, tmp_1);
    mpz_mod(tmp_2, tmp_2, n);
    mpz_powm(_r_p, tmp_2, sk->getElement("d"), n);

    r_p->insertElement("r", _r_p);

    mpz_clears(g,tmp,y,n,e,_r,g_p,tmp_1,tmp_2,_r_p,NULL);
}

bool MCH_CDK_2017::Verify(MCH_CDK_2017_h *h, MCH_CDK_2017_r *r_p, mpz_t m_p, MCH_CDK_2017_pk *pk){
    return this->Check(h, r_p, m_p, pk);
}

void MCH_CDK_2017::MCH_CDK_2017_clear(){}
