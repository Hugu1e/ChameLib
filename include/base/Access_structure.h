#ifndef CHAMELIB_ACCESS_STRUCTURE_H
#define CHAMELIB_ACCESS_STRUCTURE_H

#include "Element_t_vector.h"
#include "Element_t_matrix.h"
#include <map>

class Access_structure
{
private:
    Element_t_vector *ID;
    Element_t_matrix *M;
    std::map<signed long int, std::string> *rho;
    std::string *name;

public:
    Access_structure();
    Access_structure(Element_t_matrix *M, std::map<signed long int, std::string> *rho);
    Access_structure(Element_t_vector *ID, Element_t_matrix *M, std::map<signed long int, std::string> *rho, std::string *name);

    Element_t_vector *getID();
    Element_t_matrix *getM();
    std::map<signed long int, std::string> *getRho();
    std::string *getName();

    void setMSimply(Element_t_matrix *M);
    void setRhoSimply(std::map<signed long int, std::string> *rho);

    void setM(Element_t_matrix *M);
    void setRho(std::map<signed long int, std::string> *rho);
};

#endif  // CHAMELIB_ACCESS_STRUCTURE_H
