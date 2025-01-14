#include <AE/RSA.h>
#include <iostream>

int test_result = 1;

AE_RSA rsa;


int main(int argc, char *argv[]){
    std::cout<<"RSA test"<<std::endl;
    // MyRSA *test = new MyRSA(&n,&e,&d);

    // // 生成密钥对（位长度为 1024 位）
    // test->rsa_generate_keys(1024);

    // // 显示密钥
    // gmp_printf("Public Key (e, n): (%Zd, %Zd)\n", e, n);
    // gmp_printf("Private Key (d, n): (%Zd, %Zd)\n", d, n);

    // // 明文加密和解密示例
    // mpz_t plaintext, ciphertext, decrypted;
    // mpz_inits(plaintext, ciphertext, decrypted, NULL);

    // // 设置一个明文
    // mpz_set_ui(plaintext, 123456);

    // // 加密
    // test->rsa_encrypt(&ciphertext, &plaintext);
    // gmp_printf("Ciphertext: %Zd\n", ciphertext);

    // // 解密
    // test->rsa_decrypt(&decrypted, &ciphertext);
    // gmp_printf("Decrypted Plaintext: %Zd\n", decrypted);

    // // 检查解密是否正确
    // if (mpz_cmp(plaintext, decrypted) == 0) {
    //     printf("Decryption successful!\n");
    //     test_result = 0;
    // } else {
    //     printf("Decryption failed.\n");
    // }

    // // 清理
    // mpz_clears(plaintext, ciphertext, decrypted, NULL);
    // test->rsa_clear();

    // return test_result;
}