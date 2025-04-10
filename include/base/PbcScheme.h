#ifndef CHAMELIB_PBCSCHEME_H
#define CHAMELIB_PBCSCHEME_H

#include <pbc/pbc.h>
#include "../curve/params.h"
#include "../exception/CurveException.h"

class PbcScheme{
    protected:
        pairing_t pairing;
        bool shared_pairing = false;  // true if the pairing is shared, then do not free it

        element_s *G1 = new element_s();
        element_s *G2 = new element_s();
        element_s *GT = new element_s();
        element_s *Zn = new element_s();

    public:
        PbcScheme(){}
        PbcScheme(const int curve);

        void initCurve(const int curve);

        void initPairing(std::string param);

        void init(element_t _G1, element_t _G2, element_t _GT, element_t _Zn, bool shared_pairing = false);
        void init(element_t _G1, element_t _GT, element_t _Zn, bool shared_pairing = false);
        void init(element_t _G, element_t _Zn, bool shared_pairing = false);

        void defaultInit();

        element_s* GetZrElement();

        element_s* GetG1Element();

        element_s* GetG2Element();

        element_s* GetGTElement();

        ~PbcScheme();
};

#endif  // CHAMELIB_PBCSCHEME_H