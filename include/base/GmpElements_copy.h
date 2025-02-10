#ifndef CHAMELIB_GMPELEMENT_COPY_H
#define CHAMELIB_GMPELEMENT_COPY_H

#include <gmp.h>
#include <string>

class GmpElements_copy{
    private:
        int size;
        MP_INT ** elements;
    
    public:
        GmpElements_copy();
            
        void init(int n);

        GmpElements_copy(const GmpElements_copy &other);
        GmpElements_copy &operator=(const GmpElements_copy &other);
        
        MP_INT* operator[](int index);

        void set(int index, MP_INT *element);

        int getSize();
                
        void print();
        void print(int index);

        ~GmpElements_copy();
};

#endif  // CHAMELIB_GMPELEMENT_COPY_H