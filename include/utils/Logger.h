#ifndef CHAMELIB_LOGGER_H
#define CHAMELIB_LOGGER_H

#include <gmp.h>
#include <pbc.h>
#include <string>

class Logger
{
public:
    static void PrintMpz(std::string mpz_name, mpz_t mpz);
    static void PrintMpzsize(std::string mpz_name, mpz_t mpz);
    static void PrintMpzWithSize(std::string mpz_name, mpz_t mpz);

    static void PrintPbc(std::string pbc_name, element_t element);
    static void PrintPbcsize(std::string pbc_name, element_t element);
    static void PrintPbcWithSize(std::string pbc_name, element_t element);
};

#endif // CHAMELIB_LOGGER_H