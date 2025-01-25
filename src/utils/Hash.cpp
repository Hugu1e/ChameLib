#include <utils/Hash.h>

/**
 * @brief Hash(m) -> res
 *
 * @param[out] res result of hash
 * @param[in] m message
 */
void HASH::hash(element_t res, element_t m)
{
    // SHA-256
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    unsigned char bytes1[element_length_in_bytes(m)];
    element_to_bytes(bytes1, m);
    SHA256_Update(&sha256, bytes1, sizeof(bytes1));
    SHA256_Final(hash, &sha256);
    element_from_hash(res, hash, SHA256_DIGEST_LENGTH);
}
/**
 * @brief Hash(m1, m2) -> res
 * 
 * @param[out] res result of hash
 * @param[in] m1 message 1
 * @param[in] m2 message 2
 */
void HASH::hash(element_t res, element_t m1, element_t m2){
    // SHA-256
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    unsigned char bytes1[element_length_in_bytes(m1)];
    element_to_bytes(bytes1, m1);
    SHA256_Update(&sha256, bytes1, sizeof(bytes1));
    unsigned char bytes2[element_length_in_bytes(m2)];
    element_to_bytes(bytes2, m2);
    SHA256_Update(&sha256, bytes2, sizeof(bytes2));
    SHA256_Final(hash, &sha256);
    element_from_hash(res, hash, SHA256_DIGEST_LENGTH);
}

/**
 * @brief Hash(m1, m2, m3) -> res
 * 
 * @param[out] res result of hash
 * @param[in] m1 message 1
 * @param[in] m2 message 2
 * @param[in] m3 message 3
 */
void HASH::hash(element_t res, element_t m1, element_t m2, element_t m3){
    // SHA-256
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    unsigned char bytes1[element_length_in_bytes(m1)];
    element_to_bytes(bytes1, m1);
    SHA256_Update(&sha256, bytes1, sizeof(bytes1));
    unsigned char bytes2[element_length_in_bytes(m2)];
    element_to_bytes(bytes2, m2);
    SHA256_Update(&sha256, bytes2, sizeof(bytes2));
    unsigned char bytes3[element_length_in_bytes(m3)];
    element_to_bytes(bytes3, m3);
    SHA256_Update(&sha256, bytes3, sizeof(bytes3));
    SHA256_Final(hash, &sha256);
    element_from_hash(res, hash, SHA256_DIGEST_LENGTH);
}
/**
 * @brief Hash(m1, m2, m3, m4) -> res
 * 
 * @param[out] res result of hash
 * @param[in] m1 message 1
 * @param[in] m2 message 2
 * @param[in] m3 message 3
 * @param[in] m4 message 4
 */
void HASH::hash(element_t res, element_t m1, element_t m2, element_t m3, element_t m4)
{
    // SHA-256
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    unsigned char bytes1[element_length_in_bytes(m1)];
    element_to_bytes(bytes1, m1);
    SHA256_Update(&sha256, bytes1, sizeof(bytes1));
    unsigned char bytes2[element_length_in_bytes(m2)];
    element_to_bytes(bytes2, m2);
    SHA256_Update(&sha256, bytes2, sizeof(bytes2));
    unsigned char bytes3[element_length_in_bytes(m3)];
    element_to_bytes(bytes3, m3);
    SHA256_Update(&sha256, bytes3, sizeof(bytes3));
    unsigned char bytes4[element_length_in_bytes(m4)];
    element_to_bytes(bytes4, m4);
    SHA256_Update(&sha256, bytes4, sizeof(bytes4));
    SHA256_Final(hash, &sha256);
    element_from_hash(res, hash, SHA256_DIGEST_LENGTH);
}

void HASH::hash(element_t res, element_t m1, element_t m2, element_t m3, element_t m4, element_t m5)
{
    // SHA-256
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    unsigned char bytes1[element_length_in_bytes(m1)];
    element_to_bytes(bytes1, m1);
    SHA256_Update(&sha256, bytes1, sizeof(bytes1));
    unsigned char bytes2[element_length_in_bytes(m2)];
    element_to_bytes(bytes2, m2);
    SHA256_Update(&sha256, bytes2, sizeof(bytes2));
    unsigned char bytes3[element_length_in_bytes(m3)];
    element_to_bytes(bytes3, m3);
    SHA256_Update(&sha256, bytes3, sizeof(bytes3));
    unsigned char bytes4[element_length_in_bytes(m4)];
    element_to_bytes(bytes4, m4);
    SHA256_Update(&sha256, bytes4, sizeof(bytes4));
    unsigned char bytes5[element_length_in_bytes(m5)];
    element_to_bytes(bytes5, m5);
    SHA256_Update(&sha256, bytes5, sizeof(bytes5));
    SHA256_Final(hash, &sha256);
    element_from_hash(res, hash, SHA256_DIGEST_LENGTH);
}

/**
 * @brief Hash(m1,m2,m3,m4,m5,m6,m7)->res
 */
void HASH::hash(element_t res, element_t m1, element_t m2, element_t m3, element_t m4, element_t m5, element_t m6, element_t m7){
    // SHA-256
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    unsigned char bytes1[element_length_in_bytes(m1)];
    element_to_bytes(bytes1, m1);
    SHA256_Update(&sha256, bytes1, sizeof(bytes1));
    unsigned char bytes2[element_length_in_bytes(m2)];
    element_to_bytes(bytes2, m2);
    SHA256_Update(&sha256, bytes2, sizeof(bytes2));
    unsigned char bytes3[element_length_in_bytes(m3)];
    element_to_bytes(bytes3, m3);
    SHA256_Update(&sha256, bytes3, sizeof(bytes3));
    unsigned char bytes4[element_length_in_bytes(m4)];
    element_to_bytes(bytes4, m4);
    SHA256_Update(&sha256, bytes4, sizeof(bytes4));
    unsigned char bytes5[element_length_in_bytes(m5)];
    element_to_bytes(bytes5, m5);
    SHA256_Update(&sha256, bytes5, sizeof(bytes5));
    unsigned char bytes6[element_length_in_bytes(m6)];
    element_to_bytes(bytes6, m6);
    SHA256_Update(&sha256, bytes6, sizeof(bytes6));
    unsigned char bytes7[element_length_in_bytes(m7)];
    element_to_bytes(bytes7, m7);
    SHA256_Update(&sha256, bytes7, sizeof(bytes7));
    SHA256_Final(hash, &sha256);
    element_from_hash(res, hash, SHA256_DIGEST_LENGTH);
}
/**
 * @brief Hash(m) -> res
 *
 * @param[out] res result of hash
 * @param[in] m message
 */
void HASH::hash(element_t res, std::string m) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, m.c_str(), m.length());
    SHA256_Final(hash, &sha256);
    element_from_hash(res, hash, SHA256_DIGEST_LENGTH);
}

/**
 * @brief Hash(m1, m2) -> res mod n
 * 
 * @param[out] res result of hash
 * @param[in] m1 message 1
 * @param[in] m2 message 2
 * @param[in] n modulus
 */
void HASH::hash_n(mpz_t res, mpz_t m1, mpz_t m2, mpz_t n) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    
    size_t m1_size = (mpz_sizeinbase(m1, 2) + 7) / 8;
    unsigned char* bytes1 = new unsigned char[m1_size];
    mpz_export(bytes1, nullptr, 1, 1, 0, 0, m1);
    SHA256_Update(&sha256, bytes1, m1_size); 
    delete[] bytes1;

    size_t m2_size = (mpz_sizeinbase(m2, 2) + 7) / 8;
    unsigned char* bytes2 = new unsigned char[m2_size];
    mpz_export(bytes2, nullptr, 1, 1, 0, 0, m2);
    SHA256_Update(&sha256, bytes2, m2_size);
    delete[] bytes2;

    SHA256_Final(hash, &sha256);

    mpz_import(res, SHA256_DIGEST_LENGTH, 1, 1, 0, 0, hash);

    mpz_mod(res, res, n);
}

void HASH::hash_n(mpz_t res, mpz_t m1, mpz_t m2, mpz_t m3, mpz_t n){
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    size_t m1_size = (mpz_sizeinbase(m1, 2) + 7) / 8;
    unsigned char* bytes1 = new unsigned char[m1_size];
    mpz_export(bytes1, nullptr, 1, 1, 0, 0, m1);
    SHA256_Update(&sha256, bytes1, m1_size);
    delete[] bytes1;

    size_t m2_size = (mpz_sizeinbase(m2, 2) + 7) / 8;
    unsigned char* bytes2 = new unsigned char[m2_size];
    mpz_export(bytes2, nullptr, 1, 1, 0, 0, m2);
    SHA256_Update(&sha256, bytes2, m2_size);
    delete[] bytes2;

    size_t m3_size = (mpz_sizeinbase(m3, 2) + 7) / 8;
    unsigned char* bytes3 = new unsigned char[m3_size];
    mpz_export(bytes3, nullptr, 1, 1, 0, 0, m3);
    SHA256_Update(&sha256, bytes3, m3_size);
    delete[] bytes3;

    SHA256_Final(hash, &sha256);
    mpz_import(res, SHA256_DIGEST_LENGTH, 1, 1, 0, 0, hash);
    mpz_mod(res, res, n);
}

/**
 * @brief Hash(m1) -> res mod n
 * 
 * @param[out] res result of hash
 * @param[in] m message
 * @param[in] n modulus
 */
void HASH::hash_n(mpz_t res, std::string m, mpz_t n){
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, m.c_str(), m.length());
    SHA256_Final(hash, &sha256);
    mpz_import(res, SHA256_DIGEST_LENGTH, 1, 1, 0, 0, hash);
    mpz_mod(res, res, n);
}

void HASH::hash_n(mpz_t res, mpz_t m, mpz_t n){
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    size_t m_size = (mpz_sizeinbase(m, 2) + 7) / 8;
    unsigned char* bytes = new unsigned char[m_size];
    mpz_export(bytes, nullptr, 1, 1, 0, 0, m);
    SHA256_Update(&sha256, bytes, m_size);
    delete[] bytes;
    SHA256_Final(hash, &sha256);
    mpz_import(res, SHA256_DIGEST_LENGTH, 1, 1, 0, 0, hash);
    mpz_mod(res, res, n);
}