#ifndef CHAMELIB_ELEMENT_T_MATRIX_H
#define CHAMELIB_ELEMENT_T_MATRIX_H

#include "Element_t_vector.h"
#include "Num_vector.h"

class Element_t_matrix
{
private:
    std::vector<std::vector<element_s *>> value;
    std::vector<std::string> names;

public:
    Element_t_matrix();
    Element_t_matrix(signed long int r, signed long int c, element_s *sample_element);

    void printMatrix();

    signed long int row();
    signed long int col();

    element_s *getElement(signed long int r, signed long int c);
    void setElement(signed long int r, signed long int c, element_s *elem);

    void pushBack(Element_t_vector *v);
    void pushBack(Element_t_vector *v, signed long int r);

    void pushBackName(std::string name);
    std::string getName(signed long int r);

    Element_t_matrix* inverse();

    Element_t_vector* getCoordinateAxisUnitVector();

    static signed long int gaussElimination(Element_t_vector *x, Element_t_matrix *A, Element_t_vector *b);
};

#endif // CHAMELIB_ELEMENT_T_MATRIX_H
