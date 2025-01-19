#include <scheme/CH_KEF_MH_RSA_F_AM_2004.h>

CH_KEF_MH_RSA_F_AM_2004::CH_KEF_MH_RSA_F_AM_2004(pk *pk, sk *sk){
    this->rsa = new MyRSA(&pk->n, &pk->e, &sk->d);
}

/**
 * @param k security parameter
 * @param t security parameter
 * @param pk public key
 * @param sk secret key
 */
void CH_KEF_MH_RSA_F_AM_2004::KGen(int k, int t, pk *pk, sk *sk){
    this->k = k;
    this->t = t;
    // generate e s.t. e > 2^t
    GenerateRandomWithLength(pk->e, t+1);
    mpz_nextprime(pk->e, pk->e);
    // Generate two primes p and q using RSAKGen
    this->rsa->rsa_generate_keys_with_e(k, &pk->e);
}

/**
 * a collision-resistanthash function mapping strings of arbitrary length to strings of ﬁxed length τ .
 * 
 */
void CH_KEF_MH_RSA_F_AM_2004::H(mpz_t *m, mpz_t *res, int t){
    mpz_t n;
    mpz_init(n);
    // n = 2^t
    mpz_ui_pow_ui(n, 2, t);
    Hm_n(*m,*res, n);
    mpz_clear(n);  
}

/**
 * @param pk public key
 * @param sk secret key
 * @param m message
 * @param tag tag
 * @param h hash
 * @param B secret trapdoor
 * @param r random number
 */
void CH_KEF_MH_RSA_F_AM_2004::Hash(pk *pk, sk *sk, mpz_t *m, mpz_t *tag, mpz_t *h, mpz_t *B, mpz_t *r){
    mpz_t J,tmp,tmp_2;
    mpz_inits(J,tmp,tmp_2,NULL);
    // J = C(L), C : {0, 1}∗ → {0, · · · , 2^(2κ−1)}
    this->H(tag, &J, 2 * this->k - 1);

    // secret trapdoor B = J ^ d mod n
    mpz_powm(*B, J, sk->d, pk->n);

    // r
    GenerateRandomInN(*r, pk->n);

    // tmp = H(m)
    this->H(m, &tmp, this->t);
    // J^H(m) * r^e mod n
    mpz_powm(tmp, J, tmp, pk->n);
    mpz_powm(tmp_2, *r, pk->e, pk->n);
    mpz_mul(*h, tmp, tmp_2);
    mpz_mod(*h,*h, pk->n);

    mpz_clears(J,tmp,tmp_2,NULL);
}

/**
 * @param pk public key
 * @param m message
 * @param tag tag
 * @param h hash
 * @param r random number
 */
bool CH_KEF_MH_RSA_F_AM_2004::Check(pk *pk, mpz_t *m, mpz_t *tag, mpz_t *h,mpz_t *r){
    mpz_t J,tmp,tmp_2,tmp_3;
    mpz_inits(J,tmp,tmp_2,tmp_3,NULL);
    // J = C(L), C : {0, 1}∗ → {0, · · · , 2^(2κ−1)}
    this->H(tag, &J, 2 * this->k - 1);

    // tmp = H(m)
    this->H(m, &tmp, this->t);
    // J^H(m) * r^e mod n
    mpz_powm(tmp, J, tmp, pk->n);
    mpz_powm(tmp_2, *r, pk->e, pk->n);
    mpz_mul(tmp_3, tmp, tmp_2);
    mpz_mod(tmp_3,tmp_3, pk->n);

    if(mpz_cmp(*h, tmp_3) == 0){
        mpz_clears(J,tmp,tmp_2,tmp_3,NULL);
        return true;
    }else{
        mpz_clears(J,tmp,tmp_2,tmp_3,NULL);
        return false;
    }
}

/**
 * @param pk public key
 * @param m message m
 * @param m_p message m'
 * @param tag tag
 * @param h hash
 * @param B secret trapdoor
 * @param r random number r
 * @param r_p random number r'
 */
void CH_KEF_MH_RSA_F_AM_2004::Adapt(pk *pk, mpz_t *m,  mpz_t *m_p, mpz_t *tag, mpz_t *h, mpz_t *B, mpz_t *r, mpz_t *r_p){
    mpz_t tmp,tmp_2;
    mpz_inits(tmp,tmp_2,NULL);

    // r' = r * B^(H(m) - H(m')) mod n
    this->H(m, &tmp, this->t);
    this->H(m_p, &tmp_2, this->t);
    mpz_sub(tmp, tmp, tmp_2);
    mpz_powm(tmp, *B, tmp, pk->n);
    mpz_mul(*r_p, *r, tmp);
    mpz_mod(*r_p, *r_p, pk->n);

    mpz_clears(tmp,tmp_2,NULL);
}

bool CH_KEF_MH_RSA_F_AM_2004::Verify(pk *pk, mpz_t *m_p, mpz_t *tag, mpz_t *h, mpz_t *r_p){
    return this->Check(pk, m_p, tag, h, r_p);
}

CH_KEF_MH_RSA_F_AM_2004::~CH_KEF_MH_RSA_F_AM_2004(){
    this->rsa->rsa_clear();
}
