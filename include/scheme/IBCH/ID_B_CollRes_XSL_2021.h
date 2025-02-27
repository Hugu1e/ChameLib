#ifndef ID_B_CollRes_XSL_2021_H
#define ID_B_CollRes_XSL_2021_H

#include "../../base/PbcElements.h"
#include "../../base/PbcScheme.h"
#include "../../utils/Hash.h"
#include <cstring>

class ID_B_CollRes_XSL_2021_pp: public PbcElements{
    private:
        int n;
        PbcElements u;
    public:
        int get_n(){
            return n;
        }
        void set_n(int n){
            this->n = n;
        }
        PbcElements &get_u(){
            return u;
        }
};

class ID_B_CollRes_XSL_2021_msk: public PbcElements{};

class ID_B_CollRes_XSL_2021_tk: public PbcElements{};

class ID_B_CollRes_XSL_2021_h: public PbcElements{};

class ID_B_CollRes_XSL_2021_r: public PbcElements{};


class ID_B_CollRes_XSL_2021: public PbcScheme{
    private:
        element_t a;  // secret α ∈ Zp
        element_t t;  // t ∈ Zp

        element_t tmp_G, tmp_G_2, tmp_H, tmp_H_2, tmp_GT, tmp_GT_2, tmp_GT_3, tmp_Zn;

        bool swap;

        bool getBit(const char *bytes, int index);

        bool Pairing(element_t res, element_t g1, element_t g2);

    public:
        ID_B_CollRes_XSL_2021(int curve, bool swap);

        void SetUp(ID_B_CollRes_XSL_2021_pp &pp, ID_B_CollRes_XSL_2021_msk &msk, ID_B_CollRes_XSL_2021_tk &tk, ID_B_CollRes_XSL_2021_h &h, ID_B_CollRes_XSL_2021_r &r, ID_B_CollRes_XSL_2021_r &r_p, unsigned long int n);

        void KeyGen(ID_B_CollRes_XSL_2021_tk &tk, ID_B_CollRes_XSL_2021_msk &msk, const char *I, ID_B_CollRes_XSL_2021_pp &pp);

        void Hash(ID_B_CollRes_XSL_2021_h &h, ID_B_CollRes_XSL_2021_r &r, element_t m, const char *I, ID_B_CollRes_XSL_2021_pp &pp);

        bool Check(ID_B_CollRes_XSL_2021_h &h, element_t m, ID_B_CollRes_XSL_2021_r &r, const char *I, ID_B_CollRes_XSL_2021_pp &pp);

        void Adapt(ID_B_CollRes_XSL_2021_r &r_p, element_t m_p, ID_B_CollRes_XSL_2021_h &h, element_t m, ID_B_CollRes_XSL_2021_r &r, ID_B_CollRes_XSL_2021_tk &tk);
                        
        bool Verify(ID_B_CollRes_XSL_2021_h &h, element_t m_p, ID_B_CollRes_XSL_2021_r &r_p, const char *I, ID_B_CollRes_XSL_2021_pp &pp);

        ~ID_B_CollRes_XSL_2021();

        enum {
            g, g1, g2
        };

        enum {
            s
        };

        enum {
            tk1, tk2
        };

        enum {
            h1
        };

        enum {
            r1, r2
        };
};


#endif //ID_B_CollRes_XSL_2021_H