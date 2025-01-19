#ifndef IMPORT_ELEMENTLIST
#define IMPORT_ELEMENTLIST
#include "base/ElementList.h"
#endif //IMPORT_ELEMENTLIST

#ifndef IMPORT_UTIL_FUNC
#define IMPORT_UTIL_FUNC
#include "utils/func.h"
#endif //IMPORT_UTIL_FUNC


#ifndef CH_KEF_DLP_LLA_2012_H
#define CH_KEF_DLP_LLA_2012_H

#include <stdexcept>  // 包含 std::invalid_argument

class CH_KEF_DLP_LLA_2012 {
    protected:
    element_t *G1, *G2, *Zn, *GT;
    element_t tmp_G1, tmp_G1_2, tmp_G2, tmp_Zn,tmp_Zn_2, tmp_GT,tmp_GT_2,tmp_GT_3;

    element_t g;  // 生成元g
    element_t y;
    element_t y1,w1;
    element_t t;
   

    public:

    struct pk{
        element_t y2;
        void Init(element_t *_G1){
            element_init_same_as(y2, *_G1);
        }
        ~pk(){
            element_clear(y2);
        }
    };
    
    struct sk{
        element_t a,x1,x2;
        void Init(element_t *_Zn){
            element_init_same_as(a, *_Zn);
            element_init_same_as(x1, *_Zn);
            element_init_same_as(x2, *_Zn);
        }
        ~sk(){
            element_clear(a);
            element_clear(x1);
            element_clear(x2);
        }
    };

    struct label{
        element_t L,R;
        void Init(element_t *_G1){
            element_init_same_as(L, *_G1);
            element_init_same_as(R, *_G1);
        }
        ~label(){
            element_clear(L);
            element_clear(R);
        }
    };

    CH_KEF_DLP_LLA_2012(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT);

    void LabelManager(element_t *y1, element_t *w1, element_t *t, label *label);

    void H1(element_t *m1, element_t *m2, element_t *m3, element_t *res);
    void H2(element_t *m, element_t *res);

    void PG();

    void KG(sk *sk, pk *pk, label *label);

    void Hash(pk *pk, element_t *m, element_t *r, label *label, element_t *S);

    bool Check(element_t *m, element_t *r, pk *pk, label *label, element_t *S);

    void hash_with_r(element_t *I, element_t *m, element_t *r1, element_t *r2, element_t *h);

    void UForge(sk *sk,pk *pk,label *label, element_t *S, element_t *m, element_t *m_p, element_t *r, element_t *r_p);
    void IForge(label *label, element_t *m, element_t *m_p, element_t *r, element_t *r_p, element_t *m_pp, element_t *r_pp);

    bool Verify(element_t *m_p, element_t *r_p, pk *pk, label *label, element_t *S);

    ~CH_KEF_DLP_LLA_2012();
};


#endif //CH_KEF_DLP_LLA_2012_H