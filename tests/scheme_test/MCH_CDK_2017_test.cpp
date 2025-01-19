#include <scheme/MCH_CDK_2017.h>

int test_result = 1;

mpz_t n; 
mpz_t e; 
mpz_t d; 

// mpz_t tmp_n; 
// mpz_t tmp_e; 
// mpz_t tmp_d; 

mpz_t h;
mpz_t r;
mpz_t m;
mpz_t tag;
mpz_t r_p;
mpz_t m_p;
mpz_t tag_p;

void test_init(){
    // mpz_init(n);
    // mpz_init(e);
    // mpz_init(d);

    mpz_init(h);
    mpz_init(r);
    mpz_init(m);
    mpz_init(r_p);
    mpz_init(m_p);
}

void test_clear(){
    // mpz_clear(n);
    // mpz_clear(e);
    // mpz_clear(d);

    mpz_clear(h);
    mpz_clear(r);
    mpz_clear(m);
    mpz_clear(r_p);
    mpz_clear(m_p);
}


int main(int argc, char *argv[]){
    printf("Test MCH_CDK_2017\n");
    test_init();
    MCH_CDK_2017 *test = new MCH_CDK_2017(&n,&e,&d);
    
    test->CParGen(&n,&e,&d);

    test->CKGen(&n,&e,&d);
    gmp_printf("e: %Zd\n", e);
    gmp_printf("n: %Zd\n", n);
    gmp_printf("d: %Zd\n", d);

    mpz_set_ui(m, 123456);
    test->CHash(&h,&r,&n,&e,&m);
    // 打印hash值
    gmp_printf("Hash: %Zd\n", h);
    gmp_printf("r: %Zd\n", r);

 
    mpz_set_ui(m_p, 789101);
    test->Adapt(&r_p,&m_p,&m,&r,&h,&n,&e,&d);
    // 打印r_p
    gmp_printf("r_p: %Zd\n", r_p);    

    // Check
    printf("Check\n");
    if(test->CHashCheck(&h,&m_p,&n,&e,&r_p)){
        printf("Hash check successful!\n");
        test_result = 0;
    }else{
        printf("Hash check failed.\n");
    }

    test->MCH_CDK_2017_clear();
    test_clear();
    return test_result;
}