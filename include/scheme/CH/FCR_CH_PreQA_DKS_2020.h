#ifndef IMPORT_ELEMENTLIST
#define IMPORT_ELEMENTLIST
#include "base/ElementList.h"
#endif //IMPORT_ELEMENTLIST

#ifndef IMPORT_UTIL_FUNC
#define IMPORT_UTIL_FUNC
#include "utils/func.h"
#endif //IMPORT_UTIL_FUNC


#ifndef FCR_CH_PreQA_DKS_2020_H
#define FCR_CH_PreQA_DKS_2020_H

#include <stdexcept>  // 包含 std::invalid_argument

class FCR_CH_PreQA_DKS_2020 {
    protected:
    element_t *G1, *G2, *Zn, *GT;
    element_t tmp_G1, tmp_G1_2,tmp_G1_3, tmp_G2, tmp_Zn,tmp_Zn_2, tmp_GT,tmp_GT_2,tmp_GT_3;
    element_t tmp_y;

    element_t g1;
    element_t g2;
    element_t x;  // secret x ∈ Zp

    element_t xi,k11,k12,k2,e2,s2;
    element_t u1,u2,e,e1,s11,s12;

// 
    

    //
    element_t a;


    public:
    FCR_CH_PreQA_DKS_2020(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT);

    void PG();

    void KG(element_t *y);

    void H(element_t *y, element_t *h, element_t *m,element_t *u1,element_t *u2, element_t *res);

    void Hash(element_t *m, element_t *y, 
                                element_t *h,
                                element_t *_e1, element_t *_e2, element_t *_s11, element_t *_s12, element_t *_s2);

    bool Check(element_t *y, element_t *m, element_t *h,
                                element_t *_e1, element_t *_e2, element_t *_s11, element_t *_s12, element_t *_s2);

    void Forge(element_t *m, element_t *m_p, 
                                element_t *_e1, element_t *_e2, element_t *_s11, element_t *_s12, element_t *_s2,
                                element_t *h,
                                element_t *e1_p,element_t *e2_p,element_t *s11_p,element_t *s12_p,element_t *s2_p);

    bool Verify(element_t *y, element_t *m_p, element_t *h,
                                 element_t *e1_p,element_t *e2_p,element_t *s11_p,element_t *s12_p,element_t *s2_p);

    bool Verify2(element_t *y, element_t *m, element_t *m_p);

    ~FCR_CH_PreQA_DKS_2020();
};


#endif //FCR_CH_PreQA_DKS_2020_H