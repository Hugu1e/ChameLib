#include <scheme/PBCH/PCH_DSS_2019.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    PCH_DSS_2019 ch(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());


    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    const int SIZE_OF_ATTR = attr_list.size();  // S
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;

    int k;
    PCH_DSS_2019_sk skPCH;
    PCH_DSS_2019_pk pkPCH;
    PCH_DSS_2019_sks sksPCH;

    mpz_t m,m_p;
    PCH_DSS_2019_h h;
    PCH_DSS_2019_r r,r_p;

    mpz_inits(m,m_p,NULL);

    k = 512;
    printf("k = %d\n", k);
    RandomGenerator::RandomInLength(m, 128);
    Logger::PrintGmp("m", m);
    RandomGenerator::RandomInLength(m_p, 128);
    Logger::PrintGmp("m_p", m_p);

    
    test.start("SetUp");
    ch.SetUp(pkPCH, skPCH, h, r, r_p, k);
    test.end("SetUp");

    test.start("KeyGen");
    ch.KeyGen(skPCH, pkPCH, attr_list, sksPCH);
    test.end("KeyGen");

    test.start("Hash");
    ch.Hash(pkPCH, m, POLICY, h, r);
    test.end("Hash");
    // h1 h2 N2
    h.getH().print();

    test.start("Check");
    bool check_result = ch.Check(pkPCH, m, h, r);
    test.end("Check");
    if(check_result){
        printf("Check success\n");
    }
    else{
        printf("Check failed\n");
    }
    
    test.start("Adapt");
    ch.Adapt(r_p, m_p, h, r, m, pkPCH, sksPCH, POLICY);
    test.end("Adapt");
    r_p.print();

    test.start("Verify");
    bool verify_result = ch.Verify(pkPCH, m_p, h, r_p);
    test.end("Verify");
    
    if(verify_result){
        printf("Verify success\n");
        test_result = 0;
    }
    else{
        printf("Verify failed\n");
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