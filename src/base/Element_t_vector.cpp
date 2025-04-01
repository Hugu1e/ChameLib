#include "base/Element_t_vector.h"

Element_t_vector::Element_t_vector() {}
Element_t_vector::Element_t_vector(signed long int len, element_s *sample_element) {
    for (signed long int i = 0; i < len; ++i) {
        element_s *initialization_element = new element_s();
        element_init_same_as(initialization_element, sample_element);
        value.push_back(initialization_element);
    }
}
Element_t_vector::Element_t_vector(const Element_t_vector &v) {
    value.resize(0);
    for (signed long int i = 0; i < v.length(); ++i) {
        pushBack(v.getElement(i));
    }
}

/**
 * @brief  operator = overload(deep copy)
 * 
 * @param v the right operand
 * @return *this
 */
Element_t_vector& Element_t_vector::operator=(const Element_t_vector &v) {
    for(signed long int i = 0; i < length(); ++i) {
        element_clear(value[i]);
    }
    value.resize(0);
    for (signed long int i = 0; i < v.length(); ++i) {
        pushBack(v.getElement(i));
    }
    return *this;
}

/**
 * @brief formatted output of the vector
 */
void Element_t_vector::printVector() {
    signed long int len = length();
    for (signed long int i = 0; i < len; i++) {
        Logger::PrintPbc("vector["+ std::to_string(i) +"]", value[i]);
    }
}

/**
 * @brief get the length of the vector
 * 
 * @return length of the vector
 */
signed long int Element_t_vector::length() const {
    return value.size();
}

/**
 * @brief get element with index i
 * 
 * @param i index
 * @return element with index i
 */
element_s* Element_t_vector::getElement(signed long int i) const {
    return value[i];
}

element_s *Element_t_vector::operator[](signed long int i) const {
    return value[i];
}

/**
 * @brief set 'elem' to the element with index i
 * 
 * @param i index
 * @param elem 'elem'
 */
void Element_t_vector::setElement(signed long int i, element_s *elem) {
    element_set(value[i], elem);
}

/**
 * @brief add 'elem' to the end of the vector
 * 
 * @param elem 'elem'
 */
void Element_t_vector::pushBack(element_s *elem) {
    element_s *initialization_element = new element_s();
    element_init_same_as(initialization_element, elem);
    element_set(initialization_element, elem);
    value.push_back(initialization_element);
}

/**
 * @brief resize the vector
 * 
 * @param i the new size
 */
void Element_t_vector::resizeValue(signed long int i) {
    if(length() == 0){
        value.resize(i);
    }else{
        if(i < length()) {
            for(signed long int j = i; j < length(); ++j) {
                element_clear(value[j]);
            }
            value.resize(i);
        }else if(i > length()) {
            for(signed long int j = length(); j < i; ++j) {
                element_s *initialization_element = new element_s();
                element_init_same_as(initialization_element, value[0]);
                element_set0(initialization_element);
                value.push_back(initialization_element);
            }
        }
    }
}