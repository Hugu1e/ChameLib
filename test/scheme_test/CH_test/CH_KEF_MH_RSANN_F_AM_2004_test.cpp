#include <scheme/CH/CH_KEF_MH_RSANN_F_AM_2004.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    CH_KEF_MH_RSANN_F_AM_2004 ch;

    CH_KEF_MH_RSANN_F_AM_2004_pk pk;
    CH_KEF_MH_RSANN_F_AM_2004_sk sk;
    CH_KEF_MH_RSANN_F_AM_2004_h h;
    CH_KEF_MH_RSANN_F_AM_2004_r r, r_p;
    
    mpz_t m,m_p;
    mpz_t label;
    
    mpz_inits(m, m_p, label, NULL);
    RandomGenerator::RandomInLength(m, 64);
    RandomGenerator::RandomInLength(m_p, 64);
    RandomGenerator::RandomInLength(label, 64);
    
    Logger::PrintGmp("m", m);
    Logger::PrintGmp("m_p", m_p);
    Logger::PrintGmp("label", label);

    test.start("SetUp");
    ch.SetUp(pk, sk, h, r, r_p);
    test.end("SetUp");

    test.start("KeyGen");
    ch.KeyGen(pk, sk, 512);
    test.end("KeyGen");

    
    test.start("Hash");
    ch.Hash(h, r, m, label, pk);
    test.end("Hash");
    h.print();
    r.print();
    

    test.start("Check");
    bool check_result = ch.Check(h, m, r, label, pk);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    mpz_set_ui(m_p, 96725346346246);
    test.start("Adapt");
    ch.Adapt(r_p, m_p, h, m, r, label, sk, pk);
    test.end("Adapt");
    r_p.print();

    test.start("Verify");
    bool verify_result = ch.Verify(h, m_p, r_p, label, pk);
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

