#include <scheme/PBCH/RPCH_TMM_2022.h>
#include <CommonTest.h>
#include "utils/TimeUtils.h"

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    RPCH_TMM_2022 ch(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    const int N = 8;  // a binary tree with N leaf nodes
    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    const int SIZE_OF_ATTR = attr_list.size();  // S
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;
    const time_t T = TimeUtils::TimeCast(2024, 12, 21, 0, 0, 0);  // present time


    int k;
    RPCH_TMM_2022_sk skRPCH;
    RPCH_TMM_2022_pk pkRPCH;
    RPCH_TMM_2022_skid skidRPCH;
    RPCH_TMM_2022_dkidt dkidtRPCH;
    RPCH_TMM_2022_kut kut;

    RPCH_TMM_2022_h h,h_p;

    RPCH_TMM_2022_RevokedPresonList rl;
    RPCH_TMM_2022_Binary_tree st;

    element_t m,m_p;
    element_t id;

    element_init_same_as(id, test.get_Zn());
    element_init_same_as(m, test.get_Zn());
    element_init_same_as(m_p, test.get_Zn());
    element_random(m);
    element_random(m_p);
    element_random(id);


    k = 512;
    printf("k = %d\n", k);
    
    test.start("SetUp");
    ch.SetUp(skRPCH, pkRPCH, rl, st, k, N);
    test.end("SetUp");
    
    test.start("KeyGen");
    ch.KeyGen(skidRPCH, pkRPCH, skRPCH, st, id, attr_list);
    test.end("KeyGen");
    
    test.start("KUpt");
    ch.KUpt(kut, pkRPCH, st, rl, T);
    test.end("KUpt");
    
    test.start("DKGen");
    ch.DKGen(dkidtRPCH, pkRPCH, skidRPCH, kut);
    test.end("DKGen");
    
    test.start("Rev");
    ch.Rev(rl, id, T);
    test.end("Rev");

    test.start("Hash");
    ch.Hash(h, m, pkRPCH, POLICY, T);
    test.end("Hash");
    
    test.start("Check");
    bool check = ch.Check(pkRPCH, m, h);
    test.end("Check");
            
    if(check){
        printf("Check success\n");
    }
    else{
        printf("Check failed\n");
    }
        
        
    test.start("Adapt");
    ch.Adapt(h_p, m_p, m, h, pkRPCH, dkidtRPCH);
    test.end("Adapt");
    
    test.start("Verify");
    bool verify = ch.Verify(pkRPCH, m_p, h_p);
    test.end("Verify");

    if(verify){
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