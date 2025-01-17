#ifndef CHAMELIB_GMPELEMENT_H
#define CHAMELIB_GMPELEMENT_H

#include <gmp.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <exception/ElementException.h>

class GmpElements{
    private:
        std::unordered_map<std::string, MP_INT *> *elements_gmp;
    
    public:
        GmpElements();
        ~GmpElements();
        
        MP_INT* getElement(std::string s);
        
        void insertElement(std::string s, MP_INT *element);
        
        void printElement();
        void printElement(std::string s);
};

#endif  // CHAMELIB_GMPELEMENT_H