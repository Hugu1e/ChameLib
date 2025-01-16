#ifndef CHAMELIB_LOGGER_H
#define CHAMELIB_LOGGER_H

#include <gmp.h>
#include <string>

class Logger{
    public:
        static void PrintMpz(std::string mpz_name, mpz_t *mpz);
        static void PrintMpzsize(std::string mpz_name, mpz_t *mpz);
        static void PrintMpzWithSize(std::string mpz_name, mpz_t *mpz);
};

#endif  // CHAMELIB_LOGGER_H