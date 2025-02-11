#include <scheme/CH/CH_KEF_NoMH_AM_2004.h>

CH_KEF_NoMH_AM_2004::CH_KEF_NoMH_AM_2004() {}


void CH_KEF_NoMH_AM_2004::Setup(CH_KEF_NoMH_AM_2004_pk &pk, CH_KEF_NoMH_AM_2004_sk &sk, 
    CH_KEF_NoMH_AM_2004_h &h, CH_KEF_NoMH_AM_2004_r &r, CH_KEF_NoMH_AM_2004_r &r_p){
    pk.init(4);
    sk.init(1); 
    h.init(1);
    r.init(2);
    r_p.init(2);
}


void CH_KEF_NoMH_AM_2004::KeyGen(CH_KEF_NoMH_AM_2004_pk &pk, CH_KEF_NoMH_AM_2004_sk &sk, int k) {
    RandomGenerator::RandomPrimeInLength(pk[q], k);
    // p = 2q + 1
    mpz_mul_ui(pk[p], pk[q], 2);
    mpz_add_ui(pk[p], pk[p], 1);
    while(!mpz_probab_prime_p(pk[p], 25)){
        RandomGenerator::RandomPrimeInLength(pk[q], k);
        // p = 2q + 1
        mpz_mul_ui(pk[p], pk[q], 2);
        mpz_add_ui(pk[p], pk[p], 1);
    }
    // random g until pow(g, q, p) = 1
    mpz_t tmp;
    mpz_init(tmp);
    RandomGenerator::RandomN(pk[g], pk[p]);
    mpz_powm(tmp, pk[g], pk[q], pk[p]);
    while(mpz_cmp_ui(tmp, 1) != 0){
        RandomGenerator::RandomN(pk[g], pk[p]);
        mpz_powm(tmp, pk[g], pk[q], pk[p]);
    }

    // y = g^x
    RandomGenerator::RandomN(sk[x], pk[q]);
    mpz_powm(pk[y], pk[g], sk[x], pk[p]);
}


void CH_KEF_NoMH_AM_2004::Hash(CH_KEF_NoMH_AM_2004_h &h, CH_KEF_NoMH_AM_2004_r &r, mpz_t m, CH_KEF_NoMH_AM_2004_pk &pk) {
    mpz_t e, tmp, tmp_2;
    mpz_inits(e, tmp, tmp_2, NULL);
    
    // random r,s
    RandomGenerator::RandomN(r[r1], pk[q]);
    RandomGenerator::RandomN(r[s1], pk[q]);

    // e = H(m,r)
    HASH::hash_n(e, m, r[r1], pk[q]);

    // h = r - ((y^e)(g^s) mod p)mod q
    mpz_powm(tmp, pk[y], e, pk[p]);
    mpz_powm(tmp_2, pk[g], r[s1], pk[p]);
    mpz_mul(tmp, tmp, tmp_2);
    mpz_mod(tmp, tmp, pk[p]);
    mpz_sub(h[h1], r[r1], tmp);
    mpz_mod(h[h1], h[h1], pk[q]);

    mpz_clears(e, tmp, tmp_2, NULL);
}


bool CH_KEF_NoMH_AM_2004::Check(CH_KEF_NoMH_AM_2004_h &h, CH_KEF_NoMH_AM_2004_r &r, mpz_t m, CH_KEF_NoMH_AM_2004_pk &pk){
    mpz_t e, tmp, tmp_2;
    mpz_inits(e, tmp, tmp_2, NULL);
    
    // e = H(m,r)
    HASH::hash_n(e, m, r[r1], pk[q]);

    // h = r - ((y^e)(g^s) mod p)mod q
    mpz_powm(tmp, pk[y], e, pk[p]);
    mpz_powm(tmp_2, pk[g], r[s1], pk[p]);
    mpz_mul(tmp, tmp, tmp_2);
    mpz_mod(tmp, tmp, pk[p]);
    mpz_sub(tmp_2, r[r1], tmp);
    mpz_mod(tmp_2, tmp_2, pk[q]);

    bool result = mpz_cmp(tmp_2, h[h1]) == 0;

    mpz_clears(e, tmp, tmp_2, NULL);
    return result;
}



void CH_KEF_NoMH_AM_2004::Adapt(CH_KEF_NoMH_AM_2004_r &r_p, mpz_t m_p, 
    CH_KEF_NoMH_AM_2004_h &h, CH_KEF_NoMH_AM_2004_r &r, mpz_t m, CH_KEF_NoMH_AM_2004_pk &pk, CH_KEF_NoMH_AM_2004_sk &sk){
    
    mpz_t k_, e, tmp;
    mpz_inits(k_, e, tmp, NULL);
    // k'
    RandomGenerator::RandomN(k_, pk[q]);

    // r_p = h + (g^k_p mod p) mod q
    mpz_powm(tmp, pk[g], k_, pk[p]);
    mpz_add(r_p[r1], h[h1], tmp);
    mpz_mod(r_p[r1], r_p[r1], pk[q]);

    // e_p = H(m_p, r_p)
    HASH::hash_n(e, m_p, r_p[r1], pk[q]);

    // s_p = k_p - e_p*x mod q
    mpz_mul(tmp, e, sk[x]);
    mpz_sub(r_p[s1], k_, tmp);
    mpz_mod(r_p[s1], r_p[s1], pk[q]);

    mpz_clears(k_, e, tmp, NULL);
}


bool CH_KEF_NoMH_AM_2004::Verify(CH_KEF_NoMH_AM_2004_h &h, CH_KEF_NoMH_AM_2004_r &r_p, mpz_t m_p, CH_KEF_NoMH_AM_2004_pk &pk){
    return this->Check(h, r_p, m_p, pk);
}

CH_KEF_NoMH_AM_2004::~CH_KEF_NoMH_AM_2004() {}