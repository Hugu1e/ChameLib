#ifndef CHAMELIB_PBCSCHEME_H
#define CHAMELIB_PBCSCHEME_H

#include <pbc/pbc.h>
#include "../curve/params.h"
#include "../exception/CurveException.h"

class PbcScheme{
    protected:
        pairing_t pairing;
        element_t G1, G2, GT, Zn;

    public:
        PbcScheme(){}
        PbcScheme(const int curve);

        void initCurve(const int curve);

        void initPairing(std::string param);

        void init(element_t _G1, element_t _G2, element_t _GT, element_t _Zn);

        void init(element_t _G, element_t _Zn);

        element_s* GetZrElement();

        element_s* GetG1Element();

        element_s* GetG2Element();

        element_s* GetGTElement();

        ~PbcScheme(){}
};

#endif  // CHAMELIB_PBCSCHEME_H