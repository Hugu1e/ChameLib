#ifndef IMPORT_ELEMENTLIST
#define IMPORT_ELEMENTLIST
#include "base/ElementList.h"
#endif //IMPORT_ELEMENTLIST

#ifndef IMPORT_UTIL_FUNC
#define IMPORT_UTIL_FUNC
#include "utils/func.h"
#endif //IMPORT_UTIL_FUNC


#ifndef IB_CH_KEF_CZS_2014_H
#define IB_CH_KEF_CZS_2014_H

#include <stdexcept>  // 包含 std::invalid_argument

class IB_CH_KEF_CZS_2014 {
    protected:
    element_t *G1, *G2, *Zn, *GT;
    element_t tmp_G1, tmp_G1_2,tmp_G1_3,tmp_G1_4, tmp_G2, tmp_Zn,tmp_Zn_2, tmp_GT,tmp_GT_2,tmp_GT_3;

    element_t P;  // generator of G1
    element_t x;
    element_t Ppub;

    element_t SID;
    element_t QID;
    element_t a;


    public:
    IB_CH_KEF_CZS_2014(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT);

    void PG(element_t *_x, element_t *_Ppub);

    void KG(element_t *_x, element_t *ID, element_t *SID);

    void H(element_t *m, element_t *res);

    void Hash(element_t *ID, element_t *L, element_t *m, element_t *r1, element_t *r2, element_t *h);

    bool Check(element_t *h, element_t *L,element_t *m, element_t *r1);

    void Forge(element_t *SID, element_t *ID, element_t *L, element_t *h, element_t *m, element_t *r1, element_t *r2, element_t *m_p, 
                                element_t *r1_p, element_t *r2_p);

    bool Verify(element_t *h, element_t *L,element_t *m_p, element_t *r1_p);


    ~IB_CH_KEF_CZS_2014();
};


#endif //IB_CH_KEF_CZS_2014_H