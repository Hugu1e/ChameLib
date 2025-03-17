#include <scheme/CH/CH_KEF_MH_RSANN_F_AM_2004.h>

CH_KEF_MH_RSANN_F_AM_2004::CH_KEF_MH_RSANN_F_AM_2004(){}

void CH_KEF_MH_RSANN_F_AM_2004::H(mpz_t res, mpz_t m, mpz_t n){
    HASH::hash_n(res, m, n);
    mpz_nextprime(res, res);
}

void CH_KEF_MH_RSANN_F_AM_2004::lcm(mpz_t res, mpz_t a, mpz_t b){
    mpz_t gcd;
    mpz_init(gcd);
    mpz_gcd(gcd, a, b);
    mpz_mul(res, a, b);
    mpz_div(res, res, gcd);
    mpz_clear(gcd);
}

void CH_KEF_MH_RSANN_F_AM_2004::L(mpz_t res, mpz_t x, mpz_t n){
    // L(x) = (x-1)/n
    mpz_sub_ui(res, x, 1);
    mpz_div(res, res, n);
}

void CH_KEF_MH_RSANN_F_AM_2004::SetUp(){}

void CH_KEF_MH_RSANN_F_AM_2004::KeyGen(CH_KEF_MH_RSANN_F_AM_2004_pk &pk, CH_KEF_MH_RSANN_F_AM_2004_sk &sk, int k){
    RandomGenerator::RandomPrimeInLength(sk[p], k);
    RandomGenerator::RandomPrimeInLength(sk[q], k);
    mpz_mul(pk[n], sk[p], sk[q]);
}

void CH_KEF_MH_RSANN_F_AM_2004::Hash(CH_KEF_MH_RSANN_F_AM_2004_h &h, CH_KEF_MH_RSANN_F_AM_2004_r &r, mpz_t m, mpz_t Label, CH_KEF_MH_RSANN_F_AM_2004_pk &pk){
    mpz_t h_, n_pow_2, tmp, tmp_2, tmp_3;
    mpz_inits(h_, n_pow_2, tmp, tmp_2, tmp_3, NULL);

    // n^2
    mpz_mul(n_pow_2, pk[n], pk[n]);

    // h_ = H(Label)
    H(h_, Label, n_pow_2);

    RandomGenerator::RandomN(r[r1], pk[n]);
    RandomGenerator::RandomCoprimeN(r[r2], pk[n]);

    //(1 + mn) h^r1 r2^n
    mpz_mul(tmp, m, pk[n]);
    mpz_add_ui(tmp, tmp, 1);
    mpz_powm(tmp_2, h_, r[r1], n_pow_2);
    mpz_powm(tmp_3, r[r2], pk[n], n_pow_2);
    mpz_mul(tmp, tmp, tmp_2);
    mpz_mul(tmp, tmp, tmp_3);
    mpz_mod(h[h1], tmp, n_pow_2);

    mpz_clears(h_, n_pow_2, tmp, tmp_2, tmp_3, NULL);
}

bool CH_KEF_MH_RSANN_F_AM_2004::Check(CH_KEF_MH_RSANN_F_AM_2004_h &h, mpz_t m, CH_KEF_MH_RSANN_F_AM_2004_r &r, mpz_t Label, CH_KEF_MH_RSANN_F_AM_2004_pk &pk){
    mpz_t h_, n_pow_2, tmp, tmp_2, tmp_3;
    mpz_inits(h_, n_pow_2, tmp, tmp_2, tmp_3, NULL);

    // n^2
    mpz_mul(n_pow_2, pk[n], pk[n]);

    // h_ = H(Label)
    H(h_, Label, n_pow_2);

    //(1 + mn) h^r1 r2^n
    mpz_mul(tmp, m, pk[n]);
    mpz_add_ui(tmp, tmp, 1);
    mpz_powm(tmp_2, h_, r[r1], n_pow_2);
    mpz_powm(tmp_3, r[r2], pk[n], n_pow_2);
    mpz_mul(tmp, tmp, tmp_2);
    mpz_mul(tmp, tmp, tmp_3);
    mpz_mod(tmp, tmp, n_pow_2);

    if(mpz_cmp(h[h1], tmp) == 0){
        mpz_clears(h_, n_pow_2, tmp, tmp_2, tmp_3, NULL);
        return true;
    }
    else{
        mpz_clears(h_, n_pow_2, tmp, tmp_2, tmp_3, NULL);
        return false;
    }
}

void CH_KEF_MH_RSANN_F_AM_2004::Adapt(CH_KEF_MH_RSANN_F_AM_2004_r &r_p, mpz_t m_p, CH_KEF_MH_RSANN_F_AM_2004_h &h, mpz_t m, CH_KEF_MH_RSANN_F_AM_2004_r &r, mpz_t Label, CH_KEF_MH_RSANN_F_AM_2004_sk &sk, CH_KEF_MH_RSANN_F_AM_2004_pk &pk){
    mpz_t tmp, tmp_2, tmp_3, tmp_4, c, lamuda, n_pow_2, h_;
    mpz_inits(tmp, tmp_2, tmp_3, tmp_4, c, lamuda, n_pow_2, h_, NULL);

    // n^2
    mpz_mul(n_pow_2, pk[n], pk[n]);

    // h_ = H(Label)
    H(h_, Label, n_pow_2);

    // c' = c(1-m'n) mod n^2
    mpz_mul(tmp, m_p, pk[n]);
    mpz_set_ui(tmp_2, 1);
    mpz_sub(tmp_2, tmp_2, tmp);
    mpz_mul(c, h[h1], tmp_2);
    mpz_mod(c, c, n_pow_2);

    // lamuda = lcm(p-1, q-1)
    mpz_sub_ui(tmp, sk[p], 1);
    mpz_sub_ui(tmp_2, sk[q], 1);
    lcm(lamuda, tmp, tmp_2);

    // L(c^lamuda mod n^2)
    mpz_powm(tmp, c, lamuda, n_pow_2);
    L(tmp_2, tmp, pk[n]);
    // L(h_^lamuda mod n^2)
    mpz_powm(tmp, h_, lamuda, n_pow_2);
    L(tmp_3, tmp, pk[n]);
    // a = r1' = L(c^lamuda mod n^2) / L(h_^lamuda mod n^2) mod n
    mpz_invert(tmp_4, tmp_3, pk[n]);
    mpz_mul(r_p[r1], tmp_2, tmp_4);
    mpz_mod(r_p[r1], r_p[r1], pk[n]);

    // c * (h_^-r1') mod n
    mpz_powm(tmp, h_, r_p[r1], pk[n]);
    mpz_invert(tmp, tmp, pk[n]);
    mpz_mul(tmp, c, tmp);
    mpz_mod(tmp, tmp, pk[n]);
    // b = r2' = (c * (h_^-r1'))^(n^-1 mod lamuda)  mod n
    mpz_invert(tmp_2, pk[n], lamuda);
    mpz_powm(r_p[r2], tmp, tmp_2, pk[n]);

    mpz_clears(tmp, tmp_2, tmp_3, tmp_4, c, lamuda, n_pow_2, h_, NULL);
}

bool CH_KEF_MH_RSANN_F_AM_2004::Verify(CH_KEF_MH_RSANN_F_AM_2004_h &h, mpz_t m_p, CH_KEF_MH_RSANN_F_AM_2004_r &r_p, mpz_t Label, CH_KEF_MH_RSANN_F_AM_2004_pk &pk){
    return Check(h, m_p, r_p, Label, pk);
}

CH_KEF_MH_RSANN_F_AM_2004::~CH_KEF_MH_RSANN_F_AM_2004(){}
