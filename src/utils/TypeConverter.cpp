#include "utils/TypeConverter.h"
#include <string.h>

/**
 * @brief parse element_t to mpz_t
 * @param[out] mpz mpz_t
 * @param[in] element element_t
 */
void TypeConverter::mpz_from_element(mpz_t mpz, element_t element){
    unsigned char str[element_length_in_bytes(element)];
    element_to_bytes(str, element);
    if(str[1] > 0) {
        unsigned char str2[str[1]];
        strcpy((char*)str2, (char*)(str + 2));
        mpz_set_str(mpz, (char *)str2, 62);
    }
    if(mpz_cmp_ui(mpz, 0) == 0){
        throw std::runtime_error("mpz_from_element: mpz = 0");
    }
}

/**
 * @brief parse mpz_t to element_t
 * 
 * @param[out] element element_t
 * @param[in] mpz mpz_t
 */
void TypeConverter::mpz_to_element(element_t element, mpz_t mpz){
    unsigned char str[element_length_in_bytes(element)];
    mpz_get_str((char*)str, 62, mpz);
    unsigned char str2[strlen((char*)str) + 2];
    strcpy((char*)(str2 + 2), (char*)str);
    str2[1] = strlen((char*)str);
    str2[0] = 0;
    element_from_bytes(element, (unsigned char *)str2);
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