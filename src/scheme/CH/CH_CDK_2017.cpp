#include <scheme/CH/CH_CDK_2017.h>

CH_CDK_2017::CH_CDK_2017(){}

void CH_CDK_2017::SetUp(){}

void CH_CDK_2017::KeyGen(CH_CDK_2017_pk &pk, CH_CDK_2017_sk &sk, short k){
    rsa.KeyGen(pk.get_rsa_pk(), sk.get_rsa_sk(), k);
}

void CH_CDK_2017::H(mpz_t res, mpz_t m1, mpz_t m2, mpz_t n){
    HASH::hash_n(res, m1, m2, n);  
}

void CH_CDK_2017::Hash(CH_CDK_2017_h &h, CH_CDK_2017_r &r, mpz_t m, mpz_t tag, CH_CDK_2017_pk &pk){
    // Draw r ← Zn*
    mpz_t _r,_h,n,e,g,tmp;
    mpz_inits(_r,_h,n,e,g,tmp,NULL);

    mpz_set(n, pk.get_rsa_pk()[AE_RSA::n]);
    mpz_set(e, pk.get_rsa_pk()[AE_RSA::e]);

    RandomGenerator::RandomCoprimeN(_r, n);
    r.set(r1, _r);

    // Let g ← Hn(τ, m)
    this->H(g, tag, m, n);
    
    // h ← gr^e mod n
    mpz_powm(tmp, _r, e, n);
    mpz_mul(_h, g, tmp);
    mpz_mod(_h, _h, n);
    h.set(h1, _h);

    mpz_clears(_r,_h,n,e,g,tmp,NULL);
}

bool CH_CDK_2017::Check(CH_CDK_2017_h &h, CH_CDK_2017_r &r, mpz_t m, mpz_t tag, CH_CDK_2017_pk &pk){
    bool CheckResult = false;

    mpz_t g,n,tmp,tmp_2,_r,e,_h;
    mpz_inits(g,n,tmp,tmp_2,_r,e,_h,NULL);

    mpz_set(n, pk.get_rsa_pk()[AE_RSA::n]);
    mpz_set(_r, r[r1]);
    mpz_set(e, pk.get_rsa_pk()[AE_RSA::e]);
    mpz_set(_h, h[h1]);

    // Let g ← Hn(τ, m)
    this->H(g, tag, m, n);
    
    // h ← gr^e mod n
    mpz_powm(tmp, _r, e, n);
    mpz_mul(tmp_2, g, tmp);
    mpz_mod(tmp_2, tmp_2, n);

    if(mpz_cmp(tmp_2, _h) == 0){
        CheckResult = true;
    }
    mpz_clears(g,n,tmp,tmp_2,_r,e,_h,NULL);
    return CheckResult;
}

void CH_CDK_2017::Adapt(CH_CDK_2017_r &r_p, mpz_t tag_p, CH_CDK_2017_h &h, CH_CDK_2017_r &r, mpz_t m_p, CH_CDK_2017_sk &sk, CH_CDK_2017_pk &pk){
    mpz_t n,_r,e,tmp_1,tmp_2,_h,_r_p, p,q,phi,d;
    mpz_inits(n,_r,e,tmp_1,tmp_2,_h,_r_p,p,q,phi,d,NULL);

    mpz_set(n, pk.get_rsa_pk()[AE_RSA::n]);
    mpz_set(_r, r[r1]);
    mpz_set(e, pk.get_rsa_pk()[AE_RSA::e]);
    mpz_set(_h, h[h1]);
    mpz_set(d, sk.get_rsa_sk()[AE_RSA::d]);
    

    RandomGenerator::RandomN(tag_p, n);

    this->H(tmp_1,tag_p, m_p, n);
    mpz_invert(tmp_2, tmp_1, n);  
    mpz_mul(tmp_1, _h, tmp_2);
    mpz_mod(tmp_1, tmp_1, n);
    mpz_powm(_r_p, tmp_1, d, n);

    r_p.set(r1, _r_p);

    mpz_clears(n,_r,e,tmp_1,tmp_2,_h,_r_p,p,q,phi,d,NULL);
}

bool CH_CDK_2017::Verify(CH_CDK_2017_h &h, CH_CDK_2017_r &r_p, mpz_t m_p, mpz_t tag_p, CH_CDK_2017_pk &pk){
    return Check(h, r_p, m_p, tag_p, pk);
}

CH_CDK_2017::~CH_CDK_2017(){}

