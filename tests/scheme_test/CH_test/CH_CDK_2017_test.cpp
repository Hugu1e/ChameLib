#include <scheme/CH/CH_CDK_2017.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    CH_CDK_2017 ch;

    CH_CDK_2017_pk pk;
    CH_CDK_2017_sk sk;
    CH_CDK_2017_h h;
    CH_CDK_2017_r r, r_p;
    mpz_t m, tag, m_p, tag_p;
    mpz_inits(m, tag, m_p, tag_p, NULL);

    test.start("SetUp");
    ch.SetUp();
    test.end("SetUp");

    test.start("KeyGen");
    ch.KeyGen(&pk, &sk, 1024);
    test.end("KeyGen");

    RandomGenerator::RandomN(m, pk.getElement("n"));
    RandomGenerator::RandomN(tag, pk.getElement("n"));
    Logger::PrintGmp("m", m);
    Logger::PrintGmp("tag", tag);
    test.start("Hash");
    ch.Hash(&h, &r, m, tag, &pk);
    test.end("Hash");
    Logger::PrintGmp("h", h.getElement("h"));
    Logger::PrintGmp("r", r.getElement("r"));

    test.start("Check");
    bool check_result = ch.Check(&h, &r, m, tag, &pk);
    test.end("Check");
    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    RandomGenerator::RandomN(m_p, pk.getElement("n"));
    Logger::PrintGmp("m_p", m_p);
    test.start("Adapt");
    ch.Adapt(&r_p, tag_p, &h, &r, m_p, &sk, &pk);
    test.end("Adapt");
    Logger::PrintGmp("r_p", r_p.getElement("r"));
    Logger::PrintGmp("tag_p", tag_p);

    test.start("Verify");
    bool verify_result = ch.Verify(&h, &r_p, m_p, tag_p, &pk);
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

