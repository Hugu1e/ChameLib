#ifndef IMPORT_ELEMENTLIST
#define IMPORT_ELEMENTLIST
#include "base/ElementList.h"
#endif //IMPORT_ELEMENTLIST

#ifndef IMPORT_UTIL_FUNC
#define IMPORT_UTIL_FUNC
#include "utils/func.h"
#endif //IMPORT_UTIL_FUNC


#ifndef CH_KEF_DL_CZT_2011_H
#define CH_KEF_DL_CZT_2011_H

class CH_KEF_DL_CZT_2011 {
    protected:
    element_t *G1, *G2, *Zn, *GT;
    element_t P_pub, tmp_G1, tmp_G1_2, tmp_G1_hash, tmp_G2, tmp_Zn,tmp_Zn_2, tmp_GT; 
    element_t g;  // 生成元g
    element_t tmp_h;  // tmp_h = H(y,I)


    void H(element_t *gs, element_t *m, element_t *res);
    

    // void H_G1(element_t *m, element_t *res);

    // void base_hash(element_t *ID, element_t *L, element_t *H, element_t *m, element_t *r_1, element_t *r_2);

    public:
    CH_KEF_DL_CZT_2011(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT);

    // int public_key_size() {
    //     return CountSize(this->P) + CountSize(this->P_pub);
    // };

    void PG();

    void KG(element_t *x, element_t *y);

    void Hash(element_t *L, element_t *m, element_t *r_1, element_t *r_2, element_t *a, element_t *y, element_t *h);

    void Forge(element_t *h, element_t *x, element_t *L, element_t *m, element_t *m_p, element_t *r_1, element_t *r_2, element_t *r_1_p, element_t *r_2_p);

    bool Verify(element_t *h, element_t *L,element_t *m_p, element_t *r_1_p, element_t *x);

    ~CH_KEF_DL_CZT_2011();
};


#endif //CH_KEF_DL_CZT_2011_H