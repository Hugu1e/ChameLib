#include <scheme/CH/FCR_CH_PreQA_DKS_2020.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);
    
    FCR_CH_PreQA_DKS_2020 ch(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    FCR_CH_PreQA_DKS_2020_pp pp;
    FCR_CH_PreQA_DKS_2020_pk pk;
    FCR_CH_PreQA_DKS_2020_sk sk;
    FCR_CH_PreQA_DKS_2020_h h;
    FCR_CH_PreQA_DKS_2020_r r, r_p;
    element_t m,m_p;  // message

    element_init_same_as(m, test.get_Zn());  
    element_init_same_as(m_p, test.get_Zn()); 
    
    element_random(m);
    element_random(m_p);

    test.start("SetUp");
    ch.SetUp(&pp);
    test.end("SetUp");

    test.start("KeyGen");
    ch.KeyGen(&pk, &sk, &pp);
    test.end("KeyGen");

    Logger::PrintPbc("m", m);
    test.start("Hash");
    ch.Hash(&h, &r, m, &pk, &pp);
    test.end("Hash");
    h.printElement("h");
    r.printElement();

    test.start("Check");
    bool check_result = ch.Check(&h, &r, m, &pk, &pp);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    Logger::PrintPbc("m_p", m_p);
    test.start("Adapt");
    ch.Adapt(&r_p, m_p, m, &h, &r, &sk, &pp);
    test.end("Adapt");
    r_p.printElement();

    test.start("Verify");
    bool verify_result = ch.Verify(&h, &r_p, m_p, &pk, &pp);
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

