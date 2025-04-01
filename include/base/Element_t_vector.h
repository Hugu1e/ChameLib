#ifndef CHAMELIB_ELEMENT_T_VECTOR_H
#define CHAMELIB_ELEMENT_T_VECTOR_H

#include <pbc/pbc.h>
#include <vector>
#include "../utils/Logger.h"

class Element_t_vector
{
private:
    std::vector<element_s *> value;

public:
    Element_t_vector();
    Element_t_vector(signed long int len, element_s *sample_element);
    Element_t_vector(const Element_t_vector &v);

    Element_t_vector &operator=(const Element_t_vector &v);

    void printVector();

    signed long int length() const;

    element_s *getElement(signed long int i) const;
    element_s *operator[](signed long int i) const;
    void setElement(signed long int i, element_s *elem);

    void pushBack(element_s *elem);

    void resizeValue(signed long int i);
};

#endif // CHAMELIB_ELEMENT_T_VECTOR_H
