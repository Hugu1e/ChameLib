#ifndef CHAMELIB_GMPELEMENT_H
#define CHAMELIB_GMPELEMENT_H

#include <gmp.h>
#include <string>

class GmpElements{
    private:
        int size;
        MP_INT ** elements;
    
    public:
        GmpElements();
            
        void init(int n);
        void init_same_as(const GmpElements &other);

        GmpElements(const GmpElements &other);
        GmpElements &operator=(const GmpElements &other);
        
        MP_INT* operator[](int index);

        void set(int index, MP_INT *element);

        int getSize();

        bool operator==(const GmpElements &other) const;
                
        void print();
        void print(int index);

        ~GmpElements();
};

#endif  // CHAMELIB_GMPELEMENT_H