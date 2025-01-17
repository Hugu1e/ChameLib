#include <signature/BLS.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    BLS bls(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());
    BLS_pp pp;
    BLS_pk pk;
    BLS_sk sk;
    BLS_signature signature;
    
    test.start("Setup");
    bls.Setup(&pp);
    test.end("Setup");

    test.start("KeyGen");
    bls.KeyGen(&pk, &sk, &pp);
    test.end("KeyGen");
    pk.printElement();
    sk.printElement();
    pp.printElement();

    std::string message = "Hello, BLS!";
    test.start("Sign");
    bls.Sign(&signature, &sk, message);
    test.end("Sign");
    signature.printElement();

    test.start("Verify");
    bool verify_result = bls.Verify(&pp, &pk, message, &signature);
    test.end("Verify");

    if(verify_result){
        printf("Verify successfully.\n");
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

