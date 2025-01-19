#ifndef CHAMELIB_PBCSCHEME_H
#define CHAMELIB_PBCSCHEME_H

#include <pbc.h>

class PbcScheme{
    protected:
        element_s *G1, *G2, *GT, *Zn;

        // temporary variables
        element_t tmp_G, tmp_G_2, tmp_G_3, tmp_G_4;
        element_t tmp_H, tmp_H_2, tmp_H_3, tmp_H_4;
        element_t tmp_GT, tmp_GT_2, tmp_GT_3, tmp_GT_4;
        element_t tmp_Zn, tmp_Zn_2, tmp_Zn_3, tmp_Zn_4;

    public:
        PbcScheme(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn): G1(_G1), G2(_G2), GT(_GT), Zn(_Zn){
            element_init_same_as(this->tmp_G, G1);
            element_init_same_as(this->tmp_G_2, G1);
            element_init_same_as(this->tmp_G_3, G1);
            element_init_same_as(this->tmp_G_4, G1);

            element_init_same_as(this->tmp_H, G2);
            element_init_same_as(this->tmp_H_2, G2);
            element_init_same_as(this->tmp_H_3, G2);
            element_init_same_as(this->tmp_H_4, G2);

            element_init_same_as(this->tmp_GT, GT);
            element_init_same_as(this->tmp_GT_2, GT);
            element_init_same_as(this->tmp_GT_3, GT);
            element_init_same_as(this->tmp_GT_4, GT);

            element_init_same_as(this->tmp_Zn, Zn);
            element_init_same_as(this->tmp_Zn_2, Zn);
            element_init_same_as(this->tmp_Zn_3, Zn);
            element_init_same_as(this->tmp_Zn_4, Zn);
        }

        virtual ~PbcScheme(){
            element_clear(this->tmp_G);
            element_clear(this->tmp_G_2);
            element_clear(this->tmp_G_3);
            element_clear(this->tmp_G_4);

            element_clear(this->tmp_H);
            element_clear(this->tmp_H_2);
            element_clear(this->tmp_H_3);
            element_clear(this->tmp_H_4);

            element_clear(this->tmp_GT);
            element_clear(this->tmp_GT_2);
            element_clear(this->tmp_GT_3);
            element_clear(this->tmp_GT_4);

            element_clear(this->tmp_Zn);
            element_clear(this->tmp_Zn_2);
            element_clear(this->tmp_Zn_3);
            element_clear(this->tmp_Zn_4);
        }
};

#endif  // CHAMELIB_PBCSCHEME_H