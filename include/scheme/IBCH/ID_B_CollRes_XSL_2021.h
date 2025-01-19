#ifndef IMPORT_ELEMENTLIST
#define IMPORT_ELEMENTLIST
#include "base/ElementList.h"
#endif //IMPORT_ELEMENTLIST

#ifndef IMPORT_UTIL_FUNC
#define IMPORT_UTIL_FUNC
#include "utils/func.h"
#endif //IMPORT_UTIL_FUNC


#ifndef ID_B_CollRes_XSL_2021_H
#define ID_B_CollRes_XSL_2021_H

#include <stdexcept>  // 包含 std::invalid_argument

class ID_B_CollRes_XSL_2021 {
    protected:
    element_t *G1, *G2, *Zn, *GT;
    element_t tmp_G1, tmp_G1_2, tmp_G2, tmp_Zn,tmp_Zn_2, tmp_GT,tmp_GT_2,tmp_GT_3;

    element_t g;  // 生成元g
    element_t a;  // secret α ∈ Zp
    element_t g1,g2;  // g1= g^α, g2 ∈ G

    unsigned long int n; // n>=1
    element_t *array_u;  // u0,u1,...,un

    element_t t;  // t ∈ Zp

    element_t tmp;  // u0*(u1^I1 * u2^I2 * ... * un^In)


    public:
    ID_B_CollRes_XSL_2021(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT);

    void PG(unsigned long int n, element_t *msk);

    void KG(element_t *msk, element_t *I, element_t *tk1, element_t *tk2);

    bool getBit(element_t *element, unsigned long int index);

    void Hash(element_t *I, element_t *m, element_t *h, element_t *r1, element_t *r2);

    void hash_with_r(element_t *I, element_t *m, element_t *r1, element_t *r2, element_t *h);

    void Forge(element_t *tk1, element_t *tk2, 
                                    element_t *h, element_t *m,
                                    element_t *r1, element_t *r2, 
                                    element_t *m_p,
                                    element_t *r1_p, element_t *r2_p);

    bool Verify(element_t *I, element_t *m_p, element_t *r1_p, element_t *r2_p, element_t *h);

    ~ID_B_CollRes_XSL_2021();
};


#endif //ID_B_CollRes_XSL_2021_H