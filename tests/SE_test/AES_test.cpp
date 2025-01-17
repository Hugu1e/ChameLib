#include <SE/AES.h>
#include <CommonTest.h>
#include <utils/RandomGenerator.h>
#include <utils/Logger.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    AES aes;
    element_t key;
    mpz_t plaintext;
    mpz_t ciphertext;
    mpz_t decrypted_plaintext;

    element_init_same_as(key, test.get_GT());
    mpz_inits(plaintext, ciphertext, decrypted_plaintext, NULL);

    test.start("KGen");
    aes.KGen(key, 256);
    test.end("KGen");
    Logger::PrintPbc("key", key);

    test.start("Encrypt");
    RandomGenerator::RandomInLength(plaintext, 128);
    Logger::PrintGmp("plaintext", plaintext);
    aes.Enc(ciphertext, key, plaintext);
    test.end("Encrypt");

    test.start("Decrypt");
    aes.Dec(decrypted_plaintext, key, ciphertext);
    Logger::PrintGmp("decrypted_plaintext", decrypted_plaintext);
    test.end("Decrypt");

    if(mpz_cmp(plaintext, decrypted_plaintext) == 0){
        printf("Decrypt successfully.\n");
        test_result = 0;
    }else{
        printf("Decrypt failed.\n");
    }
    
    element_clear(key);
    mpz_clears(plaintext, ciphertext, decrypted_plaintext, NULL);
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

