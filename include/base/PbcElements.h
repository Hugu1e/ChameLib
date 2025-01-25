#ifndef CHAMELIB_PBCELEMENT_H
#define CHAMELIB_PBCELEMENT_H

#include <pbc.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <exception/ElementException.h>

class PbcElements{
    private:
        std::unordered_map<std::string, element_s*> *elements_G1;
        std::unordered_map<std::string, element_s*> *elements_G2;
        std::unordered_map<std::string, element_s*> *elements_GT;
        std::unordered_map<std::string, element_s*> *elements_Zn;
    
    public:
        PbcElements();
        ~PbcElements();

        PbcElements(const PbcElements &other);
        PbcElements &operator=(const PbcElements &other);
        
        element_s* getElement(std::string s, std::string group);
        element_s* getElement(std::string s);
        
        void insertElement(std::string s, std::string group, element_s *element);
        
        void printElement();
        void printElement(std::string s);
};

#endif  // CHAMELIB_PBCELEMENT_H