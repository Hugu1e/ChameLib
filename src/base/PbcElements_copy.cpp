#include <base/PbcElements_copy.h>

PbcElements_copy::PbcElements_copy(){}

PbcElements_copy::PbcElements_copy(const PbcElements_copy &other){
    if (this == &other)
        return;

    for (auto elem : other.elements) {
        element_s *element = new element_s();
        element_init_same_as(element, elem);
        element_set(element, elem);
        elements.push_back(element);
    }
}

PbcElements_copy &PbcElements_copy::operator=(const PbcElements_copy &other){
    if (this == &other)
        return *this;

    // Clear existing elements
    for (auto elem : elements) {
        element_clear(elem);
    }
    elements.clear();
    
    for (auto elem : other.elements) {
        element_s *element = new element_s();
        element_init_same_as(element, elem);
        element_set(element, elem);
        elements.push_back(element);
    }
}

element_s *PbcElements_copy::get(int index){
    if(index < 0 || index >= elements.size()){
        return NULL;
    }
    return elements[index];
}

void PbcElements_copy::insert(int index, element_s *element){
    if(get(index) != NULL){
        throw std::invalid_argument("PbcElements_copy::insertElement(): Element[" + std::to_string(index) + "] already exists");
    }

    element_s *insertElement = new element_s();
    element_init_same_as(insertElement, element);
    element_set(insertElement, element);

    if(index < elements.size()){
        elements[index] = insertElement;
    }else{
        int nulls = index - elements.size();
        while(nulls > 0){
            elements.push_back(NULL);
            nulls--;
        }
        elements.push_back(insertElement);
    }
}

void PbcElements_copy::print(){
    if (!elements.empty()){
        std::cout << "PbcElements_copy: " << elements.size() << " elements" << std::endl;
        for(int i = 0; i < elements.size(); i++){
            if(get(i) != NULL){
                std::cout << "Element[" << i << "]: " << std::endl;
                element_printf("%B\n", elements[i]);
            }
        }
    }else{
        std::cout << "No elements" << std::endl;
    }
}

void PbcElements_copy::print(int index){
    if(get(index) != NULL){
        std::cout << "Element[" << index << "]: " << std::endl;
        element_printf("%B\n", elements[index]);
    }else{
        std::cout << "Element[" << index << "] does not exist" << std::endl;
    }
}

PbcElements_copy::~PbcElements_copy()
{
    for (auto elem : elements) {
        element_clear(elem);
    }
}

