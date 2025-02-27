#ifndef CHAMELIB_CURVEEXCEPTION_H
#define CHAMELIB_CURVEEXCEPTION_H

#include "BaseException.h"

class CurveException: public BaseException{
    public:
        CurveException(const int type):BaseException(){
            switch(type){
                case INVALID_CURVE:
                    m = "Invalid curve type";
                    break;
                case INVALID_GROUP:
                    m = "Invalid group type";
                    break;
                default:
                    m = "Unknown curve exception";
                    break;
            }
        }

        const static int INVALID_CURVE = 0;
        const static int INVALID_GROUP = 1;
};

#endif  // CHAMELIB_CURVEEXCEPTION_H