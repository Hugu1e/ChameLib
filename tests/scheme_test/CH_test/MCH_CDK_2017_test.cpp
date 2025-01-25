#include <scheme/CH/MCH_CDK_2017.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    MCH_CDK_2017 ch;
    MCH_CDK_2017_pk pk;
    MCH_CDK_2017_sk sk;
    MCH_CDK_2017_h h;
    MCH_CDK_2017_r r,r_p;

    mpz_t m, m_p;
    mpz_inits(m, m_p, NULL);

    test.start("KeyGen");
    ch.KeyGen(&pk, &sk, 1024);
    test.end("KeyGen");
    pk.printElement("n");
    pk.printElement("e");
    sk.printElement("d");

    RandomGenerator::RandomInLength(m, 100);
    Logger::PrintGmp("m", m);
    test.start("Hash");
    ch.Hash(&h, &r, m, &pk);
    test.end("Hash");
    h.printElement("h");
    r.printElement("r");

    test.start("Check");
    bool check_result = ch.Check(&h, &r, m, &pk);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    RandomGenerator::RandomInLength(m_p, 100);
    Logger::PrintGmp("m_p", m_p);
    test.start("Adapt");
    ch.Adapt(&r_p, m_p, m, &r, &h, &sk, &pk);
    test.end("Adapt");
    r_p.printElement("r");

    test.start("Verify");
    bool verify_result = ch.Verify(&h, &r_p, m_p, &pk);
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

