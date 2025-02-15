#include "utils/TypeConverter.h"

/**
 * parse element_t to mpz_t
 * @param mpz mpz_t
 * @param element element_t
 */
void TypeConverter::mpz_from_element(mpz_t mpz, element_t element){
    unsigned char *str = (unsigned char *)malloc(element_length_in_bytes(element));
    element_to_bytes(str, element);
    mpz_set_str(mpz, (char *)str, 10); 
    free(str);
    if(mpz_cmp_ui(mpz, 0) == 0){
        throw std::runtime_error("mpz_from_element: mpz = 0");
    }
}

/**
 * parse mpz_t to element_t
 * 
 */
void TypeConverter::mpz_to_element(element_t element, mpz_t mpz){
    char *str = mpz_get_str(NULL, 10, mpz);
    element_from_bytes(element, (unsigned char *)str);
    free(str);
}


void TypeConverter::element_g_from_element_zn(element_t element_g, element_t element_zn){
    unsigned char *str = (unsigned char *)malloc(element_length_in_bytes(element_zn));
    element_to_bytes(str, element_zn);
    element_from_bytes_x_only(element_g, (unsigned char *)str); 
    free(str);
}

void TypeConverter::element_zn_from_element_g(element_t element_zn, element_t element_g){
    unsigned char *str = (unsigned char *)malloc(element_length_in_bytes_x_only(element_g));
    element_to_bytes_x_only(str, element_g);
    element_from_bytes(element_zn, (unsigned char *)str); 
    free(str);
}