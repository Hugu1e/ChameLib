#ifndef CHAMELIB_BASEEXCEPTION_H
#define CHAMELIB_BASEEXCEPTION_H

#include <exception>
#include <string>

class BaseException: public std::exception{
    protected:
        std::string m;
    public:
        BaseException(){}
        BaseException(const std::string& message):m(message){}

        const char* what() const noexcept override {
            return m.c_str();
        }
};

#endif  // CHAMELIB_BASEEXCEPTION_H