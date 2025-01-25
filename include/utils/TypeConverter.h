#ifndef CHAMELIB_TYPE_CONVERTER_H
#define CHAMELIB_TYPE_CONVERTER_H

#include <gmp.h>
#include <pbc.h>
#include <stdexcept>

class TypeConverter{
    public:
        static void mpz_from_element(mpz_t mpz, element_t element);
        static void mpz_to_element(element_t element, mpz_t mpz);
};

#endif  //CHAMELIB_TYPE_CONVERTER_H