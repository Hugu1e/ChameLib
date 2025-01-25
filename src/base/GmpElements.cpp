#include <base/GmpElements.h>

GmpElements::GmpElements()
{
    elements_gmp = new std::unordered_map<std::string, MP_INT *>();
}

GmpElements::GmpElements(const GmpElements &other){
    elements_gmp = new std::unordered_map<std::string, MP_INT *>();

    std::unordered_map<std::string, MP_INT *>::iterator it;
    for(it = other.elements_gmp->begin(); it != other.elements_gmp->end(); ++it){
        MP_INT *element = new MP_INT();
        mpz_init(element);
        mpz_set(element, it->second);
        elements_gmp->insert(std::pair<std::string, MP_INT *>(it->first, element));
    }
}

GmpElements &GmpElements::operator=(const GmpElements &other)
{
    if (this == &other)
        return *this;

    // Clear existing elements
    std::unordered_map<std::string, MP_INT *>::iterator it;
    for (it = elements_gmp->begin(); it != elements_gmp->end(); ++it)
    {
        mpz_clear(it->second);
        // TODO whether necessary ?
        delete it->second;
    }
    delete elements_gmp;

    // Copy elements from other
    elements_gmp = new std::unordered_map<std::string, MP_INT *>();
    for (it = other.elements_gmp->begin(); it != other.elements_gmp->end(); ++it)
    {
        MP_INT *element = new MP_INT();
        mpz_init(element);
        mpz_set(element, it->second);
        elements_gmp->insert(std::pair<std::string, MP_INT *>(it->first, element));
    }

    return *this;
}

MP_INT *GmpElements::getElement(std::string s)
{
    std::unordered_map<std::string, MP_INT *>::iterator it;
    it = elements_gmp->find(s);
    if (it != elements_gmp->end())
    {
        return it->second;
    }
    return NULL;
}

void GmpElements::insertElement(std::string s, MP_INT *element)
{
    if(getElement(s) != NULL){
        throw ElementExistsException("insertElement(): Element " + s + " already exists");
    }

    MP_INT *insertElement = new MP_INT();
    mpz_init(insertElement);
    mpz_set(insertElement, element);

    elements_gmp->insert(std::pair<std::string, MP_INT *>(s, insertElement));
    
}

void GmpElements::printElement()
{
    if (elements_gmp->empty())
    {
        std::cout << "No elements" << std::endl;
        return;
    }

    std::unordered_map<std::string, MP_INT *>::iterator it;
    std::cout << "Mpz: " << elements_gmp->size() << " elements" << std::endl;
    for (it = elements_gmp->begin(); it != elements_gmp->end(); ++it)
    {
        std::cout << it->first << ": " << std::endl;
        gmp_printf("%Zd\n", it->second);
    }
}

void GmpElements::printElement(std::string s){
    if(getElement(s) != NULL){
        gmp_printf("%s: %Zd\n", s.c_str(), getElement(s));
    }
    else{
        std::cout << "element " << s << " not exist"<< std::endl;
    }
}

GmpElements::~GmpElements()
{
    std::unordered_map<std::string, MP_INT *>::iterator it;
    for (it = elements_gmp->begin(); it != elements_gmp->end(); ++it)
    {
        mpz_clear(it->second);
    }
}