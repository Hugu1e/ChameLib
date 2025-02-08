#ifndef CHAMELIB_PBCELEMENT_COPY_H
#define CHAMELIB_PBCELEMENT_COPY_H

#include <pbc.h>
#include <iostream>
#include <string>
#include <vector>
#include <exception/ElementException.h>

class PbcElements_copy{
    private:
        std::vector<element_s*> elements;
    
    public:
        PbcElements_copy();
        ~PbcElements_copy();

        PbcElements_copy(const PbcElements_copy &other);
        PbcElements_copy &operator=(const PbcElements_copy &other);
        
        element_s* get(int index);
        
        void insert(int index, element_s *element);
        
        void print();
        void print(int index);
};

#endif  // CHAMELIB_PBCELEMENT_COPY_H