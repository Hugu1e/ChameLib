#ifndef CHAMELIB_PBCELEMENT_COPY_H
#define CHAMELIB_PBCELEMENT_COPY_H

#include <pbc.h>
#include <string>
#include <exception/ElementException.h>

class PbcElements_copy{
    private:
        int size;
        element_s** elements;
    
    public:
        PbcElements_copy();
        
        void init(int n);

        PbcElements_copy(const PbcElements_copy &other);
        
        element_s* operator[](int index);

        void set(int index, element_s *element);
                
        void print();
        void print(int index);

        ~PbcElements_copy();
};

#endif  // CHAMELIB_PBCELEMENT_COPY_H