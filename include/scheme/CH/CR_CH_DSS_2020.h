#ifndef IMPORT_ELEMENTLIST
#define IMPORT_ELEMENTLIST
#include "base/ElementList.h"
#endif //IMPORT_ELEMENTLIST

#ifndef IMPORT_UTIL_FUNC
#define IMPORT_UTIL_FUNC
#include "utils/func.h"
#endif //IMPORT_UTIL_FUNC


#ifndef CR_CH_DSS_2020_H
#define CR_CH_DSS_2020_H

#include <stdexcept>  // 包含 std::invalid_argument

class CR_CH_DSS_2020 {
    protected:
    element_t *G1, *G2, *Zn, *GT;
    element_t tmp_G1, tmp_G1_2,tmp_G1_3,tmp_G1_4, tmp_G2, tmp_Zn,tmp_Zn_2, tmp_GT,tmp_GT_2,tmp_GT_3;
    element_t tmp_y;

    element_t g;
    element_t x,y;

    element_t xi,k1,e2,s2;
    element_t u11,u12,u2,e,e1,s1;
    element_t k2;


    public:
    CR_CH_DSS_2020(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT);

    void PG();

    void KG(element_t *_x, element_t *_y);

    void H(element_t *y, element_t *h1, element_t *h2, element_t *m,
                                element_t *u11,element_t *u12,element_t *u2, 
                                element_t *res);

    void Hash(element_t *_y, element_t *m, 
                                element_t *h1,element_t *h2,
                                element_t *_e1, element_t *_e2, element_t *_s1, element_t *_s2);

    bool Check(element_t *_y, element_t *m, 
                            element_t *h1, element_t *h2,
                            element_t *_e1, element_t *_e2, element_t *_s1, element_t *_s2);

    void Forge(element_t *_x, element_t *m, element_t *m_p, 
                                element_t *_e1, element_t *_e2, element_t *_s1, element_t *_s2,
                                element_t *h1, element_t *h2,
                                element_t *_e1_p,element_t *_e2_p,element_t *_s1_p,element_t *_s2_p);

    bool Verify(element_t *_y, element_t *m_p, element_t *h1, element_t *h2,
                                 element_t *_e1_p,element_t *_e2_p,element_t *_s1_p,element_t *_s2_p);


    ~CR_CH_DSS_2020();
};


#endif //CR_CH_DSS_2020_H