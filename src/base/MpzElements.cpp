#include <base/MpzElements.h>

MpzElements::MpzElements()
{
    elements_mpz = new std::unordered_map<std::string, MP_INT *>();
}

MP_INT *MpzElements::getElement(std::string s)
{
    std::unordered_map<std::string, MP_INT *>::iterator it;
    it = elements_mpz->find(s);
    if (it != elements_mpz->end())
    {
        return it->second;
    }
    return NULL;
}

void MpzElements::insertElement(std::string s, MP_INT *element)
{
    if(getElement(s) != NULL){
        throw ElementExistsException("insertElement(): Element " + s + " already exists");
    }

    MP_INT *insertElement = new MP_INT();
    mpz_init(insertElement);
    mpz_set(insertElement, element);

    elements_mpz->insert(std::pair<std::string, MP_INT *>(s, insertElement));
    
}

void MpzElements::printElement()
{
    if (elements_mpz->empty())
    {
        std::cout << "No elements" << std::endl;
        return;
    }

    std::unordered_map<std::string, MP_INT *>::iterator it;
    std::cout << "Mpz: " << elements_mpz->size() << " elements" << std::endl;
    for (it = elements_mpz->begin(); it != elements_mpz->end(); ++it)
    {
        std::cout << it->first << ": " << std::endl;
        gmp_printf("%Zd\n", it->second);
    }
}

void MpzElements::printElement(std::string s){
    if(getElement(s) != NULL){
        gmp_printf("%s: %Zd\n", s.c_str(), getElement(s));
    }
    else{
        std::cout << "element " << s << " not exist"<< std::endl;
    }
}

MpzElements::~MpzElements()
{
    std::unordered_map<std::string, MP_INT *>::iterator it;
    for (it = elements_mpz->begin(); it != elements_mpz->end(); ++it)
    {
        mpz_clear(it->second);
    }
}