#include <base/Access_structure.h>

Access_structure::Access_structure()
{
    this->ID = new Element_t_vector();
    this->M = new Element_t_matrix();
    this->rho = new std::map<signed long int, std::string>();
    this->name = new std::string();
}

Access_structure::Access_structure(Element_t_matrix *M, std::map<signed long int, std::string> *rho)
{
    this->M = new Element_t_matrix(M->row(), M->col(), M->getElement(0, 0));

    this->rho = new std::map<signed long int, std::string>();

    // copy M
    for (signed long int i = 0; i < M->row(); ++i)
    {
        for (signed long int j = 0; j < M->col(); ++j)
        {
            this->M->setElement(i, j, M->getElement(i, j));
        }
    }

    // copy rho
    std::map<signed long int, std::string>::iterator iterator1;
    for (iterator1 = rho->begin(); iterator1 != rho->end(); ++iterator1)
    {
        this->rho->insert(std::pair<signed long int, std::string>(iterator1->first, iterator1->second));
    }
}

Access_structure::Access_structure(Element_t_vector *ID, Element_t_matrix *M, std::map<signed long int, std::string> *rho,
                                   std::string *name)
{
    this->ID = new Element_t_vector(ID->length(), ID->getElement(0));

    this->M = new Element_t_matrix(M->row(), M->col(), M->getElement(0, 0));

    this->rho = new std::map<signed long int, std::string>();

    this->name = new std::string();

    // copy ID
    for (signed long int i = 0; i < ID->length(); ++i)
    {
        this->ID->setElement(i, ID->getElement(i));
    }

    // copy M
    for (signed long int i = 0; i < M->row(); ++i)
    {
        for (signed long int j = 0; j < M->col(); ++j)
        {
            this->M->setElement(i, j, M->getElement(i, j));
        }
    }

    // copy rho
    std::map<signed long int, std::string>::iterator iterator1;
    for (iterator1 = rho->begin(); iterator1 != rho->end(); ++iterator1)
    {
        this->rho->insert(std::pair<signed long int, std::string>(iterator1->first, iterator1->second));
    }

    // copy name
    *(this->name) = *name;
}

Element_t_vector *Access_structure::getID()
{
    return ID;
}

Element_t_matrix *Access_structure::getM()
{
    return M;
}

std::map<signed long int, std::string> *Access_structure::getRho()
{
    return rho;
}

std::string *Access_structure::getName()
{
    return name;
}

void Access_structure::setMSimply(Element_t_matrix *M)
{
    this->M = M;
}

void Access_structure::setRhoSimply(std::map<signed long int, std::string> *rho)
{
    this->rho = rho;
}

void Access_structure::setM(Element_t_matrix *M)
{
    this->M = new Element_t_matrix(M->row(), M->col(), M->getElement(0, 0));

    // copy M
    for (signed long int i = 0; i < M->row(); ++i)
    {
        for (signed long int j = 0; j < M->col(); ++j)
        {
            this->M->setElement(i, j, M->getElement(i, j));
        }
    }
}

void Access_structure::setRho(std::map<signed long int, std::string> *rho)
{
    this->rho = new std::map<signed long int, std::string>();

    // copy rho
    std::map<signed long int, std::string>::iterator iterator1;
    for (iterator1 = rho->begin(); iterator1 != rho->end(); ++iterator1)
    {
        this->rho->insert(std::pair<signed long int, std::string>(iterator1->first, iterator1->second));
    }
}