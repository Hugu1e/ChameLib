#include "utils/Logger.h"

/**
 * @brief Print mpz_t
 * 
 * @param[in] mpz_name name of mpz_t
 * @param[in] mpz mpz_t
 */
void Logger::PrintGmp(std::string mpz_name, mpz_t mpz){
    gmp_printf("%s: %Zd\n", mpz_name.c_str(), mpz);
}

/**
 * @brief Print size of mpz_t
 * 
 * @param[in] mpz_name name of mpz_t
 * @param[in] mpz mpz_t
 */
void Logger::PrintGmpsize(std::string mpz_name, mpz_t mpz){
    printf("size of %s = %ld bytes\n", mpz_name.c_str(), (mpz_sizeinbase(mpz, 2) + 7) / 8);
}

/**
 * @brief Print mpz_t with size
 * 
 * @param[in] mpz_name name of mpz_t
 * @param[in] mpz mpz_t
 */
void Logger::PrintGmpWithSize(std::string mpz_name, mpz_t mpz){
    PrintGmp(mpz_name, mpz);
    PrintGmpsize(mpz_name, mpz);
}

/**
 * @brief Print element_t
 * 
 * @param[in] pbc_name name of element_t
 * @param[in] element element_t
 */
void Logger::PrintPbc(std::string pbc_name, element_t element){
    element_printf("%s: %B\n", pbc_name.c_str(), element);
}

/**
 * @brief Print size of element_t
 * 
 * @param[in] pbc_name name of element_t
 * @param[in] element element_t
 */
void Logger::PrintPbcsize(std::string pbc_name, element_t element){
    printf("size of %s = %d bytes\n", pbc_name.c_str(), element_length_in_bytes(element));
}

/**
 * @brief Print element_t with size
 * 
 * @param[in] pbc_name name of element_t
 * @param[in] element element_t
 */
void Logger::PrintPbcWithSize(std::string pbc_name, element_t element){
    PrintPbc(pbc_name, element);
    PrintPbcsize(pbc_name, element);
}

/**
 * @brief Print PbcElements
 * 
 * @param[in] pbcElements_name name of PbcElements
 * @param[in] pbcElements PbcElements
 */
void Logger::PrintPbcElements(std::string pbcElements_name, PbcElements &pbcElements){
    printf("%s:\n", pbcElements_name.c_str());
    pbcElements.print();
}

/**
 * @brief Print GmpElements
 * 
 * @param[in] GmpElements_name name of GmpElements
 * @param[in] gmpElements GmpElements
 */
void PrintGmpElements(std::string GmpElements_name, GmpElements &gmpElements){
    printf("%s:\n", GmpElements_name.c_str());
    gmpElements.print();
}