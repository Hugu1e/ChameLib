#ifndef ID_B_CollRes_XSL_2021_H
#define ID_B_CollRes_XSL_2021_H

#include <base/PbcElements.h>
#include <base/PbcScheme.h>
#include <utils/Hash.h>

class ID_B_CollRes_XSL_2021_pp: public PbcElements{};

class ID_B_CollRes_XSL_2021_msk: public PbcElements{};

class ID_B_CollRes_XSL_2021_tk: public PbcElements{};

class ID_B_CollRes_XSL_2021_h{
    private:
        PbcElements h;
        PbcElements r;
    public:
        PbcElements& get_h(){
            return h;
        }
        PbcElements& get_r(){
            return r;
        }
};


class ID_B_CollRes_XSL_2021: public PbcScheme{
    private:
        element_t a;  // secret α ∈ Zp

        unsigned long int n; // n>=1
        element_t t;  // t ∈ Zp


    public:
        ID_B_CollRes_XSL_2021(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void SetUp(ID_B_CollRes_XSL_2021_pp &pp, ID_B_CollRes_XSL_2021_msk &msk, ID_B_CollRes_XSL_2021_tk &tk, ID_B_CollRes_XSL_2021_h &h, ID_B_CollRes_XSL_2021_h &h_p, unsigned long int n);

        void KeyGen(ID_B_CollRes_XSL_2021_tk &tk, ID_B_CollRes_XSL_2021_msk &msk, element_t I, ID_B_CollRes_XSL_2021_pp &pp);

        bool getBit(element_t element, unsigned long int index);

        void Hash(ID_B_CollRes_XSL_2021_h &h, element_t m, element_t I, ID_B_CollRes_XSL_2021_pp &pp);

        bool Check(ID_B_CollRes_XSL_2021_h &h, element_t m, element_t I, ID_B_CollRes_XSL_2021_pp &pp);

        void Adapt(ID_B_CollRes_XSL_2021_h &h_p, element_t m_p, ID_B_CollRes_XSL_2021_h &h, element_t m, ID_B_CollRes_XSL_2021_tk &tk);
                        
        bool Verify(ID_B_CollRes_XSL_2021_h &h_p, element_t m_p, element_t I, ID_B_CollRes_XSL_2021_pp &pp);

        ~ID_B_CollRes_XSL_2021();

        enum {
            g,
            g1,
            g2,
            u
        };

        enum {
            s
        };

        enum {
            tk1,
            tk2
        };

        enum {
            h1
        };

        enum {
            r1,
            r2
        };
};


#endif //ID_B_CollRes_XSL_2021_H