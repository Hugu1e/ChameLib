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