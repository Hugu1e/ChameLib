#ifndef IMPORT_ELEMENTLIST
#define IMPORT_ELEMENTLIST
#include "base/ElementList.h"
#endif //IMPORT_ELEMENTLIST

#ifndef IMPORT_UTIL_FUNC
#define IMPORT_UTIL_FUNC
#include "utils/func.h"
#endif //IMPORT_UTIL_FUNC


#ifndef IB_CH_MD_LSX_2022_H
#define IB_CH_MD_LSX_2022_H

class IB_CH_MD_LSX_2022 {
    protected:
    element_t *G1, *G2, *Zn, *GT;
    element_t tmp_G1, tmp_G1_2, tmp_G2, tmp_Zn, tmp_Zn_2, tmp_Zn_3, tmp_GT,tmp_GT_2,tmp_GT_3,tmp_GT_hash;
    element_t g;  // 生成元
    element_t a,b;  // α,β ∈ Zp
    element_t g1,g2;
    element_t egg,eg2g;

    public:
    IB_CH_MD_LSX_2022(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT);

    void PG();

    void KG(element_t *L, element_t *t, element_t *td1, element_t *td2);

    void Hash(element_t *h, element_t *L, element_t *m, element_t *r_1, element_t *r_2);

    void Forge(element_t *h, element_t *m, element_t *r_1, element_t *r_2, element_t *m_p, element_t *r_1_p, element_t *r_2_p, element_t *td1, element_t *td2);

    bool Verify(element_t *h, element_t *m_p, element_t *r_1_p, element_t *r_2_p, element_t *L);

    ~IB_CH_MD_LSX_2022();
};


#endif //IB_CH_MD_LSX_2022_H