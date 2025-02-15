#ifndef CHAMELIB_ELEMENTEXCEPTION_H
#define CHAMELIB_ELEMENTEXCEPTION_H

#include "BaseException.h"

class ElementExistsException: public BaseException{
    public:
        ElementExistsException(const std::string& message):BaseException(message){}
};

class ElementNotExistsException: public BaseException{
    public:
        ElementNotExistsException(const std::string& message):BaseException(message){}
};

#endif  // CHAMELIB_ELEMENTEXCEPTION_H