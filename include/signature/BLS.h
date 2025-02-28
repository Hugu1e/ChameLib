#ifndef CHAMELIB_BLS_H
#define CHAMELIB_BLS_H

#include "../base/PbcElements.h"
#include "../base/PbcScheme.h"
#include "../utils/Hash.h"

class BLS_pp: public PbcElements{};
class BLS_pk: public PbcElements{};
class BLS_sk: public PbcElements{};
class BLS_signature: public PbcElements{};

class BLS: public PbcScheme{
    private:
        void H(element_t res, std::string m);

        bool swap;
        element_t tmp_G, tmp_H, tmp_H_2, tmp_GT, tmp_GT_2, tmp_Zn;

        void Pairing(element_t res, element_t a, element_t b);
    public:
        BLS(){}
        BLS(int curve, bool swap);

        void init(element_t _G, element_t _H, element_t _GT, element_t _Zn);

        void Setup(BLS_pp &pp, BLS_pk &pk, BLS_sk &sk, BLS_signature &signature);
        void Setup(BLS_pp &pp, BLS_pk &pk, BLS_sk &sk, BLS_signature &signature, element_t _g);

        void KeyGen(BLS_pk &pk, BLS_sk &sk, BLS_pp &pp);

        void Sign(BLS_signature &signature, BLS_sk &sk, std::string message);

        bool Verify(BLS_pp &pp, BLS_pk &pk, std::string message, BLS_signature &signature);

        ~BLS();
        
        enum {
            g
        };

        enum {
            a
        };

        enum {
            y
        };

        enum {
            sigma
        };
};

#endif  // CHAMELIB_BLS_H