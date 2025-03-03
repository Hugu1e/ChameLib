#include "SE/AES.h"
#include <openssl/rand.h>

AES::AES() {}

/**
 * @brief AES KGen
 *
 * @param key
 * @param k
 */
void AES::KGen(element_t key, int k)
{
    // TODO
    element_random(key);
}

/**
 * @brief AES KGen
 * 
 * @param key 
 */
void AES::KGen(element_t key)
{
    element_random(key);
}

/**
 * @brief AES KeyGen
 * 
 * @param key 
 * @param k bit length of key(128, 192, 256)
 */
void AES::KGen(unsigned char *key, int k){
    if (k == 128 || k == 192 || k == 256) {
        RAND_bytes(key, k / 8);
    }
}

void AES::Enc(mpz_t ciphertext, unsigned char *key, mpz_t plaintext, int k){
    size_t plaintext_size = (mpz_sizeinbase(plaintext, 2) + 7) / 8;
    unsigned char *plaintext_bytes = new unsigned char[plaintext_size];
    memset(plaintext_bytes, 0, sizeof(plaintext_bytes));
    mpz_export(plaintext_bytes, nullptr, 1, sizeof(plaintext_bytes[0]), 0, 0, plaintext);

    unsigned char ciphertext_bytes[256];
    int ciphertext_len;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (k == 128)
        EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, NULL); //  AES-128-CBC mode
    else if (k == 192)
        EVP_EncryptInit_ex(ctx, EVP_aes_192_cbc(), NULL, key, NULL); //  AES-192-CBC mode
    else if (k == 256)
        EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, NULL); //  AES-256-CBC mode
    ciphertext_len = 0;
    int len;
    // encrypt
    EVP_EncryptUpdate(ctx, ciphertext_bytes, &len, plaintext_bytes, plaintext_size);
    ciphertext_len += len;
    EVP_EncryptFinal_ex(ctx, ciphertext_bytes + ciphertext_len, &len);
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);

    // ciphertext_bytes -> ciphertext
    mpz_import(ciphertext, ciphertext_len, 1, sizeof(ciphertext_bytes[0]), 0, 0, ciphertext_bytes);
}

void AES::Dec(mpz_t decrypted_plaintext, unsigned char *key, mpz_t ciphertext, int k){
    unsigned char ciphertext_bytes[256];
    memset(ciphertext_bytes, 0, sizeof(ciphertext_bytes));
    size_t ciphertext_size;
    mpz_export(ciphertext_bytes, &ciphertext_size, 1, sizeof(ciphertext_bytes[0]), 0, 0, ciphertext);

    unsigned char decrypted_bytes[256];
    int decrypted_len;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (k == 128)
        EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, NULL);
    else if (k == 192)
        EVP_DecryptInit_ex(ctx, EVP_aes_192_cbc(), NULL, key, NULL);
    else if (k == 256)
        EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, NULL);
    decrypted_len = 0;
    int len;
    // decrypt
    EVP_DecryptUpdate(ctx, decrypted_bytes, &len, ciphertext_bytes, ciphertext_size);
    decrypted_len += len;
    EVP_DecryptFinal_ex(ctx, decrypted_bytes + decrypted_len, &len);
    decrypted_len += len;
    decrypted_bytes[decrypted_len] = '\0'; // add terminator
    EVP_CIPHER_CTX_free(ctx);

    mpz_import(decrypted_plaintext, decrypted_len, 1, sizeof(decrypted_bytes[0]), 0, 0, decrypted_bytes);
}

/**
 * @brief AES Enc
 * 
 * @param ciphertext
 * @param key
 * @param plaintext
 * @param k AES key length(128, 192, 256)
 */
void AES::Enc(mpz_t ciphertext, element_t key, mpz_t plaintext, int k)
{
    unsigned char aes_key[element_length_in_bytes(key)];
    element_to_bytes(aes_key, key);

    size_t plaintext_size = (mpz_sizeinbase(plaintext, 2) + 7) / 8;
    unsigned char *plaintext_bytes = new unsigned char[plaintext_size];
    memset(plaintext_bytes, 0, sizeof(plaintext_bytes));
    mpz_export(plaintext_bytes, nullptr, 1, sizeof(plaintext_bytes[0]), 0, 0, plaintext);

    unsigned char ciphertext_bytes[256];
    int ciphertext_len;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (k == 128)
        EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, aes_key, NULL); //  AES-128-CBC mode
    else if (k == 192)
        EVP_EncryptInit_ex(ctx, EVP_aes_192_cbc(), NULL, aes_key, NULL); //  AES-192-CBC mode
    else if (k == 256)
        EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, aes_key, NULL); //  AES-256-CBC mode
    ciphertext_len = 0;
    int len;
    // encrypt
    EVP_EncryptUpdate(ctx, ciphertext_bytes, &len, plaintext_bytes, plaintext_size);
    ciphertext_len += len;
    EVP_EncryptFinal_ex(ctx, ciphertext_bytes + ciphertext_len, &len);
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);

    // ciphertext_bytes -> ciphertext
    mpz_import(ciphertext, ciphertext_len, 1, sizeof(ciphertext_bytes[0]), 0, 0, ciphertext_bytes);
}

/**
 * @brief AES Dec
 * 
 * @param decrypted_plaintext
 * @param key
 * @param ciphertext
 * @param k AES key length(128, 192, 256)
 */
void AES::Dec(mpz_t decrypted_plaintext, element_t key, mpz_t ciphertext, int k)
{
    unsigned char aes_key[element_length_in_bytes(key)];
    element_to_bytes(aes_key, key);

    unsigned char ciphertext_bytes[256];
    memset(ciphertext_bytes, 0, sizeof(ciphertext_bytes));
    size_t ciphertext_size;
    mpz_export(ciphertext_bytes, &ciphertext_size, 1, sizeof(ciphertext_bytes[0]), 0, 0, ciphertext);

    unsigned char decrypted_bytes[256];
    int decrypted_len;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (k == 128)
        EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, aes_key, NULL);
    else if (k == 192)
        EVP_DecryptInit_ex(ctx, EVP_aes_192_cbc(), NULL, aes_key, NULL);
    else if (k == 256)
        EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, aes_key, NULL);
    decrypted_len = 0;
    int len;
    // decrypt
    EVP_DecryptUpdate(ctx, decrypted_bytes, &len, ciphertext_bytes, ciphertext_size);
    decrypted_len += len;
    EVP_DecryptFinal_ex(ctx, decrypted_bytes + decrypted_len, &len);
    decrypted_len += len;
    decrypted_bytes[decrypted_len] = '\0'; // add terminator
    EVP_CIPHER_CTX_free(ctx);

    mpz_import(decrypted_plaintext, decrypted_len, 1, sizeof(decrypted_bytes[0]), 0, 0, decrypted_bytes);
}

AES::~AES()
{
}