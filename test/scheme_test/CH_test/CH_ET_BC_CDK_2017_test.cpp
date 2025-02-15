#include <CommonTest.h>
#include "scheme/CH/CH_ET_BC_CDK_2017.h"

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    CH_ET_BC_CDK_2017 ch;
    CH_ET_BC_CDK_2017_pp pp;
    CH_ET_BC_CDK_2017_pk pk;
    CH_ET_BC_CDK_2017_sk sk;
    CH_ET_BC_CDK_2017_etd etd;
    CH_ET_BC_CDK_2017_h h;
    CH_ET_BC_CDK_2017_r r,r_p;

    std::string m = "123456";
    std::string m_p = "789101";

    test.start("SetUp");
    ch.SetUp(pp, pk, sk, h, r, r_p, etd, 1024);
    test.end("SetUp");

    test.start("KeyGen");
    ch.KeyGen(pk, sk, pp);
    test.end("KeyGen");

    test.start("Hash");
    ch.Hash(h, r, etd, pp, pk, m);
    test.end("Hash");
    h.print();

    test.start("Check");
    bool check_result = ch.Check(h, r, pk, m);
    test.end("Check");
    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    test.start("Adapt");
    ch.Adapt(r_p, sk, etd, pk, h, r, m, m_p);
    test.end("Adapt");
    r.print();
    
    test.start("Verify");
    bool verify_result = ch.Verify(h, r_p, pk, m_p);
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

