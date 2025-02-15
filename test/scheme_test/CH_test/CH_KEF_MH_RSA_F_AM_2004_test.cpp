#include <scheme/CH/CH_KEF_MH_RSA_F_AM_2004.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    CH_KEF_MH_RSA_F_AM_2004 ch;

    CH_KEF_MH_RSA_F_AM_2004_pk pk;
    CH_KEF_MH_RSA_F_AM_2004_sk sk;
    mpz_t h;
    mpz_t B;
    mpz_t m,m_p;
    mpz_t label;
    mpz_t r, r_p;
    mpz_inits(h, B, r, m, label, r_p, m_p, NULL);

    test.start("SetUp");
    ch.SetUp(pk, sk);
    test.end("SetUp");

    test.start("KeyGen");
    ch.KeyGen(pk, sk, 1024, 500);
    test.end("KeyGen");

    
    mpz_set_ui(m, 42525346346746);
    mpz_set_ui(label, 424253532414);
    test.start("Hash");
    ch.Hash(h, r, B, pk, sk, m, label);
    test.end("Hash");
    Logger::PrintGmp("h", h);
    Logger::PrintGmp("r", r);
    Logger::PrintGmp("B", B);
    

    test.start("Check");
    bool check_result = ch.Check(pk, m, label, h, r);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    mpz_set_ui(m_p, 96725346346246);
    test.start("Adapt");
    ch.Adapt(r_p, pk, m, m_p, label, h, B, r);
    test.end("Adapt");
    Logger::PrintGmp("r_p", r_p);

    test.start("Verify");
    bool verify_result = ch.Verify(pk, m_p, label, h, r_p);
    test.end("Verify");

    if(verify_result){
        printf("Verify successful!\n");
        test_result = 0;
    }else{
        printf("Verify failed.\n");
    }
}


int main(int argc, char *argv[]){
    if(argc == 1) {
        test(argv[0], "a");
    }else if(argc == 2){
        test(argv[0], argv[1]);
    }else{
        printf("usage: %s [a|e|i|f|d224]\n", argv[0]);
        return 1;
    }
    return test_result;
}

