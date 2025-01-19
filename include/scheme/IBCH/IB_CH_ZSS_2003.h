#ifndef IMPORT_UTIL_FUNC
#define IMPORT_UTIL_FUNC
#include "utils/func.h"
#endif //IMPORT_UTIL_FUNC


#ifndef IB_CH_ZSS_2003_H
#define IB_CH_ZSS_2003_H

class IB_CH {
    protected:
    element_t *G1, *G2, *Zn, *GT, P, P_pub, msk, tmp_G1, tmp_G2, tmp_Zn, tmp_Zn_2, tmp_GT, tmp_GT_2;
    int rev_G1G2;
    
    void H0(element_t &m, element_t &res);

    void H1(element_t &m, element_t &res);

    void H2(element_t &m, element_t &res);

    public:
    IB_CH(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT, int _rev_G1G2);

    void Setup();

    ~IB_CH();
};

class IB_CH_S1: public IB_CH {
    protected:
    void base_hash(element_t *H, element_t *R, element_t *ID, element_t *m);

    public:
    IB_CH_S1(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT, int _rev_G1G2): IB_CH(_G1, _G2, _Zn, _GT, _rev_G1G2) {};

    int public_key_size() {
        return CountSize(this->P) + CountSize(this->P_pub);
    };

    void Extract(element_t *S_ID, element_t *ID);

    void Hash(element_t *H, element_t *R, element_t *ID, element_t *m);

    bool Verify(element_t *H, element_t *R, element_t *ID, element_t *m);

    void Forge(element_t *ID, element_t *S_ID, element_t *m, element_t *m_p, element_t *R, element_t *R_p);
};

class IB_CH_S2: public IB_CH {
    protected:
    element_t P_1;

    void base_hash(element_t *H, element_t *R, element_t *ID, element_t *m);

    public:
    int public_key_size() {
        if(this->rev_G1G2 < 0) return CountSize(this->P) + CountSize(this->P_pub); 
        else return CountSize(this->P) + CountSize(this->P_pub) + CountSize(this->P_1);
    };

    IB_CH_S2(element_t *_G1, element_t *_G2, element_t *_Zn, element_t *_GT, int _rev_G1G2): IB_CH(_G1, _G2, _Zn, _GT, _rev_G1G2) {
        element_init_same_as(this->P_1, *this->G1);
    };

    void Setup();

    void Extract(element_t *S_ID, element_t *ID);

    void Hash(element_t *H, element_t *R, element_t *ID, element_t *m);

    bool Verify(element_t *H, element_t *R, element_t *ID, element_t *m);

    void Forge(element_t *ID, element_t *S_ID, element_t *m, element_t *m_p, element_t *R, element_t *R_p);

    ~IB_CH_S2();
};

#endif //IB_CH_ZSS_2003_H