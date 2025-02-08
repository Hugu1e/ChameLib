#include <base/GmpElements_copy.h>

GmpElements_copy::GmpElements_copy() : size(0), elements(nullptr){}

void GmpElements_copy::init(int n){
    size = n;
    elements = new MP_INT*[n];
    for(int i=0; i<n; i++){
        MP_INT *element = new MP_INT();
        mpz_init(element);
        elements[i] = element;
    }
}

GmpElements_copy::GmpElements_copy(const GmpElements_copy &other){
    if (this == &other)
        return;
    
    elements = new MP_INT*[other.size];
    for(int i = 0; i < other.size; i++){
        MP_INT *element = new MP_INT();
        mpz_init(element);
        mpz_set(element, other.elements[i]);
        elements[i] = element;
    }
}

MP_INT* GmpElements_copy::operator[](int index){
    return elements[index];
}

void GmpElements_copy::set(int index, MP_INT *element){
    mpz_set(elements[index], element);
}

void GmpElements_copy::print(){
    if (size > 0){
        printf("GmpElements: %d elements\n", size);
        for(int i = 0; i < size; i++){
            if(elements[i] != nullptr){
                printf("Element[%d]: \n", i);
                gmp_printf("%Zd\n", elements[i]);
            }
        }
    }else{
        printf("No elements\n");
    }
}

void GmpElements_copy::print(int index){
    if(elements[index] != nullptr){
        printf("Element[%d]: \n", index);
        gmp_printf("%Zd\n", elements[index]);
    }else{
        printf("Element[%d] does not exist\n", index);
    }
}

GmpElements_copy::~GmpElements_copy(){
    if (elements != nullptr){
        for (int i = 0; i < size; i++){
            mpz_clear(elements[i]);
        }
        delete[] elements;
    }
}

