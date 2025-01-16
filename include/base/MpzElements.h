#ifndef CHAMELIB_MPZELEMENT_H
#define CHAMELIB_MPZELEMENT_H

#include <gmp.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <exception/ElementException.h>

class MpzElements{
    private:
        std::unordered_map<std::string, MP_INT *> *elements_mpz;
    
    public:
        MpzElements();
        ~MpzElements();
        
        MP_INT* getElement(std::string s);
        
        void insertElement(std::string s, MP_INT *element);
        
        void printElement();
        void printElement(std::string s);
};

#endif  // CHAMELIB_MPZELEMENT_H