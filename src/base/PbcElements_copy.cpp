#include <base/PbcElements_copy.h>

PbcElements_copy::PbcElements_copy() : size(0), elements(nullptr){}

void PbcElements_copy::init(int n){
    size = n;
    elements = new element_s*[n];
    for(int i=0; i<n; i++){
        element_s *element = new element_s();
        elements[i] = element;
    }
}

PbcElements_copy::PbcElements_copy(const PbcElements_copy &other){
    if (this == &other)
        return;
    
    elements = new element_s*[other.size];
    for(int i = 0; i < other.size; i++){
        element_s *element = new element_s();
        element_init_same_as(element, other.elements[i]);
        element_set(element, other.elements[i]);
        elements[i] = element;
    }
}

element_s* PbcElements_copy::operator[](int index){
    return elements[index];
}

void PbcElements_copy::print(){
    if (size > 0){
        printf("PbcElements_copy: %d elements\n", size);
        for(int i = 0; i < size; i++){
            if(elements[i] != nullptr){
                printf("Element[%d]: \n", i);
                element_printf("%B\n", elements[i]);
            }
        }
    }else{
        printf("No elements\n");
    }
}

void PbcElements_copy::print(int index){
    if(elements[index] != nullptr){
        printf("Element[%d]: \n", index);
        element_printf("%B\n", elements[index]);
    }else{
        printf("Element[%d] does not exist\n", index);
    }
}

PbcElements_copy::~PbcElements_copy(){
    if (elements != nullptr){
        for (int i = 0; i < size; i++){
            element_clear(elements[i]);
        }
        delete[] elements;
    }
}

