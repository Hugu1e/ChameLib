#include <AE/PKE_CPA_AMV_2017.h>
#include <CommonTest.h>
#include <utils/TypeConverter.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    PKE_CPA_AMV_2017 pke(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    PKE_CPA_AMV_2017_pk pk;
    PKE_CPA_AMV_2017_sk sk;
    PKE_CPA_AMV_2017_c c;

    element_t m,res;
    
    element_init_same_as(m, test.get_Zn());
    element_init_same_as(res, test.get_Zn());

    element_random(m);

    test.start("KeyGen");
    pke.KeyGen(&pk, &sk);
    test.end("KeyGen");
    Logger::PrintPbcElements("pk", pk);
    Logger::PrintPbcElements("sk", sk);

    Logger::PrintPbc("m", m);
    test.start("Encrypt");
    pke.Encrypt(&c, m, &pk);
    test.end("Encrypt");
    Logger::PrintPbcElements("c", c);

    test.start("Decrypt");
    pke.Decrypt(res, &c, &sk);
    test.end("Decrypt");
    Logger::PrintPbc("res", res);

    if (element_cmp(m, res) == 0) {
        printf("Decryption successful!\n");
        test_result = 0;
    } else {
        printf("Decryption failed.\n");
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

