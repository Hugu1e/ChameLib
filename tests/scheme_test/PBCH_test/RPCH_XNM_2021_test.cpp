#include <scheme/PBCH/RPCH_XNM_2021.h>
#include <CommonTest.h>
#include <utils/TimeUtils.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    RPCH_XNM_2021 ch(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    const int N = 8;  // a binary tree with N leaf nodes
    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    const int SIZE_OF_ATTR = attr_list.size();  // S
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;
    const time_t T = TimeUtils::TimeCast(2024, 12, 21, 0, 0, 0);  // present time

    const time_t end_time_1 = TimeUtils::TimeCast(2025, 12, 31, 0, 0, 0);


    int k;
    RPCH_XNM_2021_sk skRPCH;
    RPCH_XNM_2021_pk pkRPCH;
    RPCH_XNM_2021_skid skidRPCH;
    RPCH_XNM_2021_dkidt dkidtRPCH;
    RPCH_XNM_2022_kut kut;

    RPCH_XNM_2021_h h,h_p;

    RPCH_XNM_2021_RevokedPresonList rl;
    RPCH_XNM_2022_Binary_tree st;

    element_t id;
    element_init_same_as(id, test.get_Zn());
    element_random(id);
    mpz_t m,m_p;
    mpz_inits(m,m_p,NULL);
    RandomGenerator::RandomInLength(m, 128);
    RandomGenerator::RandomInLength(m_p, 128);
        
    k = 512;
    printf("k = %d\n", k);
    
    test.start("SetUp");
    ch.SetUp(skRPCH, pkRPCH, rl, st, k, N);
    test.end("SetUp");

    test.start("KeyGen");
    ch.KeyGen(skidRPCH, pkRPCH, skRPCH, st, attr_list, id, end_time_1);
    test.end("KeyGen");

    test.start("KUpt");
    ch.KUpt(kut, pkRPCH, st, rl, T);
    test.end("KUpt");
    printf("size of kut.ku_theta: %ld\n", kut.get_kut().get_ku_theta().size());

    test.start("DKGen");
    ch.DKGen(dkidtRPCH, pkRPCH, skidRPCH, kut);
    test.end("DKGen");

    test.start("Rev");
    ch.Rev(rl, id, T);
    test.end("Rev");

    Logger::PrintGmp("m", m);
    test.start("Hash");
    ch.Hash(h, m, pkRPCH, POLICY, T);
    test.end("Hash");
    // h1 h2 r1 r2 N2 cSE
    h.get_h().print();
    h.get_r().print();

    test.start("Check");
    bool check = ch.Check(pkRPCH, m, h);
    test.end("Check");

    if(check){
        printf("Check success\n");
    }else{
        printf("Check failed\n");
    }

    Logger::PrintGmp("m_p", m_p);
    test.start("Adapt");
    ch.Adapt(h_p, m_p, m, h, pkRPCH, dkidtRPCH);
    test.end("Adapt");
    // h1 h2 r1 r2 N2 cSE
    h_p.get_h().print();
    h_p.get_r().print();


    test.start("Verify");
    bool verify = ch.Verify(pkRPCH, m_p, h_p);
    test.end("Verify");

    if(verify){
        printf("Verify success\n");
        test_result = 0;
    }else{
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