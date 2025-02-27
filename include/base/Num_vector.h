#ifndef CHAMELIB_NUM_VECTOR_H
#define CHAMELIB_NUM_VECTOR_H

#include <vector>
#include <iostream>

class Num_vector
{
private:
    std::vector<signed long int> value;

public:
    Num_vector(signed long int len);

    void printVector();

    signed long int length();

    signed long int getElement(signed long int i);
    void setElement(signed long int i, signed long int elem);

    void pushBack(signed long int elem);

    void resizeValue(signed long int i);
};

#endif  // CHAMELIB_NUM_VECTOR_H
