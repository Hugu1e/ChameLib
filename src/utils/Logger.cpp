#include <utils/Logger.h>

/**
 * @brief Print mpz_t
 * 
 * @param mpz_name name of mpz_t
 * @param mpz mpz_t
 */
void Logger::PrintMpz(std::string mpz_name, mpz_t *mpz){
    gmp_printf("%s: %Zd\n", mpz_name.c_str(), *mpz);
}

/**
 * @brief Print size of mpz_t
 * 
 * @param mpz_name name of mpz_t
 * @param mpz mpz_t
 */
void Logger::PrintMpzsize(std::string mpz_name, mpz_t *mpz){
    printf("size of %s = %ld bytes\n", mpz_name.c_str(), (mpz_sizeinbase(*mpz, 2) + 7) / 8);
}

/**
 * @brief Print mpz_t with size
 * 
 * @param mpz_name name of mpz_t
 * @param mpz mpz_t
 */
void Logger::PrintMpzWithSize(std::string mpz_name, mpz_t *mpz){
    PrintMpz(mpz_name, mpz);
    PrintMpzsize(mpz_name, mpz);
}