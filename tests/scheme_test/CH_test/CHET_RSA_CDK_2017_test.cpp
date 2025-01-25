#include <scheme/CH/CHET_RSA_CDK_2017.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    CHET_RSA_CDK_2017 ch;
    CHET_RSA_CDK_2017_pk pk;
    CHET_RSA_CDK_2017_sk sk;
    CHET_RSA_CDK_2017_h h;
    CHET_RSA_CDK_2017_etd etd;

    mpz_t r, r_p;
    mpz_t m, m_p;
    mpz_inits(r, r_p, m, m_p, NULL);

    test.start("SetUp");
    ch.SetUp();
    test.end("SetUp");

    test.start("KeyGen");
    ch.KeyGen(&pk, &sk, 1024);
    test.end("KeyGen");
    pk.printElement("n");
    pk.printElement("e");
    sk.printElement("p");
    sk.printElement("q");

    RandomGenerator::RandomInLength(m, 100);
    Logger::PrintGmp("m", m);
    test.start("Hash");
    ch.Hash(&h, r, &etd, m, &pk);
    test.end("Hash");
    h.printElement("h");
    h.printElement("n_");
    Logger::PrintGmp("r", r);
    etd.printElement("p");
    etd.printElement("q");

    test.start("Check");
    bool check_result = ch.Check(&h, r, m, &pk);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    RandomGenerator::RandomInLength(m_p, 100);
    Logger::PrintGmp("m_p", m_p);
    test.start("Adapt");
    ch.Adapt(r_p, m_p, m, r, &h, &sk, &etd, &pk);
    test.end("Adapt");
    Logger::PrintGmp("r_p", r_p);

    test.start("Verify");
    bool verify_result = ch.Verify(&h, r_p, m_p, &pk);
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

