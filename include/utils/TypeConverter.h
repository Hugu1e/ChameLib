#ifndef CHAMELIB_TYPE_CONVERTER_H
#define CHAMELIB_TYPE_CONVERTER_H

#include <gmp.h>
#include <pbc.h>
#include <stdexcept>

class TypeConverter{
    public:
        static void mpz_from_element(mpz_t mpz, element_t element);
        static void mpz_to_element(element_t element, mpz_t mpz);
        static void element_g_from_element_zn(element_t element_g, element_t element_zn);
        static void element_zn_from_element_g(element_t element_zn, element_t element_g);
};

#endif  //CHAMELIB_TYPE_CONVERTER_H