#include <scheme/PBCH/PCHBA_TLL_2020.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    PCHBA_TLL_2020 ch(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    const int SIZE_OF_ATTR = attr_list.size();  // S, S是Policy所有属性的子集
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;   // Policy的属性个数（不去重）

    const int K = 10;
    const int I = 5;  // modifier
    const int J = 5;  // owner
    PCHBA_TLL_2020_sk skPCHBA;
    PCHBA_TLL_2020_pk pkPCHBA;
    PCHBA_TLL_2020_ID ID;
    PCHBA_TLL_2020_sks sksPCHBA;
    PCHBA_TLL_2020_h h, h_p;

    element_t m,m_p;

    ID.get_IDABET().init(K);
    for(int i = 1;i<=K;i++){
        element_t tmp_Zn;
        element_init_same_as(tmp_Zn, test.get_Zn());
        element_random(tmp_Zn);

        ID.get_IDABET().set(i-1, tmp_Zn);
        element_clear(tmp_Zn);
    }
    ID.get_IDABET().print();

    element_init_same_as(m, test.get_Zn());
    element_init_same_as(m_p, test.get_Zn());
    element_random(m);
    element_random(m_p);

    test.start("SetUp");
    ch.SetUp(pkPCHBA, skPCHBA, sksPCHBA, h, h_p, K);
    test.end("SetUp");
    
    test.start("KeyGen");
    ch.KeyGen(sksPCHBA, pkPCHBA, skPCHBA, attr_list, ID, I);
    test.end("KeyGen");
    
    test.start("Hash");
    ch.Hash(h, m, pkPCHBA, skPCHBA, POLICY, ID, J);
    test.end("Hash");
    
    test.start("Check");
    bool check_result = ch.Check(h, m, pkPCHBA);
    test.end("Check");

    if(check_result){
        printf("Check success\n");
    }else{
        printf("Check failed\n");
    }
    
    test.start("Adapt");
    ch.Adapt(h_p, m_p, h, m, POLICY, ID, I, pkPCHBA, skPCHBA, sksPCHBA);
    test.end("Adapt");

    test.start("Verify");
    bool verify_result = ch.Verify(h_p, m_p, pkPCHBA);
    test.end("Verify");

    if(verify_result){
        printf("Verify success\n");
        test_result = 0;
    }else{
        printf("Verify failed\n");
    }
    
    test.start("Judge");
    bool judgeRes = ch.Judge(m, h, m_p, h_p, ID, I, pkPCHBA, skPCHBA);
    test.end("Judge");
    

    if(judgeRes){
        printf("Judge success\n");
        test_result  = 0;
    }else{
        printf("Judge failed\n");
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