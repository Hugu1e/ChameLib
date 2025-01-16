#include <exception/BaseException.h>

class ElementExistsException: public BaseException{
    public:
        ElementExistsException(const std::string& message):BaseException(message){}
};