#include "base/PbcElements.h"

PbcElements::PbcElements() : size(0), elements(nullptr){}

void PbcElements::init(int n){
    if(size > 0){
        // already initialized
        return;
    }
    size = n;
    elements = new element_s*[n];
    for(int i=0; i<n; i++){
        element_s *element = new element_s();
        elements[i] = element;
    }
}

void PbcElements::init_same_as(const PbcElements &other){
    if(size > 0){
        // already initialized
        return;
    }
    init(other.size);
}

PbcElements::PbcElements(const PbcElements &other)
{
    if (this == &other)
        return;

    size = other.size;
    
    elements = new element_s*[other.size];
    for(int i = 0; i < other.size; i++){
        element_s *element = new element_s();
        element_init_same_as(element, other.elements[i]);
        element_set(element, other.elements[i]);
        elements[i] = element;
    }
}

PbcElements &PbcElements::operator=(const PbcElements &other){
    if (this == &other)
        return *this;

    size = other.size;

    elements = new element_s*[other.size];
    for(int i = 0; i < other.size; i++){
        element_s *element = new element_s();
        element_init_same_as(element, other.elements[i]);
        element_set(element, other.elements[i]);
        elements[i] = element;
    }
}

element_s* PbcElements::operator[](int index){
    return elements[index];
}

element_s* PbcElements::get(int index){
    return elements[index];
}

void PbcElements::set(int index, element_s *element){
    element_init_same_as(elements[index], element);
    element_set(elements[index], element);
}

int PbcElements::getSize(){
    return size;
}

bool PbcElements::operator==(const PbcElements &other) const{
    if(size != other.size){
        return false;
    }
    for(int i = 0; i < size; i++){
        if(element_cmp(elements[i], other.elements[i]) != 0){
            return false;
        }
    }
    return true;
}

void PbcElements::print(){
    if (size > 0){
        printf("PbcElements: %d elements\n", size);
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

void PbcElements::print(int index){
    if(elements[index] != nullptr){
        printf("Element[%d]: \n", index);
        element_printf("%B\n", elements[index]);
    }else{
        printf("Element[%d] does not exist\n", index);
    }
}

void PbcElements::pop_front(){
    if (size > 0){
        element_clear(elements[0]);
        delete elements[0];
        for(int i = 0; i < size-1; i++){
            elements[i] = elements[i+1];
        }
        size--;
    }
}

PbcElements::~PbcElements(){
    if (elements != nullptr){
        for (int i = 0; i < size; i++){
            element_clear(elements[i]);
            delete elements[i];
        }
        delete[] elements;
    }
}

