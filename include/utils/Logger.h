#ifndef CHAMELIB_LOGGER_H
#define CHAMELIB_LOGGER_H

#include <gmp.h>
#include <pbc.h>
#include <string>

class Logger
{
public:
    static void PrintGmp(std::string mpz_name, mpz_t mpz);
    static void PrintGmpsize(std::string mpz_name, mpz_t mpz);
    static void PrintGmpWithSize(std::string mpz_name, mpz_t mpz);

    static void PrintPbc(std::string pbc_name, element_t element);
    static void PrintPbcsize(std::string pbc_name, element_t element);
    static void PrintPbcWithSize(std::string pbc_name, element_t element);
};

#endif // CHAMELIB_LOGGER_H