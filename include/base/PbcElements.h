#ifndef CHAMELIB_PBCELEMENT_H
#define CHAMELIB_PBCELEMENT_H

#include <pbc.h>
#include <string>
#include <exception/ElementException.h>
#include <stdexcept>

class PbcElements{
    private:
        int size;
        element_s** elements;
    
    public:
        PbcElements();
        
        void init(int n);
        void init_same_as(const PbcElements &other);

        PbcElements(const PbcElements &other);
        PbcElements &operator=(const PbcElements &other);
        
        element_s* operator[](int index);
        element_s* get(int index);

        void set(int index, element_s *element);

        int getSize();

        bool operator==(const PbcElements &other) const;
                
        void print();
        void print(int index);

        ~PbcElements();
};

#endif  // CHAMELIB_PBCELEMENT_H