#include <base/PbcElements.h>

PbcElements::PbcElements()
{
    elements_G1 = new std::unordered_map<std::string, element_s *>();
    elements_G2 = new std::unordered_map<std::string, element_s *>();
    elements_GT = new std::unordered_map<std::string, element_s *>();
    elements_Zn = new std::unordered_map<std::string, element_s *>();
}

PbcElements::PbcElements(const PbcElements &other){
    elements_G1 = new std::unordered_map<std::string, element_s *>();
    elements_G2 = new std::unordered_map<std::string, element_s *>();
    elements_GT = new std::unordered_map<std::string, element_s *>();
    elements_Zn = new std::unordered_map<std::string, element_s *>();

    std::unordered_map<std::string, element_s *>::iterator it;
    for(it = other.elements_G1->begin(); it != other.elements_G1->end(); ++it){
        element_s *element = new element_s();
        element_init_same_as(element, it->second);
        element_set(element, it->second);
        elements_G1->insert(std::pair<std::string, element_s *>(it->first, element));
    }
    for(it = other.elements_G2->begin(); it != other.elements_G2->end(); ++it){
        element_s *element = new element_s();
        element_init_same_as(element, it->second);
        element_set(element, it->second);
        elements_G2->insert(std::pair<std::string, element_s *>(it->first, element));
    }
    for(it = other.elements_GT->begin(); it != other.elements_GT->end(); ++it){
        element_s *element = new element_s();
        element_init_same_as(element, it->second);
        element_set(element, it->second);
        elements_GT->insert(std::pair<std::string, element_s *>(it->first, element));
    }
    for(it = other.elements_Zn->begin(); it != other.elements_Zn->end(); ++it){
        element_s *element = new element_s();
        element_init_same_as(element, it->second);
        element_set(element, it->second);
        elements_Zn->insert(std::pair<std::string, element_s *>(it->first, element));
    }
}

PbcElements &PbcElements::operator=(const PbcElements &other)
{
    if (this == &other)
        return *this;

    // Clear existing elements
    std::unordered_map<std::string, element_s *>::iterator it;
    for (it = elements_G1->begin(); it != elements_G1->end(); ++it)
    {
        element_clear(it->second);
    }
    for (it = elements_G2->begin(); it != elements_G2->end(); ++it)
    {
        element_clear(it->second);
    }
    for (it = elements_GT->begin(); it != elements_GT->end(); ++it)
    {
        element_clear(it->second);
    }
    for (it = elements_Zn->begin(); it != elements_Zn->end(); ++it)
    {
        element_clear(it->second);
    }

    delete elements_G1;
    delete elements_G2;
    delete elements_GT;
    delete elements_Zn;

    // Allocate new maps
    elements_G1 = new std::unordered_map<std::string, element_s *>();
    elements_G2 = new std::unordered_map<std::string, element_s *>();
    elements_GT = new std::unordered_map<std::string, element_s *>();
    elements_Zn = new std::unordered_map<std::string, element_s *>();

    // Copy elements from other
    for (it = other.elements_G1->begin(); it != other.elements_G1->end(); ++it)
    {
        element_s *element = new element_s();
        element_init_same_as(element, it->second);
        element_set(element, it->second);
        elements_G1->insert(std::pair<std::string, element_s *>(it->first, element));
    }
    for (it = other.elements_G2->begin(); it != other.elements_G2->end(); ++it)
    {
        element_s *element = new element_s();
        element_init_same_as(element, it->second);
        element_set(element, it->second);
        elements_G2->insert(std::pair<std::string, element_s *>(it->first, element));
    }
    for (it = other.elements_GT->begin(); it != other.elements_GT->end(); ++it)
    {
        element_s *element = new element_s();
        element_init_same_as(element, it->second);
        element_set(element, it->second);
        elements_GT->insert(std::pair<std::string, element_s *>(it->first, element));
    }
    for (it = other.elements_Zn->begin(); it != other.elements_Zn->end(); ++it)
    {
        element_s *element = new element_s();
        element_init_same_as(element, it->second);
        element_set(element, it->second);
        elements_Zn->insert(std::pair<std::string, element_s *>(it->first, element));
    }

    return *this;
}

element_s *PbcElements::getElement(std::string s, std::string group)
{
    std::unordered_map<std::string, element_s *>::iterator it;
    if (group == "G1")
    {
        it = elements_G1->find(s);
        if (it == elements_G1->end())
        {
            return NULL;
        }
        else
        {
            return (*it).second;
        }
    }
    else if (group == "G2")
    {
        it = elements_G2->find(s);
        if (it == elements_G2->end())
        {
            return NULL;
        }
        else
        {
            return (*it).second;
        }
    }
    else if (group == "GT")
    {
        it = elements_GT->find(s);
        if (it == elements_GT->end())
        {
            return NULL;
        }
        else
        {
            return (*it).second;
        }
    }
    else if (group == "Zn")
    {
        it = elements_Zn->find(s);
        if (it == elements_Zn->end())
        {
            return NULL;
        }
        else
        {
            return (*it).second;
        }
    }
}

element_s *PbcElements::getElement(std::string s)
{
    element_s *res;

    res = getElement(s, "G1");
    if (res != NULL)
    {
        return res;
    }
    res = getElement(s, "G2");
    if (res != NULL)
    {
        return res;
    }
    res = getElement(s, "GT");
    if (res != NULL)
    {
        return res;
    }
    res = getElement(s, "Zn");
    if (res != NULL)
    {
        return res;
    }

    return NULL;
}

void PbcElements::insertElement(std::string s, std::string group, element_s *element)
{
    if(getElement(s) != NULL){
        throw ElementExistsException("insertElement(): Element " + s + " already exists");
    }

    element_s *insertElement = new element_s();
    element_init_same_as(insertElement, element);
    element_set(insertElement, element);
    if (group == "G1")
    {
        elements_G1->insert(std::pair<std::string, element_s *>(s, insertElement));
    }
    else if (group == "G2")
    {
        elements_G2->insert(std::pair<std::string, element_s *>(s, insertElement));
    }
    else if (group == "GT")
    {
        elements_GT->insert(std::pair<std::string, element_s *>(s, insertElement));
    }
    else if (group == "Zn")
    {
        elements_Zn->insert(std::pair<std::string, element_s *>(s, insertElement));
    }else{
        throw std::invalid_argument("PbcElements::insertElement(): Invalid group " + group);
    }
}

void PbcElements::printElement()
{
    if (elements_G1->empty() && elements_G2->empty() && elements_GT->empty() && elements_Zn->empty())
    {
        std::cout << "No elements" << std::endl;
        return;
    }

    std::unordered_map<std::string, element_s *>::iterator it;
    if (!elements_G1->empty())
    {
        std::cout << "G1: " << elements_G1->size() << " elements" << std::endl;
        for (it = elements_G1->begin(); it != elements_G1->end(); ++it)
        {
            std::cout << it->first << ": " << std::endl;
            element_printf("%B\n", it->second);
        }
    }
    if (!elements_G2->empty())
    {
        std::cout << "G2: " << elements_G2->size() << " elements" << std::endl;
        for (it = elements_G2->begin(); it != elements_G2->end(); ++it)
        {
            std::cout << it->first << ": " << std::endl;
            element_printf("%B\n", it->second);
        }
    }
    if (!elements_GT->empty())
    {
        std::cout << "GT: " << elements_GT->size() << " elements" << std::endl;
        for (it = elements_GT->begin(); it != elements_GT->end(); ++it)
        {
            std::cout << it->first << ": " << std::endl;
            element_printf("%B\n", it->second);
        }
    }
    if (!elements_Zn->empty())
    {
        std::cout << "Zn: " << elements_Zn->size() << " elements" << std::endl;
        for (it = elements_Zn->begin(); it != elements_Zn->end(); ++it)
        {
            std::cout << it->first << ": " << std::endl;
            element_printf("%B\n", it->second);
        }
    }
}

void PbcElements::printElement(std::string s){
    if(getElement(s) != NULL){
        element_printf("%s: %B\n", s.c_str(), getElement(s));
    }
    else{
        std::cout << "element " << s << " not exist"<< std::endl;
    }
}

PbcElements::~PbcElements()
{
    std::unordered_map<std::string, element_s *>::iterator it;
    for (it = elements_G1->begin(); it != elements_G1->end(); ++it)
    {
        element_clear(it->second);
    }
    for (it = elements_G2->begin(); it != elements_G2->end(); ++it)
    {
        element_clear(it->second);
    }
    for (it = elements_GT->begin(); it != elements_GT->end(); ++it)
    {
        element_clear(it->second);
    }
    for (it = elements_Zn->begin(); it != elements_Zn->end(); ++it)
    {
        element_clear(it->second);
    }
}

