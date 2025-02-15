#include "base/Num_vector.h"

Num_vector::Num_vector(signed long int len) {
    for (signed long int i = 0; i < len; ++i) {
        signed long int init_value = 0;
        value.push_back(init_value);
    }
}

void Num_vector::printVector() {
    for (signed long int i = 0; i < length(); ++i) {
        if (length() - 1 == i) {
            std::cout << value[i] << std::endl;
        } else {
            std::cout << value[i] << " ";
        }
    }
}

signed long int Num_vector::length() {
    return value.size();
}

signed long int Num_vector::getElement(signed long int i) {
    return value[i];
}
void Num_vector::setElement(signed long int i, signed long int elem) {
    value[i] = elem;
}

void Num_vector::pushBack(signed long int elem) {
    value.push_back(elem);
}

void Num_vector::resizeValue(signed long int i) {
    value.resize(i);
}