#include <scheme/CH_CDK_2017.h>

int test_result = 1;

mpz_t n; 
mpz_t e; 
mpz_t d; 

mpz_t h;
mpz_t r;
mpz_t m;
mpz_t tag;
mpz_t r_p;
mpz_t m_p;
mpz_t tag_p;

void test_init(){
    mpz_init(h);
    mpz_init(r);
    mpz_init(m);
    mpz_init(tag);
    mpz_init(r_p);
    mpz_init(m_p);
    mpz_init(tag_p);
}

void test_clear(){
    mpz_clear(h);
    mpz_clear(r);
    mpz_clear(m);
    mpz_clear(tag);
    mpz_clear(r_p);
    mpz_clear(m_p);
    mpz_clear(tag_p);
}


int main(int argc, char *argv[]){
    test_init();
    CH_CDK_2017 *test = new CH_CDK_2017(&n,&e,&d);

    test->CParGen();

    test->CKGen(&n,&e,&d);

    mpz_set_ui(m, 123456);
    mpz_set_ui(tag, 111111);
    test->CHash(&h,&r,&n,&e,&m,&tag);

    // 打印hash值
    gmp_printf("Hash: %Zd\n", h);
    gmp_printf("r: %Zd\n", r);

    // if(test->CHashCheck(&h,&m,&tag,&n,&e,&r)){
    //     printf("Hash check successful!\n");
    // }else{
    //     printf("Hash check failed.\n");
    // }

    mpz_set_ui(m_p, 789101);
    mpz_set_ui(tag_p, 222222);
    test->Adapt(&r_p,&m_p,&tag_p,&m,&tag,&r,&h,&n,&e,&d);

    // 打印r_p
    gmp_printf("r_p: %Zd\n", r_p);    

    // Check
    printf("Check\n");
    if(test->CHashCheck(&h,&m_p,&tag_p,&n,&e,&r_p)){
        printf("Hash check successful!\n");
        test_result = 0;
    }else{
        printf("Hash check failed.\n");
    }

    test->CH_CDK_2017_clear();
    test_clear();
    return test_result;
}