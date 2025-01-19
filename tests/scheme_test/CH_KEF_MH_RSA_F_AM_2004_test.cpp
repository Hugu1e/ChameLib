#include <scheme/CH_KEF_MH_RSA_F_AM_2004.h>

int test_result = 1;

CH_KEF_MH_RSA_F_AM_2004::pk pk;
CH_KEF_MH_RSA_F_AM_2004::sk sk;

mpz_t h;
mpz_t B;
mpz_t r;
mpz_t m;
mpz_t label;
mpz_t r_p;
mpz_t m_p;

void test_init(){
    pk.Init();
    sk.Init();

    mpz_inits(h,B,r,m,label,r_p,m_p,NULL);
}

void test_clear(){
    mpz_clears(h,B,r,m,label,r_p,m_p,NULL);
}


int main(int argc, char *argv[]){
    test_init();
    CH_KEF_MH_RSA_F_AM_2004 *test = new CH_KEF_MH_RSA_F_AM_2004(&pk, &sk);

    test->KGen(1024, 500, &pk, &sk);
    // PrintMpzAndSize("pk.n", pk.n);
    // PrintMpzAndSize("pk.e", pk.e);
    // PrintMpzAndSize("sk.d", sk.d);

    mpz_set_ui(m, 42525346346746);
    mpz_set_ui(label, 424253532414);
    test->Hash(&pk, &sk, &m, &label, &h, &B, &r);
    PrintMpzAndSize("h", h);
    PrintMpzAndSize("B", B);
    PrintMpzAndSize("r", r);

    if(test->Check(&pk, &m, &label, &h, &r)){
        printf("Hash check successful!\n");}
    else{
        printf("Hash check failed.\n");
    }

    mpz_set_ui(m_p, 96725346346246);
    test->Adapt(&pk, &m, &m_p, &label, &h, &B, &r, &r_p);
    PrintMpzAndSize("r_p", r_p);


    // Verify
    if(test->Verify(&pk, &m_p, &label, &h, &r_p)){
        printf("Verify successful!\n");
        test_result = 0;
    }else{
        printf("Verify failed.\n");
    }


    test_clear();
    return test_result;
}