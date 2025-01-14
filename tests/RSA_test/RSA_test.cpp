#include <AE/RSA.h>
#include <iostream>

int test_result = 1;



void test(){
    AE_RSA rsa;
    AE_RSA::pk pk;
    AE_RSA::sk sk;

    mpz_t m,c,m2;
    mpz_inits(m,c,m2,NULL);

    rsa.KeyGen(&pk, &sk, 1024);
    gmp_printf("Public Key (e, n): (%Zd, %Zd)\n", pk.e, pk.n);
    gmp_printf("Private Key (d): %Zd\n", sk.d);

    mpz_set_ui(m, 123456);
    rsa.Encrypt(&c, &m, &pk);
    gmp_printf("Ciphertext: %Zd\n", c);

    rsa.Decrypt(&m2, &c, &sk, &pk);
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
    test();
    return test_result;
}