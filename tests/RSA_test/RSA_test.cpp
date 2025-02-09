#include <AE/RSA.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    AE_RSA rsa;
    RSA_pk pk;
    RSA_sk sk;

    mpz_t m,c,m2;
    mpz_inits(m,c,m2,NULL);

    test.start("SetUp");
    rsa.SetUp(pk, sk);
    test.end("SetUp");

    test.start("KeyGen");
    rsa.KeyGen(pk, sk, 1024);
    test.end("KeyGen");
    pk.print();
    sk.print();

    mpz_set_ui(m, 123456);
    test.start("Encrypt");
    rsa.Encrypt(c, m, pk);
    test.end("Encrypt");
    gmp_printf("Ciphertext: %Zd\n", c);

    test.start("Decrypt");
    rsa.Decrypt(m2, c, sk, pk);
    test.end("Decrypt");
    gmp_printf("Decrypted Plaintext: %Zd\n", m);

    if (mpz_cmp(m, m2) == 0) {
        printf("Decryption successful!\n");
        test_result = 0;
    } else {
        printf("Decryption failed.\n");
    }

    mpz_clears(m,c,m2,NULL);
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

