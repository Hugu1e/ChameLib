#include <exception>
#include <string>

class BaseException: public std::exception{
    private:
        std::string m;
    public:
        BaseException(const std::string& message):m(message){}

        const char* what() const noexcept override {
            return m.c_str();
        }
};

class VarNotExistsException: public BaseException{
    public:
        VarNotExistsException(const std::string& message):BaseException(message){}
};