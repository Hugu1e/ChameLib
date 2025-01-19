#include <scheme/CHET_RSA_CDK_2017.h>

int test_result = 1;

mpz_t n;  // public key
mpz_t e; 
mpz_t d; 

mpz_t etd_n;  // trapdoor
mpz_t etd_p;
mpz_t etd_q;

mpz_t h;
mpz_t r;
mpz_t m;
mpz_t r_p;
mpz_t m_p;

void test_init(){
    // mpz_init(n);
    // mpz_init(e);
    // mpz_init(d);

    mpz_init(etd_n);
    mpz_init(etd_p);
    mpz_init(etd_q);

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

    mpz_clear(etd_n);
    mpz_clear(etd_p);
    mpz_clear(etd_q);

    mpz_clear(h);
    mpz_clear(r);
    mpz_clear(m);
    mpz_clear(r_p);
    mpz_clear(m_p);
}


int main(int argc, char *argv[]){
    printf("Test CHET_RSA_CDK_2017\n");
    test_init();
    CHET_RSA_CDK_2017 *test = new CHET_RSA_CDK_2017(&n,&e,&d);
    
    test->CParGen(&n,&e,&d);

    test->CKGen(&n,&e,&d);
    gmp_printf("e: %Zd\n", e);
    gmp_printf("n: %Zd\n", n);
   

    mpz_set_ui(m, 123456);
    test->CHash(&h,&etd_n,&r,&etd_p,&etd_q,&n,&e,&m);
    // 打印hash值
    gmp_printf("Hash: %Zd\n", h);
    gmp_printf("r: %Zd\n", r);
    gmp_printf("etd_p: %Zd\n", etd_p);
    gmp_printf("etd_q: %Zd\n", etd_q);
    gmp_printf("etd_n: %Zd\n", etd_n);

    // 输出etd_p，etd_q，hash的大小(bytes)
    size_t bits = mpz_sizeinbase(etd_p, 2);
    size_t bytes = (bits + 7) / 8;
    printf("sizeof(etd_p): %zu bytes\n", bytes);
    bits = mpz_sizeinbase(etd_q, 2);
    bytes = (bits + 7) / 8;
    printf("sizeof(etd_q): %zu bytes\n", bytes);
    bits = mpz_sizeinbase(h, 2);
    bytes = (bits + 7) / 8;
    printf("sizeof(h): %zu bytes\n", bytes);

 
    mpz_set_ui(m_p, 789101);
    if(test->Adapt(&r_p,&m_p,&m,&r,&h,&n,&etd_n,&etd_p,&etd_q,&e)){
        printf("Adapt successful!\n");
        // 打印r_p
        gmp_printf("r_p: %Zd\n", r_p);  
        // Check
        printf("Check\n");
        if(test->CHashCheck(&h,&m_p,&n,&etd_n,&e,&r_p)){
            printf("Hash check successful!\n");
            test_result = 0;
        }else{
            printf("Hash check failed.\n");
        }
    }else{
        printf("Adapt failed.\n");
    }
      



    test->CHET_RSA_CDK_2017_clear();
    test_clear();
    return test_result;
}