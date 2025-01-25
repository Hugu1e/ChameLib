#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    test.start("SetUp");
    ch.SetUp(
    test.end("SetUp");

    test.start("KeyGen");
    ch.KeyGen(
    test.end("KeyGen");

    test.start("Hash");
    ch.Hash(
    test.end("Hash");

    test.start("Check");
    bool check_result = ch.Check(
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    test.start("Adapt");
    ch.Adapt(
    test.end("Adapt");

    test.start("Verify");
    bool verify_result = ch.Verify(
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

