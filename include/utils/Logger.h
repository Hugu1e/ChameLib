#ifndef CHAMELIB_LOGGER_H
#define CHAMELIB_LOGGER_H

#include <pbc/pbc.h>
#include <string>
#include "../base/PbcElements.h"
#include "../base/GmpElements.h"

class Logger
{
public:
    static void PrintGmp(std::string mpz_name, mpz_t mpz);
    static void PrintGmpsize(std::string mpz_name, mpz_t mpz);
    static void PrintGmpWithSize(std::string mpz_name, mpz_t mpz);

    static void PrintPbc(std::string pbc_name, element_t element);
    static void PrintPbcsize(std::string pbc_name, element_t element);
    static void PrintPbcWithSize(std::string pbc_name, element_t element);

    static void PrintPbcElements(std::string pbcElements_name, PbcElements &pbcElements);
    static void PrintGmpElements(std::string GmpElements_name, GmpElements &gmpElements);
};

#endif // CHAMELIB_LOGGER_H